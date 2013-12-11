#ifndef VISUALIZATIONSEGMENTSDIALOG_H
#define VISUALIZATIONSEGMENTSDIALOG_H

#include "MyConstants.h"
#include "GrafixProject.h"

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>
#include <qpainter.h>

#include <sstream>
#include <string>
#include <time.h>
#include <sys/timeb.h>
#include <iostream>

#include "armadillo"


using namespace arma;
using namespace std;

namespace Ui {
class DialogVisualizationSegments;
}

class DialogVisualizationSegments : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogVisualizationSegments(QWidget *parent = 0);
    ~DialogVisualizationSegments();

    void loadData(GrafixParticipant *participant,int secsFragment, int hz,int expWidth, int expHeight,  mat roughM, mat smoothM, mat fixAllM, mat segmentsM);
    
private:
    Ui::DialogVisualizationSegments *ui;
    GrafixParticipant *_participant;

    int currentSegment;
    int currentFragment;
    int nFragments;
    mat roughM;
    mat smoothM;
    mat fixAllM;
    mat segmentsM;
    int typeData; // Smooth or Rough
    int secsFragment;
    int hz;
    int expWidth;
    int expHeight;
    int currentFixIndexStart;

    std::vector< pair<int,string> > pathsImages;

    int playOnOff;

    void paintXYAxes();
    void paintRoughXY();
    void paintSmoothXY();
    void playSmooth();
    void playRough();
    void paintBlankPanel();
    void paintFixations();
    void paintBackgroundImage();

    int getMilliCount();
    int getMilliSpan(int nTimeStart);

    void fncUpdateFragment();
    void fncUpdateSegment();
    void saveCurrentImage(int id, QString pathImg );
    void loadImages();

public slots:
    // Buttons
    void fncPress_bPlay();
    void fncPress_bCancel();
    void fncChange_Visualization(bool type);
    void closeEvent(QCloseEvent *);

    void fncPress_bNextFragment();
    void fncPress_bPrevFragment();
    void fncPress_bNextSegment();
    void fncPress_bPrevSegment() ;
    void fncPress_bInputImage();

};

#endif // VISUALIZATIONSEGMENTSDIALOG_H
