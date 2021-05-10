#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_XPlayerTest.h"
#include "XAudioPlayer.h"
#include "XVideoPlayer2.h"
#include <QMediaPlayer>

class XPlayerTest : public QMainWindow
{
    Q_OBJECT

public:
    XPlayerTest(QWidget *parent = Q_NULLPTR);

protected slots:
	void on_btnOpenFile1_clicked();
	void on_btnPlay1_clicked();
	void on_btnStop1_clicked();
	void on_horizontalSlider1_valueChanged(int value);

	void on_AudioPlayer_StateChanged(int ps);
	void on_AudioPlayer_DurationChanged(qint64 dur);
	void on_AudioPlayer_PositionChanged(qint64 pos);

	/*********************************************/
	void on_btnOpenFile2_clicked();
	void on_btnPlay2_clicked();
	void on_btnStop2_clicked();
	void on_horizontalSlider2_valueChanged(int value);

	void on_widgetV1_StateChanged(int ps);
	void on_widgetV1_DurationChanged(qint64 dur);
	void on_widgetV1_PositionChanged(qint64 pos);

	/*********************************************/
	void on_btnOpenFile3_clicked();
	void on_btnPlay3_clicked();
	void on_btnStop3_clicked();
	void on_horizontalSlider3_valueChanged(int value);

	void on_VideoPlayer2_StateChanged(int ps);
	void on_VideoPlayer2_DurationChanged(qint64 dur);
	void on_VideoPlayer2_PositionChanged(qint64 pos);

	/*********************************************/
	void on_btnOpenFile4_clicked();
	void on_btnPlay4_clicked();
	void on_btnStop4_clicked();
	void on_horizontalSlider4_valueChanged(int value);

	void on_QtPlayer_stateChanged(QMediaPlayer::State state);
	void on_QtPlayer_durationChanged(qint64 duration);
	void on_QtPlayer_positionChanged(qint64 position);

	/*********************************************/
	void on_btnOpenFile5_clicked();
	void on_btnPlay5_clicked();
	void on_btnStop5_clicked();
	void on_horizontalSlider5_valueChanged(int value);

	void on_QtPlayer2_stateChanged(QMediaPlayer::State state);
	void on_QtPlayer2_durationChanged(qint64 duration);
	void on_QtPlayer2_positionChanged(qint64 position);

protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
private:
    Ui::XPlayerTestClass ui;
	XAudioPlayer* m_AudioPlayerVlc;
	XVideoPlayer2* m_VideoPlayer2;
	QMediaPlayer m_QtPlayer;
	QMediaPlayer m_QtPlayer2;
};
