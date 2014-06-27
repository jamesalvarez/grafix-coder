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
private:

    GrafixParticipant* _participant;
    mat* p_roughM;
    mat* p_smoothM;
    mat* p_fixAllM;

    int typeData; // Smooth or Rough
    int secsFragment;
    int hz;
    int expWidth;
    int expHeight;

    int playOnOff;

    void playRough();

public slots:
    void openFile();
    void play();

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
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
    QGraphicsScene *scene;
    QGraphicsVideoItem *item;
    QGraphicsRectItem *rect_overlay;
    QGraphicsPixmapItem *pixmap_overlay;
    Ui::DialogVideoPlayer *ui;
};

#endif // DIALOGVIDEOPLAYER_H
