#pragma once

#include <QObject>
#include <QMutex>
#include <QPixmap>

# ifdef __cplusplus
extern "C" {
# endif
#include <vlc/vlc.h>
# ifdef __cplusplus
}
# endif

struct Context {
	libvlc_media_player_t* player;
	QMutex mutex;
	uchar *pixels = nullptr;
	int width = 0;
	int height = 0;
	QWidget* displayWidget = nullptr;
};

class XVideoPlayer2 : public QObject
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
	XVideoPlayer2(QObject *parent);
	~XVideoPlayer2();
	bool Open(QString url = "");
	bool Play();
	void Pause();
	void Stop();
	void ChangeVolume(int vol);
	void SetPosition(qint64 pos);
	PlayerState State();
	void EventCallBack(const libvlc_event_t * event);
	void SetDisplayWidget(QWidget* dw);
	void SetBgColor(QColor c);
	void SetEndImage(QString img);
signals:
	void StateChanged(int ps);
	void DurationChanged(qint64 dur);
	void PositionChanged(qint64 pos);
protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);

private:
	void CreatePlayer();
	void ReleasePlayer();
private:
	// 播放器对象
	libvlc_media_player_t *m_pVlcPlayer = nullptr;
	int64_t m_Duration = 0;
	QString m_VideoUrl;
	QColor m_BgColor = Qt::black;
	PlayerState m_pState = ps_Stoped;
	QPixmap m_Curpix;
	Context m_ctx;
	QImage* m_StopImage = nullptr;
};
