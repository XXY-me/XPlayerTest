#pragma once

#include <QWidget>

//视频播放器，会根据视频尺寸自动调整窗口大小，而且实际播放视频的窗口是独立窗口，由本类创建，
class XVideoPlayer : public QWidget
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
	XVideoPlayer(QWidget *parent);
	~XVideoPlayer();
	bool Open(QString url = "");
	bool Play();
	void Pause();
	void Stop();
	void ChangeVolume(int vol);
	//void DurationChanged4CB(int64_t dur);
	//void StateChanged4CB(PlayerState ps);
	//void TimeChanged4CB(qint64 time);
	void SetPosition(qint64 pos);
	bool IsFullScreen();
	void SetFullScreen(bool bfs = true);
	void EnableFullScreen(bool bEnable = true);
	PlayerState State();
	void EventCallBack(const libvlc_event_t * event);
	void AdjusetSize();
	QWidget* PlayerWidget() { return m_PlayWidget; }
signals:
	void StateChanged(int ps);
	void DurationChanged(qint64 dur);
	void PositionChanged(qint64 pos);
	void FullScreen(bool bFullScreen);
protected slots:
	void OnStateChanged(int ps);
protected:
	virtual void resizeEvent(QResizeEvent *re);
	virtual void hideEvent(QHideEvent *event);
	virtual bool eventFilter(QObject *watched, QEvent *event);
	virtual void timerEvent(QTimerEvent* te);
	virtual void showEvent(QShowEvent* se);
private:
	void CreatePlayer();
	void ReleasePlayer();
private:
	// 播放器对象
	libvlc_media_player_t *m_pVlcPlayer = nullptr;
	QWidget* m_PlayWidget = nullptr;				//实际播放视频的窗口，这个窗口的尺寸随视频的宽高比而改变
	int64_t m_Duration = 0;
	QString m_VideoUrl;
	//bool m_bStoped = false;
	bool m_bFullScreen = false;
	bool m_bEnableFullScreen = true;
	int m_PauseTimerId = 0;				//单击时启动定时器，双击时关闭定时器
	PlayerState m_pState = ps_Stoped;
};
