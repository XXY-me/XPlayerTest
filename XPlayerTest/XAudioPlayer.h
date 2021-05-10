#pragma once

#include <QObject>

//音频播放器，使用vlc进行播放。与vlc视频播放器共用一个vlc实例。
class XAudioPlayer : public QObject
{
	Q_OBJECT
public:
	enum PlayerState
	{
		ps_Opened,			//文件已打开
		ps_Playing,			//开始播放
		ps_Paused,			//暂停播放
		ps_Stoped			//停止播放
	};
public:
	XAudioPlayer(QObject *parent);
	~XAudioPlayer();
	//打开文件，如果文件名为空则从新打开当前文件
	bool Open(QString file = "");
	bool Play();
	void Pause();
	void Stop();
	//vlc事件回调方法，
	void EventCallBack(const libvlc_event_t *event);
	PlayerState State();
	//调整播放位置
	void SetPlayPos(qint64 pos);
	//设置positionchanged的通知间隔，
	void SetNotifyInterval(int i);
private:
	void CreatePlayer();
protected:
	virtual void timerEvent(QTimerEvent *te);
protected slots:
	void OnStateChanged(int ps);
signals:
	void StateChanged(int ps);
	void DurationChanged(qint64 dur);
	void PositionChanged(qint64 pos);
private:
	libvlc_media_player_t *m_pVlcPlayer = nullptr;
	QString m_AudioFile;
	int64_t m_Duration = 0;
	//bool m_bStoped = false;
	PlayerState m_state = ps_Stoped;
	int m_CheckPosTimer = 0;
	int m_TimerInterval = 40;
	int64_t m_CurPos = 0;			//当前播放位置
	int64_t m_CheckClock = 0;
};
