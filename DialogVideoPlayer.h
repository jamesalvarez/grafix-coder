#ifndef DIALOGVIDEOPLAYER_H
#define DIALOGVIDEOPLAYER_H

#include <QtGui/QMovie>
#include <QtWidgets/QWidget>
#include <QDialog>
#include <qmediaplayer.h>
#include <QFileDialog>
#include "GrafixProject.h"
#include <QGraphicsView>
#include <QGraphicsVideoItem>
#include <QGLWidget>


namespace Ui {
class DialogVideoPlayer;
}

class DialogVideoPlayer : public QDialog
{
    Q_OBJECT



public:
    explicit DialogVideoPlayer(QWidget *parent = 0);
    ~DialogVideoPlayer();

    void loadData(GrafixParticipant* participant, mat &p_roughM_in, mat &p_smoothM_in, mat &p_fixAllM_in);
    void resizeEvent (QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent *mouseEvent);

private:

    typedef enum  {
        PlayModeSegment,
        PlayModeFragment,
        PlayModeWholeFile
    } PlayMode;

    GrafixParticipant* _participant;
    mat* p_roughM;
    mat* p_smoothM;
    mat* p_fixAllM;


    int secsFragment;
    int hz;
    int samplesPerFragment;
    int expWidth;
    int expHeight;
    double firstSampleMS;
    double playStartSampleMS;
    int clockStartTime;

    // Play settings
    PlayMode settingPlayMode;
    bool settingPlaySmooth;
    bool settingLoop;

    // State of playback
    bool playing;
    int playOnOff;
    double currentTimeMS;
    int currentIndex;
    int currentFragment;

    // Playing - Movie driven or by timer
    void stopPlaying();
    void startPlaying();
    int getMilliCount();


    // Update state of playback
    void updatePlaybackStateTime(double newTimeMS);
    void updatePlaybackState(int index, bool resetAll);
    // Painting
    void paintCurrentVisualizationFrame();
    void paintCurrentTimeLineLineFrame();
    void paintNoFrameFound();
    void paintTimeLine();

public slots:
    void openFile();
    void playButton();

private slots:
    void changeMovieMode();
    void settingChanged();
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void movieSliderReleased();
    void handleError();
    void fncCalculateAspectRatios();

private:
    int display_width;
    int display_height;

    int vid_offset_x;
    int vid_offset_y;

    int vid_width;
    int vid_height;

    QMediaPlayer *mediaPlayer;

    QGraphicsScene *visualizationScene;
    QGraphicsScene *timeLineScene;

    QGraphicsVideoItem *visualizationVideoItem;
    QGraphicsRectItem *screenLayerItem;
    QGraphicsPixmapItem *visualizationPixmapItem;
    QGraphicsLineItem *timeLineTimeItem;
    QGraphicsPixmapItem *timeLinePixmapItem;
    Ui::DialogVideoPlayer *ui;
};

#endif // DIALOGVIDEOPLAYER_H
