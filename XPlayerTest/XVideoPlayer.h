#pragma once

#include <QWidget>

//��Ƶ���������������Ƶ�ߴ��Զ��������ڴ�С������ʵ�ʲ�����Ƶ�Ĵ����Ƕ������ڣ��ɱ��ഴ����
class XVideoPlayer : public QWidget
{
	Q_OBJECT
public:
	enum PlayerState
	{
		ps_Opened,			//�ļ��Ѵ�
		ps_Playing,			//��ʼ����
		ps_Paused,			//��ͣ����
		ps_Stoped			//ֹͣ����
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
	// ����������
	libvlc_media_player_t *m_pVlcPlayer = nullptr;
	QWidget* m_PlayWidget = nullptr;				//ʵ�ʲ�����Ƶ�Ĵ��ڣ�������ڵĳߴ�����Ƶ�Ŀ�߱ȶ��ı�
	int64_t m_Duration = 0;
	QString m_VideoUrl;
	//bool m_bStoped = false;
	bool m_bFullScreen = false;
	bool m_bEnableFullScreen = true;
	int m_PauseTimerId = 0;				//����ʱ������ʱ����˫��ʱ�رն�ʱ��
	PlayerState m_pState = ps_Stoped;
};
