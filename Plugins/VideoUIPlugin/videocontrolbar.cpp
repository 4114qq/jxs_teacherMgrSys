#include "videocontrolbar.h"
#include <QTime>

VideoControlBar::VideoControlBar(QWidget *parent)
    : QWidget(parent)
    , m_btnOpen(nullptr)
    , m_btnPlayPause(nullptr)
    , m_btnStop(nullptr)
    , m_positionSlider(nullptr)
    , m_lblTime(nullptr)
    , m_volumeSlider(nullptr)
    , m_btnFullScreen(nullptr)
    , m_isPlaying(false)
    , m_isDragging(false)
    , m_duration(0)
{
    setFixedHeight(40);

    m_btnOpen = new QPushButton(this);
    m_btnOpen->setText(QString::fromLocal8Bit("打开"));
    m_btnOpen->setFixedSize(50, 30);
    connect(m_btnOpen, &QPushButton::clicked, this, &VideoControlBar::onOpenClicked);

    m_btnPlayPause = new QPushButton(this);
    m_btnPlayPause->setText(QString::fromLocal8Bit("播放"));
    m_btnPlayPause->setFixedSize(50, 30);
    connect(m_btnPlayPause, &QPushButton::clicked, this, &VideoControlBar::onPlayPauseClicked);

    m_btnStop = new QPushButton(this);
    m_btnStop->setText(QString::fromLocal8Bit("停止"));
    m_btnStop->setFixedSize(50, 30);
    connect(m_btnStop, &QPushButton::clicked, this, &VideoControlBar::onStopClicked);

    m_positionSlider = new QSlider(Qt::Horizontal, this);
    m_positionSlider->setRange(0, 1000);
    m_positionSlider->setEnabled(false);
    connect(m_positionSlider, &QSlider::sliderPressed, this, &VideoControlBar::onPositionSliderPressed);
    connect(m_positionSlider, &QSlider::sliderReleased, this, &VideoControlBar::onPositionSliderReleased);
    connect(m_positionSlider, &QSlider::valueChanged, this, &VideoControlBar::onPositionSliderValueChanged);

    m_lblTime = new QLabel("00:00 / 00:00", this);
    m_lblTime->setFixedWidth(100);

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setFixedWidth(80);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(50);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &VideoControlBar::onVolumeSliderChanged);

    m_btnFullScreen = new QPushButton(this);
    m_btnFullScreen->setText(QString::fromLocal8Bit("全屏"));
    m_btnFullScreen->setFixedSize(50, 30);
    connect(m_btnFullScreen, &QPushButton::clicked, this, &VideoControlBar::onFullScreenClicked);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->addWidget(m_btnOpen);
    layout->addWidget(m_btnPlayPause);
    layout->addWidget(m_btnStop);
    layout->addWidget(m_positionSlider, 1);
    layout->addWidget(m_lblTime);
    layout->addWidget(m_volumeSlider);
    layout->addWidget(m_btnFullScreen);
}

VideoControlBar::~VideoControlBar()
{
}

void VideoControlBar::setDuration(qint64 duration)
{
    m_duration = duration;
    updateTimeLabel();
}

void VideoControlBar::setPosition(qint64 position)
{
    if (!m_isDragging && m_duration > 0) {
        int sliderValue = (int)(position * 1000 / m_duration);
        m_positionSlider->setValue(sliderValue);
    }
    updateTimeLabel();
}

void VideoControlBar::setPlaying(bool isPlaying)
{
    m_isPlaying = isPlaying;
    m_btnPlayPause->setText(isPlaying ? QString::fromLocal8Bit("暂停") : QString::fromLocal8Bit("播放"));
    m_positionSlider->setEnabled(true);
}

void VideoControlBar::onPlayPauseClicked()
{
    emit playPauseClicked();
}

void VideoControlBar::onOpenClicked()
{
    emit openClicked();
}

void VideoControlBar::onStopClicked()
{
    emit stopClicked();
}

void VideoControlBar::onPositionSliderPressed()
{
    m_isDragging = true;
    emit positionSliderPressed();
}

void VideoControlBar::onPositionSliderReleased()
{
    m_isDragging = false;
    if (m_duration > 0) {
        qint64 position = (m_duration * m_positionSlider->value()) / 1000;
        emit positionChanged(position);
    }
    emit positionSliderReleased();
}

void VideoControlBar::onPositionSliderValueChanged(int value)
{
    if (m_isDragging && m_duration > 0) {
        qint64 position = (m_duration * value) / 1000;
        emit positionChanged(position);
    }
    updateTimeLabel();
}

void VideoControlBar::onVolumeSliderChanged(int value)
{
    emit volumeChanged(value);
}

void VideoControlBar::onFullScreenClicked()
{
    emit fullScreenClicked();
}

void VideoControlBar::updateTimeLabel()
{
    qint64 position = 0;
    if (m_duration > 0) {
        position = (m_duration * m_positionSlider->value()) / 1000;
    }
    QString posStr = QTime(0, 0).addMSecs(position).toString("mm:ss");
    QString durStr = QTime(0, 0).addMSecs(m_duration).toString("mm:ss");
    m_lblTime->setText(posStr + " / " + durStr);
}
