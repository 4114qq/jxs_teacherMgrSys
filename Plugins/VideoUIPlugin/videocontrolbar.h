#ifndef VIDEOCONTROLBAR_H
#define VIDEOCONTROLBAR_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

class VideoControlBar : public QWidget
{
    Q_OBJECT

public:
    explicit VideoControlBar(QWidget *parent = nullptr);
    ~VideoControlBar();

    void setDuration(qint64 duration);
    void setPosition(qint64 position);
    void setPlaying(bool isPlaying);

signals:
    void openClicked();
    void playPauseClicked();
    void stopClicked();
    void positionChanged(qint64 position);
    void positionSliderPressed();
    void positionSliderReleased();
    void volumeChanged(int volume);
    void fullScreenClicked();

private slots:
    void onOpenClicked();
    void onPlayPauseClicked();
    void onStopClicked();
    void onPositionSliderPressed();
    void onPositionSliderReleased();
    void onPositionSliderValueChanged(int value);
    void onVolumeSliderChanged(int value);
    void onFullScreenClicked();

private:
    void updateTimeLabel();

private:
    QPushButton *m_btnOpen;
    QPushButton *m_btnPlayPause;
    QPushButton *m_btnStop;
    QSlider *m_positionSlider;
    QLabel *m_lblTime;
    QSlider *m_volumeSlider;
    QPushButton *m_btnFullScreen;

    bool m_isPlaying;
    bool m_isDragging;
    qint64 m_duration;
};

#endif // VIDEOCONTROLBAR_H
