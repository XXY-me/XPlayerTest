#pragma once

#include <QObject>

//��Ƶ��������ʹ��vlc���в��š���vlc��Ƶ����������һ��vlcʵ����
class XAudioPlayer : public QObject
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
	XAudioPlayer(QObject *parent);
	~XAudioPlayer();
	//���ļ�������ļ���Ϊ������´򿪵�ǰ�ļ�
	bool Open(QString file = "");
	bool Play();
	void Pause();
	void Stop();
	//vlc�¼��ص�������
	void EventCallBack(const libvlc_event_t *event);
	PlayerState State();
	//��������λ��
	void SetPlayPos(qint64 pos);
	//����positionchanged��֪ͨ�����
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
	int64_t m_CurPos = 0;			//��ǰ����λ��
	int64_t m_CheckClock = 0;
};
