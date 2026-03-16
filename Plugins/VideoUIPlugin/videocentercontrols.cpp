#include "videocentercontrols.h"

VideoCenterControls::VideoCenterControls(QWidget *parent)
    : QDialog(parent)
    , m_btnSeekBackward(nullptr)
    , m_btnPlayPause(nullptr)
    , m_btnSeekForward(nullptr)
    , m_isPlaying(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    m_btnSeekBackward = new QPushButton(this);
    m_btnSeekBackward->setText(QString::fromLocal8Bit("◀◀"));
    m_btnSeekBackward->setFixedSize(60, 60);
    m_btnSeekBackward->setStyleSheet("QPushButton { background-color: rgba(80,80,80,220); color: white; border: none; font-size: 20px; border-radius: 30px; }");
    connect(m_btnSeekBackward, &QPushButton::clicked, this, &VideoCenterControls::onSeekBackwardClicked);

    m_btnPlayPause = new QPushButton(this);
    m_btnPlayPause->setText(QString::fromLocal8Bit("▶"));
    m_btnPlayPause->setFixedSize(80, 80);
    m_btnPlayPause->setStyleSheet("QPushButton { background-color: rgba(80,80,80,220); color: white; border: none; font-size: 40px; border-radius: 40px; }");
    connect(m_btnPlayPause, &QPushButton::clicked, this, &VideoCenterControls::onPlayPauseClicked);

    m_btnSeekForward = new QPushButton(this);
    m_btnSeekForward->setText(QString::fromLocal8Bit("▶▶"));
    m_btnSeekForward->setFixedSize(60, 60);
    m_btnSeekForward->setStyleSheet("QPushButton { background-color: rgba(80,80,80,220); color: white; border: none; font-size: 20px; border-radius: 30px; }");
    connect(m_btnSeekForward, &QPushButton::clicked, this, &VideoCenterControls::onSeekForwardClicked);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);
    layout->addStretch();
    layout->addWidget(m_btnSeekBackward);
    layout->addWidget(m_btnPlayPause);
    layout->addWidget(m_btnSeekForward);
    layout->addStretch();

    hide();
}

VideoCenterControls::~VideoCenterControls()
{
}

void VideoCenterControls::setPlaying(bool isPlaying)
{
    m_isPlaying = isPlaying;
    m_btnPlayPause->setText(isPlaying ? QString::fromLocal8Bit("⏸") : QString::fromLocal8Bit("▶"));
}

void VideoCenterControls::onPlayPauseClicked()
{
    emit playPauseClicked();
}

void VideoCenterControls::onSeekBackwardClicked()
{
    emit seekBackwardClicked();
}

void VideoCenterControls::onSeekForwardClicked()
{
    emit seekForwardClicked();
}
