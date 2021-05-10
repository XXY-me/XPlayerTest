#include "XAudioPlayer.h"
#include <qDebug>
#include <QTimerEvent>

//时长改变
void on_Media_Event(const libvlc_event_t *event, void *data)
{
	XAudioPlayer * player = static_cast<XAudioPlayer*>(data);
	player->EventCallBack(event);
}


XAudioPlayer::XAudioPlayer(QObject *parent)
	: QObject(parent)
{
	connect(this, SIGNAL(StateChanged(int)), SLOT(OnStateChanged(int)));
}

XAudioPlayer::~XAudioPlayer()
{
	if (m_pVlcPlayer)
	{
		/* release the media player */
		libvlc_media_player_release(m_pVlcPlayer);
		/* Reset application values */
		m_pVlcPlayer = NULL;
	}
}

bool XAudioPlayer::Open(QString file)
{
	//m_bStoped = false;
	if (!file.isEmpty())
	{
		m_AudioFile = file;
		m_AudioFile.replace('/', '\\');
	}
	if (m_state != ps_Stoped)
	{
		Stop();
	}
	CreatePlayer();
	auto funcOpen = libvlc_media_new_path;
	if (m_AudioFile.startsWith("rtsp://") ||
		m_AudioFile.startsWith("rtmp://") ||
		m_AudioFile.startsWith("rtp://") ||
		m_AudioFile.startsWith("tcp://") ||
		m_AudioFile.startsWith("udp://") ||
		m_AudioFile.startsWith("ftp://") ||
		m_AudioFile.startsWith("http://") ||
		m_AudioFile.startsWith("https://"))
	{
		funcOpen = libvlc_media_new_location;
	}

	/* Create a new Media */
	libvlc_media_t *vlcMedia = funcOpen(g_pVlcInstance, m_AudioFile.toStdString().c_str());
	/* Fail to open url*/
	if (!vlcMedia)
	{
		return false;
	}

	if (QString(m_AudioFile).endsWith(".hevc"))
		libvlc_media_add_option(vlcMedia, ":demux=hevc");
	else if (QString(m_AudioFile).endsWith("rtsp"))
		libvlc_media_add_option(vlcMedia, ":rtsp-tcp");

	/* Create a new libvlc player */
	//m_pVlcPlayer = libvlc_media_player_new_from_media(vlcMedia);
	//获取时长的回调
	libvlc_event_manager_t* mgr = libvlc_media_event_manager(vlcMedia);
	libvlc_event_attach(mgr, libvlc_MediaDurationChanged, on_Media_Event, this);

	libvlc_media_player_set_media(m_pVlcPlayer, vlcMedia);
	/* Release the media */
	libvlc_media_release(vlcMedia);
	m_state = ps_Opened;
	m_CurPos = 0;
	return true;
}

bool XAudioPlayer::Play()
{
	if (m_state == ps_Stoped)
	{
		Open();
	}
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

void XAudioPlayer::Pause()
{
	if (m_pVlcPlayer && libvlc_media_player_is_playing(m_pVlcPlayer))
	{
		libvlc_media_player_pause(m_pVlcPlayer);
	}
	if (m_CheckPosTimer > 0)
	{
		killTimer(m_CheckPosTimer);
		m_CheckPosTimer = 0;
	}
}

void XAudioPlayer::Stop()
{
	if (m_state == ps_Stoped )
	{
		return;
	}
	m_state = ps_Stoped;
	if (m_pVlcPlayer)
	{
		/* stop the media player */
		libvlc_media_player_stop(m_pVlcPlayer);
	}
	if (m_CheckPosTimer > 0)
	{
		killTimer(m_CheckPosTimer);
		m_CheckPosTimer = 0;
		m_CurPos = 0;
	}
	emit StateChanged(m_state);
}

void XAudioPlayer::EventCallBack(const libvlc_event_t * event)
{
	//m_bStoped = false;

	switch (event->type)
	{
	case libvlc_MediaPlayerOpening:
		m_state = ps_Opened;
		emit StateChanged(m_state);
		break;
	case libvlc_MediaPlayerPlaying:
		m_state = ps_Playing;
		emit StateChanged(m_state);
		break;
	case libvlc_MediaPlayerPaused:
		m_state = ps_Paused;
		emit StateChanged(m_state);
		break;
	case libvlc_MediaPlayerStopped:
		//m_bStoped = true;
		if (m_state != ps_Stoped)
		{
			m_state = ps_Stoped;
			emit StateChanged(m_state);
		}
		break;
	case libvlc_MediaDurationChanged:
		m_Duration = event->u.media_duration_changed.new_duration;
		emit DurationChanged(m_Duration);
		break;
	case libvlc_MediaPlayerTimeChanged:
		//调整pos计时器
		m_CheckClock = clock();
		m_CurPos = event->u.media_player_time_changed.new_time;
		break;

	default:
		break;
	}
}

XAudioPlayer::PlayerState XAudioPlayer::State()
{
	return m_state;
}

void XAudioPlayer::SetPlayPos(qint64 pos)
{
	if (m_pVlcPlayer)
	{
		libvlc_media_player_set_time(m_pVlcPlayer, pos);
		m_CurPos = pos;
		m_CheckClock = clock();
	}
}

void XAudioPlayer::SetNotifyInterval(int i)
{
	m_TimerInterval = i;
	if (m_state == ps_Playing)
	{
		if (m_CheckPosTimer)
		{
			killTimer(m_CheckPosTimer);
		}
		m_CheckPosTimer = startTimer(m_TimerInterval);
	}
}

void XAudioPlayer::CreatePlayer()
{
	if (nullptr != m_pVlcPlayer)
	{
		libvlc_media_player_release(m_pVlcPlayer);
	}
	m_pVlcPlayer = libvlc_media_player_new(g_pVlcInstance);

	libvlc_event_manager_t *mgr = libvlc_media_player_event_manager(m_pVlcPlayer);
	//添加回调
	libvlc_event_attach(mgr, libvlc_MediaPlayerOpening, on_Media_Event, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerPlaying, on_Media_Event, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerStopped, on_Media_Event, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerPaused, on_Media_Event, this);
	libvlc_event_attach(mgr, libvlc_MediaPlayerTimeChanged, on_Media_Event, this);

	libvlc_video_set_mouse_input(m_pVlcPlayer, 0);
	libvlc_video_set_key_input(m_pVlcPlayer, 0);
	//设置音量
	libvlc_audio_set_volume(m_pVlcPlayer, 100);
}

void XAudioPlayer::timerEvent(QTimerEvent * te)
{
	if (te->timerId() == m_CheckPosTimer)
	{
		//获取精确事件，并发出进度通知
		int64_t cur = clock();
		m_CurPos += cur - m_CheckClock;
		m_CheckClock = cur;
		emit PositionChanged(m_CurPos);
	}
}

void XAudioPlayer::OnStateChanged(int ps)
{
	if (ps == ps_Playing)
	{
		if (0 == m_CheckPosTimer)
		{
			m_CheckPosTimer = startTimer(m_TimerInterval);
			m_CheckClock = clock();
		}
	}
	else if (ps == ps_Stoped)
	{
		if (m_CheckPosTimer)
		{
			killTimer(m_CheckPosTimer);
			m_CheckPosTimer = 0;
		}
	}
}
