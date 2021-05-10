#include "XVideoPlayer.h"
#include <QResizeEvent>
#include <QApplication>
#include <QScreen>

void on_Media_Event_V(const libvlc_event_t *event, void *data)
{
	XVideoPlayer * player = static_cast<XVideoPlayer*>(data);
	player->EventCallBack(event);
}

XVideoPlayer::XVideoPlayer(QWidget *parent)
	: QWidget(parent)
{
	if (nullptr == m_PlayWidget)
	{
		m_PlayWidget = new QWidget(this);
		m_PlayWidget->installEventFilter(this);
		m_PlayWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::MSWindowsOwnDC);
	}
	connect(this, SIGNAL(StateChanged(int)), SLOT(OnStateChanged(int)));

}

XVideoPlayer::~XVideoPlayer()
{
	ReleasePlayer();
}

bool XVideoPlayer::Open(QString url)
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
	libvlc_event_attach(mgr, libvlc_MediaDurationChanged, on_Media_Event_V, this);
	libvlc_media_player_set_media(m_pVlcPlayer, vlcMedia);
	/* Release the media */
	libvlc_media_release(vlcMedia);
	m_pState = ps_Opened;
	return true;
}

bool XVideoPlayer::Play()
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
		//m_PlayWidget->show();
		return true;
	}
	else
		return false;
}

void XVideoPlayer::Pause()
{
	if (m_pVlcPlayer && libvlc_media_player_is_playing(m_pVlcPlayer))
	{
		libvlc_media_player_pause(m_pVlcPlayer);
	}
}

void XVideoPlayer::Stop()
{
	m_pState = ps_Stoped;
	if (m_bFullScreen)
	{
		SetFullScreen(false);
	}
	if (m_pVlcPlayer)
	{
		/* stop the media player */
		libvlc_media_player_stop(m_pVlcPlayer);
	}
	//m_PlayWidget->hide();
}

void XVideoPlayer::ChangeVolume(int )
{

}

//void XVideoPlayer::DurationChanged4CB(int64_t dur)
//{
//	m_Duration = dur;
//	emit DurationChanged(dur);
//}
//
//void XVideoPlayer::StateChanged4CB(PlayerState ps)
//{
//	switch (ps)
//	{
//	case XVideoPlayer::ps_Opened:
//		m_pState = ps_Opened;
//		break;
//	case XVideoPlayer::ps_Playing:
//		AdjusetSize();
//		m_pState = ps_Playing;
//		break;
//	case XVideoPlayer::ps_Paused:
//		m_pState = ps_Paused;
//		break;
//	case XVideoPlayer::ps_Stoped:
//		m_pState = ps_Stoped;
//		break;
//	default:
//		break;
//	}
//	emit StateChanged(ps);
//}
//
//void XVideoPlayer::TimeChanged4CB(qint64 time)
//{
//	emit PositionChanged(time);
//}

void XVideoPlayer::SetPosition(qint64 pos)
{
	libvlc_media_player_set_time(m_pVlcPlayer, pos);
}

bool XVideoPlayer::IsFullScreen()
{
	return m_bFullScreen;
}

void XVideoPlayer::SetFullScreen(bool bfs)
{
	if (bfs)
	{
		auto ss = QApplication::screens();
		if (ss.isEmpty())
		{
			return;
		}
		//libvlc_video_set_scale(m_pVlcPlayer, 0);
		m_bFullScreen = true;
		//m_PlayWidget->setParent(nullptr);
		m_PlayWidget->setFixedSize(ss.first()->size());
		m_PlayWidget->move(0, 0);
		m_PlayWidget->show();
	}
	else
	{
		//m_PlayWidget->setParent(this);
		m_bFullScreen = false;
		m_PlayWidget->show();
		AdjusetSize();
	}
	emit FullScreen(bfs);
}

void XVideoPlayer::EnableFullScreen(bool bEnable)
{
	m_bEnableFullScreen = bEnable;
	if (!bEnable && m_bFullScreen)
	{
		SetFullScreen(false);
	}
}

XVideoPlayer::PlayerState XVideoPlayer::State()
{
	return m_pState;
}

void XVideoPlayer::EventCallBack(const libvlc_event_t * event)
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

void XVideoPlayer::OnStateChanged(int ps)
{
	switch (ps)
	{
	case ps_Playing:
		AdjusetSize();
		break;
	default:
		break;
	}
}

void XVideoPlayer::resizeEvent(QResizeEvent * )
{
	AdjusetSize();
}

void XVideoPlayer::hideEvent(QHideEvent * )
{
	if (m_pState != ps_Stoped)
	{
		Stop();
	}
	m_PlayWidget->hide();
}

bool XVideoPlayer::eventFilter(QObject * watched, QEvent * event)
{
	if (m_PlayWidget == watched)
	{
		switch (event->type())
		{
		case QEvent::MouseButtonPress:			//单击暂停、播放
			if (m_pVlcPlayer)
			{
				if (0 == m_PauseTimerId)
				{
					m_PauseTimerId = startTimer(300);	
				}
			}
			event->accept();
			return true;
			break;
		case QEvent::MouseButtonDblClick:
			if (m_PauseTimerId)
			{
				killTimer(m_PauseTimerId);
				m_PauseTimerId = 0;
			}
			SetFullScreen(!m_bFullScreen);
			event->accept();
			return true;
			break;
		case QEvent::MouseMove:
		case QEvent::MouseButtonRelease:
			event->accept();
			return true;
			break;
		default:
			break;
		}
	}
	else if (event->type() == QEvent::Move)
	{
		QPoint topLeft = {(width() - m_PlayWidget->width()) / 2,(height() - m_PlayWidget->height()) / 2};
		m_PlayWidget->move(mapToGlobal(topLeft));
	}
	return false;
}

void XVideoPlayer::timerEvent(QTimerEvent * te)
{
	if (te->timerId() == m_PauseTimerId)
	{
		if (libvlc_media_player_is_playing(m_pVlcPlayer))
			Pause();
		else
			Play();
		killTimer(m_PauseTimerId);
		m_PauseTimerId = 0;
	}
}

void XVideoPlayer::showEvent(QShowEvent * se)
{
	//本窗口最顶级的窗口，监听此窗口的移动消息
	window()->installEventFilter(this);
}


void XVideoPlayer::AdjusetSize()
{
	//调整播放器尺寸以消除黑边
	if (m_pVlcPlayer && !m_bFullScreen && !isHidden())
	{
		int vw;// = libvlc_video_get_width(m_pVlcPlayer);
		int vh;// = libvlc_video_get_height(m_pVlcPlayer);

		if (libvlc_video_get_size(m_pVlcPlayer,0,(unsigned int*)&vw, (unsigned int*)&vh) != 0)
		{
			return;
		}
		double dw = vw ;
		double dh = vh ;
		int w = size().width() ;
		int h = size().height();
		float r1 = w*1.0 / dw;
		float r2 = h*1.0 / dh;
		int tn = r1 * 1000;
		r1 = tn / 1000.0;
		tn = r2 * 1000;
		r2 = tn / 1000.0;
		QPoint off;
		float rr = qMin(r1, r2);
		if (r1 < r2)
		{
			//libvlc_video_set_scale(m_pVlcPlayer, r1);
			//r1 = libvlc_video_get_scale(m_pVlcPlayer);
			//zoomNum = w;
			//zoomDen = vw;
			////vw = w;
			//int rh = qRound((double)vh * zoomNum / zoomDen);
			//vw = qRound((double)rh * vw / vh) ;
			//vh = rh;
			//off = { 0,(h - vh) / 2 };
			//libvlc_video_get_size(m_pVlcPlayer, 0, (unsigned int*)&vw, (unsigned int*)&vh);
			double rw = w;
			double rh = rw / dw * dh +0.45;
			rw = ((int)rh) / dh * dw +0.45;
			m_PlayWidget->setFixedSize( rw, rh);
			vw = rw;
			vh = rh;
		}
		else
		{
			//libvlc_video_set_scale(m_pVlcPlayer, r2);
			//r2 = libvlc_video_get_scale(m_pVlcPlayer);
			//zoomNum = h;
			//zoomDen = vh;
			////vh = h;
			//int rw = qRound((double)vw * zoomNum / zoomDen);
			//vh = qRound((double)rw * vh / vw ) ;
			//vw = rw;
			//off = { (w - vw) / 2,0 };
			//libvlc_video_get_size(m_pVlcPlayer, 0, (unsigned int*)&vw, (unsigned int*)&vh);
			double rh = h;
			double rw = rh / dh * dw +0.45;
			rh = ((int)rw) / dw * dh +0.45;
			m_PlayWidget->setFixedSize( rw, rh );
			vw = rw;
			vh = rh;
		}
		//libvlc_video_set_scale(m_pVlcPlayer, rr);
		/*vw = rr * dw;
		vh = rr * dh;
		m_PlayWidget->setFixedSize(vw, vh);*/
		off = { (w - vw) / 2, (h - vh) / 2 };
		m_PlayWidget->move(mapToGlobal(off));
		if (m_pState != ps_Stoped)
		{
			m_PlayWidget->show();
		}
		else
			m_PlayWidget->hide();
	}
}

void XVideoPlayer::CreatePlayer()
{
	if (nullptr != m_pVlcPlayer)
	{
		libvlc_media_player_release(m_pVlcPlayer);
	}
	m_pVlcPlayer = libvlc_media_player_new(g_pVlcInstance);

	libvlc_event_manager_t *mgr = libvlc_media_player_event_manager(m_pVlcPlayer);
	//添加回调
	libvlc_event_attach(mgr, libvlc_MediaPlayerOpening, on_Media_Event_V, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerPlaying, on_Media_Event_V, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerStopped, on_Media_Event_V, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerPaused, on_Media_Event_V, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerTimeChanged, on_Media_Event_V, this);

	/* Integrate the video in the interface */
#if defined(Q_OS_MAC)
	libvlc_media_player_set_nsobject(m_pVlcPlayer, (void*)m_pWidget->winId());
#elif defined(Q_OS_UNIX)
	libvlc_media_player_set_xwindow(m_pVlcPlayer, m_pWidget->winId());
#elif defined(Q_OS_WIN)
	libvlc_media_player_set_hwnd(m_pVlcPlayer, (void*)m_PlayWidget->winId());
#endif
	libvlc_video_set_mouse_input(m_pVlcPlayer, 0);
	libvlc_video_set_key_input(m_pVlcPlayer, 0);
	//libvlc_video_set_scale(m_pVlcPlayer, 0);		
	//设置音量
	libvlc_audio_set_volume(m_pVlcPlayer, 90);
	
}

void XVideoPlayer::ReleasePlayer()
{
	if (m_pVlcPlayer)
	{
		/* release the media player */
		libvlc_media_player_release(m_pVlcPlayer);
		/* Reset application values */
		m_pVlcPlayer = NULL;
	}
}
