#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QResizeEvent>
#include <QVBoxLayout>

#if defined(Q_OS_WIN)
#include <vlc/vlc.h>
#endif

#include "videocontrolbar.h"
#include "videocentercontrols.h"

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

    void play();
    void pause();
    void stop();

public slots:
    void openFile(const QString &filePath);

private slots:
    void onOpenFile();
    void onPlayPause();
    void onStop();
    void onVolumeChanged(int value);
    void onFullScreen();
    void toggleFullScreen();
    void onPositionChangedFromSlider(qint64 position);
    void onSeekBackward();
    void onSeekForward();
    void updatePosition();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void initUI();
    void initVLC();
    void updateCenterControlsPosition();
    void onHideTimerExpired();
    void showControls();
    void resetHideTimer();

private:
#if defined(Q_OS_WIN)
    libvlc_instance_t *m_vlcInstance;
    libvlc_media_player_t *m_vlcPlayer;
    libvlc_media_list_player_t *m_mediaListPlayer;
#endif

    QWidget *m_videoContainer;
    VideoControlBar *m_controlBar;
    VideoCenterControls *m_centerControls;
    QTimer *m_positionTimer;
    QTimer *m_hideTimer;
    bool m_isPlaying;
    bool m_controlsVisible;
};

#endif // VIDEOWIDGET_H
