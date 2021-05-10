#include "XPlayerTest.h"
#include <QFileDialog>
#include <QVideoWidget>

XPlayerTest::XPlayerTest(QWidget *parent)
    : QMainWindow(parent)
	, m_QtPlayer(this)
	, m_QtPlayer2(this)
{
    ui.setupUi(this);
	ui.horizontalSlider1->installEventFilter(this);
	m_AudioPlayerVlc = new XAudioPlayer(this);
	connect(m_AudioPlayerVlc, SIGNAL(StateChanged(int)), SLOT(on_AudioPlayer_StateChanged(int)));
	connect(m_AudioPlayerVlc, SIGNAL(DurationChanged(qint64)), SLOT(on_AudioPlayer_DurationChanged(qint64)));
	connect(m_AudioPlayerVlc, SIGNAL(PositionChanged(qint64)), SLOT(on_AudioPlayer_PositionChanged(qint64)));

	m_VideoPlayer2 = new XVideoPlayer2(this);
	connect(m_VideoPlayer2, SIGNAL(StateChanged(int)), SLOT(on_VideoPlayer2_StateChanged(int)));
	connect(m_VideoPlayer2, SIGNAL(DurationChanged(qint64)), SLOT(on_VideoPlayer2_DurationChanged(qint64)));
	connect(m_VideoPlayer2, SIGNAL(PositionChanged(qint64)), SLOT(on_VideoPlayer2_PositionChanged(qint64)));
	m_VideoPlayer2->SetDisplayWidget(ui.widgetV2);
	m_VideoPlayer2->SetBgColor(Qt::green);

	connect(&m_QtPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(on_QtPlayer_stateChanged(QMediaPlayer::State)));
	connect(&m_QtPlayer, SIGNAL(durationChanged(qint64)), SLOT(on_QtPlayer_durationChanged(qint64)));
	connect(&m_QtPlayer, SIGNAL(positionChanged(qint64)), SLOT(on_QtPlayer_positionChanged(qint64)));

	connect(&m_QtPlayer2, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(on_QtPlayer2_stateChanged(QMediaPlayer::State)));
	connect(&m_QtPlayer2, SIGNAL(durationChanged(qint64)), SLOT(on_QtPlayer2_durationChanged(qint64)));
	connect(&m_QtPlayer2, SIGNAL(positionChanged(qint64)), SLOT(on_QtPlayer2_positionChanged(qint64)));

	//创建widget
	QVideoWidget * vw = new QVideoWidget(ui.widgetVout);
	ui.widgetVout->layout()->addWidget(vw);
	m_QtPlayer2.setVideoOutput(vw);
}

//VLC音频相关

void XPlayerTest::on_btnOpenFile1_clicked()
{
	QString Path = QFileDialog::getOpenFileName(this, tr("open Audio File"), "", tr("Audio File (*.mp3);;All File(*.*)"));
	ui.lineEditPath1->setText(Path);
}

void XPlayerTest::on_btnPlay1_clicked()
{
	switch (m_AudioPlayerVlc->State())
	{
	case XAudioPlayer::ps_Opened:
		m_AudioPlayerVlc->Play();
		break;
	case XAudioPlayer::ps_Playing:
		m_AudioPlayerVlc->Pause();
		break;
	case XAudioPlayer::ps_Paused:
		m_AudioPlayerVlc->Play();
		break;
	case XAudioPlayer::ps_Stoped:
		m_AudioPlayerVlc->Open(ui.lineEditPath1->text());
		m_AudioPlayerVlc->Play();
		break;
	default:
		break;
	}
	
}

void XPlayerTest::on_btnStop1_clicked()
{
	m_AudioPlayerVlc->Stop();
}

void XPlayerTest::on_horizontalSlider1_valueChanged(int value)
{
	m_AudioPlayerVlc->SetPlayPos(value);
}

void XPlayerTest::on_AudioPlayer_StateChanged(int ps)
{
}

void XPlayerTest::on_AudioPlayer_DurationChanged(qint64 dur)
{
	ui.horizontalSlider1->setMaximum(dur);
}

void XPlayerTest::on_AudioPlayer_PositionChanged(qint64 pos)
{
	ui.horizontalSlider1->blockSignals(true);
	ui.horizontalSlider1->setValue(pos);
	ui.horizontalSlider1->blockSignals(false);
}

//VLC视频相关
void XPlayerTest::on_btnOpenFile2_clicked()
{
	QString Path = QFileDialog::getOpenFileName(this, tr("open Video File"), "", tr("Video File (*.mp4 *.avi *.rmvb);;All File(*.*)"));
	ui.lineEditPath2->setText(Path);
}

void XPlayerTest::on_btnPlay2_clicked()
{
	switch (ui.widgetV1->State())
	{
	case XAudioPlayer::ps_Opened:
		ui.widgetV1->Play();
		break;
	case XAudioPlayer::ps_Playing:
		ui.widgetV1->Pause();
		break;
	case XAudioPlayer::ps_Paused:
		ui.widgetV1->Play();
		break;
	case XAudioPlayer::ps_Stoped:
		ui.widgetV1->Open(ui.lineEditPath2->text());
		ui.widgetV1->Play();
		break;
	default:
		break;
	}
}

void XPlayerTest::on_btnStop2_clicked()
{
	ui.widgetV1->Stop();
}

void XPlayerTest::on_horizontalSlider2_valueChanged(int value)
{
	ui.widgetV1->SetPosition(value);
}

void XPlayerTest::on_widgetV1_StateChanged(int ps)
{
}

void XPlayerTest::on_widgetV1_DurationChanged(qint64 dur)
{
	ui.horizontalSlider2->setMaximum(dur);
}

void XPlayerTest::on_widgetV1_PositionChanged(qint64 pos)
{
	ui.horizontalSlider2->blockSignals(true);
	ui.horizontalSlider2->setValue(pos);
	ui.horizontalSlider2->blockSignals(false);
}

//VLC视频2相关
void XPlayerTest::on_btnOpenFile3_clicked()
{
	QString Path = QFileDialog::getOpenFileName(this, tr("open Video File"), "", tr("Video File (*.mp4 *.avi *.rmvb);;All File(*.*)"));
	ui.lineEditPath3->setText(Path);
}

void XPlayerTest::on_btnPlay3_clicked()
{
	switch (m_VideoPlayer2->State())
	{
	case XAudioPlayer::ps_Opened:
		m_VideoPlayer2->Play();
		break;
	case XAudioPlayer::ps_Playing:
		m_VideoPlayer2->Pause();
		break;
	case XAudioPlayer::ps_Paused:
		m_VideoPlayer2->Play();
		break;
	case XAudioPlayer::ps_Stoped:
		m_VideoPlayer2->Open(ui.lineEditPath3->text());
		m_VideoPlayer2->Play();
		break;
	default:
		break;
	}
}

void XPlayerTest::on_btnStop3_clicked()
{
	m_VideoPlayer2->Stop();
}

void XPlayerTest::on_horizontalSlider3_valueChanged(int value)
{
	m_VideoPlayer2->SetPosition(value);
}

void XPlayerTest::on_VideoPlayer2_StateChanged(int ps)
{

}

void XPlayerTest::on_VideoPlayer2_DurationChanged(qint64 dur)
{
	ui.horizontalSlider3->setMaximum(dur);
}

void XPlayerTest::on_VideoPlayer2_PositionChanged(qint64 pos)
{
	ui.horizontalSlider3->blockSignals(true);
	ui.horizontalSlider3->setValue(pos);
	ui.horizontalSlider3->blockSignals(false);
}

/******************************************************/
void XPlayerTest::on_btnOpenFile4_clicked()
{
	QString Path = QFileDialog::getOpenFileName(this, tr("open Audio File"), "", tr("Audio File (*.mp3);;All File(*.*)"));
	ui.lineEditPath4->setText(Path);
}

void XPlayerTest::on_btnPlay4_clicked()
{
	switch (m_QtPlayer.state())
	{
	case QMediaPlayer::PlayingState:
		m_QtPlayer.pause();
		break;
	case QMediaPlayer::PausedState:
		m_QtPlayer.play();
		break;
	case QMediaPlayer::StoppedState:
		m_QtPlayer.setMedia(QUrl::fromLocalFile(ui.lineEditPath4->text()));
		m_QtPlayer.play();
		break;
	default:
		break;
	}
}

void XPlayerTest::on_btnStop4_clicked()
{
	m_QtPlayer.stop();
}

void XPlayerTest::on_horizontalSlider4_valueChanged(int value)
{
	m_QtPlayer.setPosition(value);
}

void XPlayerTest::on_QtPlayer_stateChanged(QMediaPlayer::State state)
{
}

void XPlayerTest::on_QtPlayer_durationChanged(qint64 duration)
{
	ui.horizontalSlider4->setMaximum(duration);
}

void XPlayerTest::on_QtPlayer_positionChanged(qint64 position)
{
	ui.horizontalSlider4->blockSignals(true);
	ui.horizontalSlider4->setValue(position);
	ui.horizontalSlider4->blockSignals(false);
}

/******************************************************/
void XPlayerTest::on_btnOpenFile5_clicked()
{
	QString Path = QFileDialog::getOpenFileName(this, tr("open Video File"), "", tr("Video File (*.mp4 *.avi *.rmvb);;All File(*.*)"));
	ui.lineEditPath5->setText(Path);
}

void XPlayerTest::on_btnPlay5_clicked()
{
	switch (m_QtPlayer2.state())
	{
	case QMediaPlayer::PlayingState:
		m_QtPlayer2.pause();
		break;
	case QMediaPlayer::PausedState:
		m_QtPlayer2.play();
		break;
	case QMediaPlayer::StoppedState:
		m_QtPlayer2.setMedia(QUrl::fromLocalFile(ui.lineEditPath5->text()));
		m_QtPlayer2.play();
		break;
	default:
		break;
	}
}

void XPlayerTest::on_btnStop5_clicked()
{
	m_QtPlayer2.stop();
}

void XPlayerTest::on_horizontalSlider5_valueChanged(int value)
{
	m_QtPlayer2.setPosition(value);
}

void XPlayerTest::on_QtPlayer2_stateChanged(QMediaPlayer::State state)
{
}

void XPlayerTest::on_QtPlayer2_durationChanged(qint64 duration)
{
	ui.horizontalSlider5->setMaximum(duration);
}

void XPlayerTest::on_QtPlayer2_positionChanged(qint64 position)
{
	ui.horizontalSlider5->blockSignals(true);
	ui.horizontalSlider5->setValue(position);
	ui.horizontalSlider5->blockSignals(false);
}

bool XPlayerTest::eventFilter(QObject * watched, QEvent * event)
{
	if (watched == ui.horizontalSlider1)
	{
		if (event->type() == QEvent::MouseButtonPress)
		{

		}
	}
	return false;
}
