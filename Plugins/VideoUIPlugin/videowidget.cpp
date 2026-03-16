#include "videowidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

#if defined(Q_OS_WIN)
#include <vlc/vlc.h>
#endif

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
    , m_vlcInstance(nullptr)
    , m_vlcPlayer(nullptr)
    , m_videoContainer(nullptr)
    , m_controlBar(nullptr)
    , m_centerControls(nullptr)
    , m_positionTimer(nullptr)
    , m_isPlaying(false)
{
    initUI();
#if defined(Q_OS_WIN)
    initVLC();
#endif
}

VideoWidget::~VideoWidget()
{
#if defined(Q_OS_WIN)
    if (m_vlcPlayer) {
        libvlc_media_player_stop(m_vlcPlayer);
        libvlc_media_player_release(m_vlcPlayer);
    }
    if (m_vlcInstance) {
        libvlc_release(m_vlcInstance);
    }
#endif
}

void VideoWidget::initUI()
{
    m_videoContainer = new QWidget(this);
    m_videoContainer->setStyleSheet("background-color: black;");

    m_controlBar = new VideoControlBar(this);

    m_centerControls = new VideoCenterControls(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_videoContainer, 1);
    mainLayout->addWidget(m_controlBar);

    connect(m_controlBar, &VideoControlBar::openClicked, this, &VideoWidget::onOpenFile);
    connect(m_controlBar, &VideoControlBar::playPauseClicked, this, &VideoWidget::onPlayPause);
    connect(m_controlBar, &VideoControlBar::stopClicked, this, &VideoWidget::onStop);
    connect(m_controlBar, &VideoControlBar::volumeChanged, this, &VideoWidget::onVolumeChanged);
    connect(m_controlBar, &VideoControlBar::fullScreenClicked, this, &VideoWidget::onFullScreen);
    connect(m_controlBar, &VideoControlBar::positionChanged, this, &VideoWidget::onPositionChangedFromSlider);

    connect(m_centerControls, &VideoCenterControls::playPauseClicked, this, &VideoWidget::onPlayPause);
    connect(m_centerControls, &VideoCenterControls::seekBackwardClicked, this, &VideoWidget::onSeekBackward);
    connect(m_centerControls, &VideoCenterControls::seekForwardClicked, this, &VideoWidget::onSeekForward);

    m_positionTimer = new QTimer(this);
    connect(m_positionTimer, &QTimer::timeout, this, &VideoWidget::updatePosition);
}

void VideoWidget::initVLC()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString pluginPath = appPath + "/plugins";

    const char *vlcArgv[] = {
        "--no-video-title-show",
        "--no-snapshot-preview",
        "--no-sub-autodetect-file",
        qPrintable(QString("--plugin-path=%1").arg(pluginPath))
    };
    int vlcArgc = sizeof(vlcArgv) / sizeof(vlcArgv[0]);

    m_vlcInstance = libvlc_new(vlcArgc, vlcArgv);
    if (!m_vlcInstance) {
        qWarning() << "Failed to create VLC instance, plugin path:" << pluginPath;
        return;
    }

    m_vlcPlayer = libvlc_media_player_new(m_vlcInstance);
    if (!m_vlcPlayer) {
        qWarning() << "Failed to create VLC media player";
        return;
    }

    libvlc_media_player_set_hwnd(m_vlcPlayer, (void *)m_videoContainer->winId());

    qDebug() << "VLC initialized successfully, plugin path:" << pluginPath;
}

void VideoWidget::play()
{
#if defined(Q_OS_WIN)
    if (!m_vlcPlayer) {
        return;
    }
    libvlc_media_player_play(m_vlcPlayer);
    m_isPlaying = true;
    m_controlBar->setPlaying(true);
    m_centerControls->setPlaying(true);
    m_centerControls->show();
    m_centerControls->raise();
    updateCenterControlsPosition();
    m_positionTimer->start(500);
#endif
}

void VideoWidget::pause()
{
#if defined(Q_OS_WIN)
    if (!m_vlcPlayer) {
        return;
    }
    libvlc_media_player_pause(m_vlcPlayer);
    m_isPlaying = false;
    m_controlBar->setPlaying(false);
    m_centerControls->setPlaying(false);
#endif
}

void VideoWidget::onStop()
{
#if defined(Q_OS_WIN)
    if (!m_vlcPlayer) {
        return;
    }

    m_isPlaying = false;
    m_controlBar->setPlaying(false);
    m_centerControls->setPlaying(false);
    m_centerControls->hide();
    m_controlBar->setPosition(0);

    QTimer::singleShot(0, this, [this]() {
        libvlc_media_player_stop(m_vlcPlayer);
    });
#endif
}

void VideoWidget::stop()
{
#if defined(Q_OS_WIN)
    if (!m_vlcPlayer) {
        return;
    }
    libvlc_media_player_stop(m_vlcPlayer);
#endif
}

void VideoWidget::openFile(const QString &filePath)
{
#if defined(Q_OS_WIN)
    if (!m_vlcInstance || !m_vlcPlayer) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("VLC未初始化"));
        return;
    }

    stop();

    qDebug() << "Opening file:" << filePath;

    QString normalizedPath = filePath;
    normalizedPath.replace("\\", "/");
    QString fileUrl = "file:///" + normalizedPath;
    libvlc_media_t *media = libvlc_media_new_location(m_vlcInstance, qUtf8Printable(fileUrl));
    if (!media) {
        qWarning() << "Failed to create media:" << filePath;
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("无法打开文件: %1").arg(filePath));
        return;
    }

    libvlc_media_player_set_media(m_vlcPlayer, media);
    libvlc_media_release(media);

    play();
#endif
}

void VideoWidget::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        QString::fromLocal8Bit("打开视频文件"),
        QString(),
        QString::fromLocal8Bit("视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv);;所有文件 (*.*)")
    );

    if (!fileName.isEmpty()) {
        openFile(fileName);
    }
}

void VideoWidget::onPlayPause()
{
    if (m_isPlaying) {
        pause();
    } else {
        play();
    }
}

// void VideoWidget::onStop()
// {
//     if (!m_vlcPlayer) {
//         return;
//     }

//     m_isPlaying = false;
//     m_controlBar->setPlaying(false);
//     m_centerControls->setPlaying(false);
//     m_centerControls->hide();
//     m_controlBar->setPosition(0);

//     QTimer::singleShot(0, this, [this]() {
//         libvlc_media_player_stop(m_vlcPlayer);
//     });
// }

void VideoWidget::onVolumeChanged(int value)
{
#if defined(Q_OS_WIN)
    if (m_vlcPlayer) {
        libvlc_audio_set_volume(m_vlcPlayer, value);
    }
#endif
}

void VideoWidget::onFullScreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void VideoWidget::onPositionChangedFromSlider(qint64 position)
{
#if defined(Q_OS_WIN)
    if (m_vlcPlayer) {
        libvlc_media_player_set_time(m_vlcPlayer, position);
    }
#endif
}

void VideoWidget::onSeekBackward()
{
#if defined(Q_OS_WIN)
    if (m_vlcPlayer) {
        libvlc_time_t currentTime = libvlc_media_player_get_time(m_vlcPlayer);
        libvlc_media_player_set_time(m_vlcPlayer, qMax(0LL, currentTime - 5000));
    }
#endif
}

void VideoWidget::onSeekForward()
{
#if defined(Q_OS_WIN)
    if (m_vlcPlayer) {
        libvlc_time_t currentTime = libvlc_media_player_get_time(m_vlcPlayer);
        libvlc_media_player_set_time(m_vlcPlayer, currentTime + 5000);
    }
#endif
}

void VideoWidget::updatePosition()
{
#if defined(Q_OS_WIN)
    if (!m_vlcPlayer) {
        return;
    }

    if (m_centerControls && m_centerControls->isVisible()) {
        m_centerControls->raise();
    }

    libvlc_time_t duration = libvlc_media_player_get_length(m_vlcPlayer);
    libvlc_time_t currentTime = libvlc_media_player_get_time(m_vlcPlayer);

    m_controlBar->setDuration(duration);
    m_controlBar->setPosition(currentTime);
#endif
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateCenterControlsPosition();
}

void VideoWidget::updateCenterControlsPosition()
{
    if (m_videoContainer && m_centerControls) {
        QPoint globalPos = m_videoContainer->mapToGlobal(QPoint(0, 0));
        int cw = 200;
        int ch = 100;
        int cx = globalPos.x() + (m_videoContainer->width() - cw) / 2;
        int cy = globalPos.y() + (m_videoContainer->height() - ch) / 2;
        m_centerControls->move(cx, cy);
        m_centerControls->resize(cw, ch);
    }
}
