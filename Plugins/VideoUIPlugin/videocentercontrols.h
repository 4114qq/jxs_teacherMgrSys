#ifndef VIDEOCENTERCONTROLS_H
#define VIDEOCENTERCONTROLS_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

class VideoCenterControls : public QDialog
{
    Q_OBJECT

public:
    explicit VideoCenterControls(QWidget *parent = nullptr);
    ~VideoCenterControls();

    void setPlaying(bool isPlaying);

signals:
    void playPauseClicked();
    void seekBackwardClicked();
    void seekForwardClicked();

private slots:
    void onPlayPauseClicked();
    void onSeekBackwardClicked();
    void onSeekForwardClicked();

private:
    QPushButton *m_btnSeekBackward;
    QPushButton *m_btnPlayPause;
    QPushButton *m_btnSeekForward;
    bool m_isPlaying;
};

#endif // VIDEOCENTERCONTROLS_H
