#include "XVideoPlayer2.h"
#include <QResizeEvent>
#include <QBoxLayout>
#include <QWidget>
#include <QPainter>

#if defined(Q_OS_MAC)       // Mac
// TODO
#elif defined(Q_OS_UNIX)    // Linux
#pragma comment(lib, "libvlc.a")
#elif defined(Q_OS_WIN)     // Windows
#pragma comment(lib, "libvlc.lib")
#endif

static void *lock(void *opaque, void **planes)
{
	struct Context *ctx = static_cast<Context*>(opaque);
	//int vw;
	//int vh;
	//if (libvlc_video_get_size(ctx->player, 0, (unsigned int*)&vw, (unsigned int*)&vh) != 0)
	//	return nullptr;

	ctx->mutex.lock();

	// 告诉 VLC 将解码的数据放到缓冲区中
	*planes = ctx->pixels;

	return nullptr;
}

// 获取 argb 图片并保存到文件中
static void unlock(void *opaque, void *picture, void *const *planes)
{
	Q_UNUSED(picture);

	struct Context *ctx = static_cast<Context*>(opaque);
	unsigned char *data = static_cast<unsigned char *>(*planes);
	//static int frameCount = 1;

	//QImage image(data, ctx->width, ctx->height, QImage::Format_ARGB32);
	//image.save(QString("frame_%1.png").arg(frameCount++));

	ctx->mutex.unlock();
	//ctx->displayWidget->update();
}

static void display(void *opaque, void *picture)
{
	Q_UNUSED(picture);

	struct Context *ctx = static_cast<Context*>(opaque);
	ctx->displayWidget->update();
}

static unsigned format_cb(void **opaque, char *chroma,
	unsigned *width, unsigned *height,
	unsigned *pitches,
	unsigned *lines)
{
	memcpy(chroma, "RV32", 4);
	struct Context *ctx = static_cast<Context*>(*opaque);
	ctx->mutex.lock();
	ctx->height = *height;
	ctx->width = *width;

	(*pitches) = *width * 4;
	(*lines) = *height;

	ctx->pixels = new uchar[ctx->width * ctx->height * 4];
	memset(ctx->pixels, 0, ctx->width * ctx->height * 4);
	qDebug() << "format_cb";
	ctx->mutex.unlock();
	return 1;
}

void cleanup_cb(void *opaque)
{
	qDebug() << "cleanup_cb";
	struct Context *ctx = static_cast<Context*>(opaque);
	ctx->mutex.lock();
	if (nullptr != ctx->pixels)
	{
		delete[] ctx->pixels;
		ctx->pixels = nullptr;
	}
	ctx->mutex.unlock();
}

void on_Media_Event_V2(const libvlc_event_t *event, void *data)
{
	XVideoPlayer2 * player = static_cast<XVideoPlayer2*>(data);
	player->EventCallBack(event);
}

XVideoPlayer2::XVideoPlayer2(QObject *parent)
	: QObject(parent)
{
	
}

XVideoPlayer2::~XVideoPlayer2()
{
	m_ctx.mutex.lock();
	if (m_ctx.pixels)
	{
		delete[] m_ctx.pixels;
		m_ctx.pixels = nullptr;
	}
	m_ctx.mutex.unlock();
	ReleasePlayer();
	if (m_StopImage)
	{
		delete m_StopImage;
	}
}

bool XVideoPlayer2::Open(QString url)
{
	if (m_pState != ps_Stoped)
	{
		Stop();
	}
	CreatePlayer();
	//m_bStoped = false;
	if (!url.isEmpty())
	{
		m_VideoUrl = url;
		m_VideoUrl.replace('/', '\\');
	}
	auto funcOpen = libvlc_media_new_path;
	if (m_VideoUrl.startsWith("rtsp://") ||
		m_VideoUrl.startsWith("rtmp://") ||
		m_VideoUrl.startsWith("rtp://") ||
		m_VideoUrl.startsWith("tcp://") ||
		m_VideoUrl.startsWith("udp://") ||
		m_VideoUrl.startsWith("ftp://") ||
		m_VideoUrl.startsWith("http://") ||
		m_VideoUrl.startsWith("https://"))
	{
		funcOpen = libvlc_media_new_location;
	}

	/* Create a new Media */
	libvlc_media_t *vlcMedia = funcOpen(g_pVlcInstance, m_VideoUrl.toStdString().c_str());
	/* Fail to open url*/
	if (!vlcMedia)
	{
		return false;
	}

	if (QString(m_VideoUrl).endsWith(".hevc"))
		libvlc_media_add_option(vlcMedia, ":demux=hevc");
	else if (QString(m_VideoUrl).endsWith("rtsp"))
		libvlc_media_add_option(vlcMedia, ":rtsp-tcp");

	//获取时长的回调
	libvlc_event_manager_t* mgr = libvlc_media_event_manager(vlcMedia);
	libvlc_event_attach(mgr, libvlc_MediaDurationChanged, on_Media_Event_V2, this);
	libvlc_media_player_set_media(m_pVlcPlayer, vlcMedia);
	/* Release the media */
	libvlc_media_release(vlcMedia);
	m_pState = ps_Opened;
	return true;
}

bool XVideoPlayer2::Play()
{
	if (m_pState == ps_Stoped)
	{
		Open();
	}
	//设置音量
	//libvlc_audio_set_volume(m_pVlcPlayer, 0);
	/* And start playback */
	int vlc_ret = libvlc_media_player_play(m_pVlcPlayer);
	//AdjusetSize();
	if (0 == vlc_ret)
	{
		return true;
	}
	else
		return false;
}

void XVideoPlayer2::Pause()
{
	if (m_pVlcPlayer && libvlc_media_player_is_playing(m_pVlcPlayer))
	{
		libvlc_media_player_pause(m_pVlcPlayer);
	}
}

void XVideoPlayer2::Stop()
{
	m_pState = ps_Stoped;
	if (m_pVlcPlayer)
	{
		/* stop the media player */
		libvlc_media_player_stop(m_pVlcPlayer);
	}
	//m_PlayWidget->hide();
	m_ctx.displayWidget->update();
}

void XVideoPlayer2::ChangeVolume(int)
{

}

void XVideoPlayer2::SetPosition(qint64 pos)
{
	libvlc_media_player_set_time(m_pVlcPlayer, pos);
}

XVideoPlayer2::PlayerState XVideoPlayer2::State()
{
	return m_pState;
}

void XVideoPlayer2::EventCallBack(const libvlc_event_t * event)
{
	switch (event->type)
	{
	case libvlc_MediaPlayerOpening:
		m_pState = ps_Opened;
		emit StateChanged(m_pState);
		break;
	case libvlc_MediaPlayerPlaying:
		m_pState = ps_Playing;
		emit StateChanged(m_pState);
		break;
	case libvlc_MediaPlayerPaused:
		m_pState = ps_Paused;
		emit StateChanged(m_pState);
		break;
	case libvlc_MediaPlayerStopped:
		m_pState = ps_Stoped;
		emit StateChanged(m_pState);
		break;
	case libvlc_MediaDurationChanged:
		m_Duration = event->u.media_duration_changed.new_duration;
		emit DurationChanged(m_Duration);
		break;
	case libvlc_MediaPlayerTimeChanged:
		emit PositionChanged(event->u.media_player_time_changed.new_time);
		break;
	default:
		break;
	}
}

void XVideoPlayer2::SetDisplayWidget(QWidget * dw)
{
	dw->installEventFilter(this);
	m_ctx.displayWidget = dw;
}

void XVideoPlayer2::SetBgColor(QColor c)
{
	m_BgColor = c;
}

void XVideoPlayer2::SetEndImage(QString img)
{
	if (m_StopImage)
	{
		delete m_StopImage;
	}
	m_StopImage = new QImage();
	if (!m_StopImage->load(img))
	{
		delete m_StopImage;
		m_StopImage = nullptr;
	}
}

bool XVideoPlayer2::eventFilter(QObject * watched, QEvent * event)
{
	if ((QObject *)m_ctx.displayWidget == watched)
	{
		switch (event->type())
		{
		case QEvent::MouseButtonPress:			//单击暂停、播放		
			//event->accept();
			//return true;
			break;
		case QEvent::MouseButtonDblClick:
			//event->accept();
			//return true;
			break;
		case QEvent::MouseMove:
		case QEvent::MouseButtonRelease:
			//event->accept();
			//return true;
			break;
		case QEvent::Paint:
		{
			QPainter pt(m_ctx.displayWidget);
			pt.setRenderHints(QPainter::SmoothPixmapTransform);
			if (m_pState == ps_Stoped && m_StopImage)
			{
				pt.drawImage(m_ctx.displayWidget->rect(), *m_StopImage);
			}
			else
			{
				QRect rect = m_ctx.displayWidget->rect();
				pt.fillRect(rect, m_BgColor);
				m_ctx.mutex.lock();
				if (m_ctx.width * m_ctx.height > 0 && nullptr != m_ctx.pixels)
				{
					//计算，保持视频长宽比
					qreal s1 = (qreal)rect.width() / (qreal)m_ctx.width ;
					qreal s2 = (qreal)rect.height() / (qreal)m_ctx.height;
					QRect rectDest = rect;
					if (s1 < s2)
					{
						rectDest.setHeight(m_ctx.height*s1);
						rectDest.translate(0,(rect.height() - rectDest.height()) / 2);
					}
					else
					{
						rectDest.setWidth(m_ctx.width*s2);
						rectDest.translate((rect.width() - rectDest.width()) / 2 , 0);
					}
					
					QImage image(m_ctx.pixels, m_ctx.width, m_ctx.height, QImage::Format_RGB32);
					pt.drawImage(rectDest, image);
				}
				m_ctx.mutex.unlock();
			}
		}
			break;
		case QEvent::Destroy:
			Stop();
			m_ctx.mutex.lock();
			m_ctx.displayWidget = nullptr;
			m_ctx.mutex.unlock();
			break;
		default:
			break;
		}
	}
	return false;
}

void XVideoPlayer2::CreatePlayer()
{
	if (nullptr != m_pVlcPlayer)
	{
		libvlc_media_player_release(m_pVlcPlayer);
	}
	m_pVlcPlayer = libvlc_media_player_new(g_pVlcInstance);

	libvlc_event_manager_t *mgr = libvlc_media_player_event_manager(m_pVlcPlayer);
	//添加回调
	libvlc_event_attach(mgr, libvlc_MediaPlayerOpening, on_Media_Event_V2, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerPlaying, on_Media_Event_V2, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerStopped, on_Media_Event_V2, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerPaused, on_Media_Event_V2, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerTimeChanged, on_Media_Event_V2, this);

	m_ctx.player = m_pVlcPlayer;
	// 设置回调，用于提取帧或者在界面上显示。
	libvlc_video_set_callbacks(m_pVlcPlayer, lock, unlock, display, &m_ctx);
	//libvlc_video_set_format(m_pVlcPlayer, "RGBA", 1920, 1080, 1920 * 4);
	libvlc_video_set_format_callbacks(m_pVlcPlayer, format_cb, cleanup_cb);
	/* Integrate the video in the interface */

	libvlc_video_set_mouse_input(m_pVlcPlayer, 0);
	libvlc_video_set_key_input(m_pVlcPlayer, 0);
	//libvlc_video_set_scale(m_pVlcPlayer, 0);		
	//设置音量
	libvlc_audio_set_volume(m_pVlcPlayer, 90);
}

void XVideoPlayer2::ReleasePlayer()
{
	if (m_pVlcPlayer)
	{
		/* release the media player */
		libvlc_media_player_release(m_pVlcPlayer);
		/* Reset application values */
		m_pVlcPlayer = NULL;
	}
}
