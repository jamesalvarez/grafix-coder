#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "MyConstants.h"
#include "GPFixationOperations.h"
#include "GrafixProject.h"

#include "DialogParticipantEditNotes.h"
#include "DialogGrafixError.h"
#include "DialogTestSmoothParameters.h"
#include "DialogConfig.h"
#include "DialogVisualization.h"
#include "DialogVisualizationSegments.h"
#include "DialogSmoothXY.h"
#include "DialogStatistics.h"
#include "DialogHeatMap.h"
#include "DialogExport.h"
#include "DialogOpenProject.h"
#include "DialogInterpolation.h"
#include "DialogEstimateFixations.h"

#include <QProgressDialog>
#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QMutex>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QLayout>
#include <qregion.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtimer.h>

#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <iostream>

#include "armadillo"

using namespace arma;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    //actual matrices
    mat roughM;   // [time ms, 0 , left x , left y, right x, right y]
    mat smoothM;  // [time ms, interpolation(0 or 1), x, y]
    mat fixAllM;
    mat experimentalSegmentsM;
    mat autoFixAllM; // Automatic detection of fixation durations

    //pointers to above matrices
    mat* p_roughM;
    mat* p_smoothM;
    mat* p_fixAllM;
    mat* p_experimentalSegmentsM;
    mat* p_autoFixAllM;

    uword _hz;
    uword _secsFragment;
    uword _maxPupilDilation;
    uword _displayStartIndex;
    uword _displayStopIndex;
    double _displayIncrement;
    int _previousFragment;
    int _currentFragment;
    int _nFragments;
    double _degPerPixel;
    int _expWidth;
    int _expHeight;
    Ui::MainWindow *ui;
    QMutex _files_mutex;
    QProgressDialog _files_progress;
    QFuture<void> _files_future;
    bool _files_stop_flag;
    int _files_next_position;
    int _currentAction; // Create/ delete/ merge
    int _fixInAction;  // If there is a fixation creation/ merge/ delete happenning it's on.
    int _fixStartPos;
    GrafixProject _project;
    GrafixParticipant* p_active_participant;
    GrafixConfiguration _configuration;
    int _index_active_participant;
    QSize _maximised_size; //stores window size
    QLabel* p_over_display_label;
    int _previous_mouse_posX;
    QRgb _previous_colour;

    //changing participant / display / editing
    void fncLoadSettings(GrafixConfiguration configuration);
    void fncInitializeProject();
    void fncSetActiveParticipant(int position);
    void fncSetActiveParticipantThread(int position);
    void fncWaitForLoad();
    void fncResetDisplayParams();//Resets the display parameters i.e. when switching participant
    void fncSetDisplayParams();//Sets the display parameters i.e. when switching fragment
    void fncManipulateFix(int from, int to);

    //saving loading files
    bool fncReadAllFiles(GrafixParticipant* participant);
    void fncSaveAllFiles(GrafixParticipant *participant);

    //painting
    void paintAll();
    void paintRoughData();
    void paintSmoothData();
    void paintFixations();
    void paintAutomaticFixations();
    void paintVelocity();
    void paintPupilDilation();
    void paintCurrentFixation(int from, int to);
    void paintCurrentFixationOnList(int from, int to);
    void paintExperimentalSegments();
    void paintLabels();
    void paintClear();

protected:
    void  mousePressEvent( QMouseEvent * );
    void  mouseMoveEvent( QMouseEvent * );
    void  mouseReleaseEvent( QMouseEvent * );
    void  mouseDoubleClickEvent(QMouseEvent *e);
    void  resizeEvent(QResizeEvent* event);

public slots:
    void fncChange_sInterpolation();
    void fncChange_sDisplacement();
    void fncChange_sDisplacInterpolate();
    void fncChange_sMinFixation() ;
    void fncChange_sVelocity() ;
    void fncChange_sVelocityVariance() ;
    void fncChange_tDegreePerPixel(QString string);
    void fncChange_tParticipantNumber();
    //void fncPress_max();
    //void fncPress_min();
    //void fncPress_bNext();
    //void fncPress_bPrevious();
    void fncPress_cbDisplacement();
    void fncPress_cbMinFixation() ;
    void fncPress_cbVelocityVariance();

    void fncPress_subMenuDialogConfig();
    void fncPress_subMenuProjectOpen();
    void fncPress_subMenuVisualizeCurrentFragment();
    void fncPress_subMenuVisualizeSegments();
    void fncPress_subMenuExport();
    void fncPress_subMenuCalculateSmooth();
    void fncPress_subMenuRecalculateFixations();
    void fncPress_subMenuShowStatistics();
    void fncPress_subMenuHeatMap();
    void fncPress_subMenuShowErrorLog();
    void fncPress_subMenuDetectFixations();
    void fncPress_subMenuManualEditing();
    void fncPress_subMenuAdjustParameters();

    void fncPress_bNext();
    void fncPress_bPrevious();
    void fncPress_bExecuteManual();
    void fncPress_bParticipantLeft();
    void fncPress_bParticipantRight();
    void fncPress_bCreate();
    void fncPress_bDelete();
    void fncPress_bMerge();
    void fncPress_bExecuteManual(int from, int to);
    void fncPress_bSmoothPursuit();
    void fncPress_bNotes();
    void fncPress_bAcceptEstimation();
    void fncPress_bInterpolate();
    void fncPress_bSmooth();
    void fncPress_bEstimateFixations();

};

#endif // MAINWINDOW_H

