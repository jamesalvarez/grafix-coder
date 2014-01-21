#ifndef DialogVisualization_H
#define DialogVisualization_H

#include "MyConstants.h"
#include "GrafixProject.h"

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <qpainter.h>

#include <sstream>
#include <string>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>

#include "armadillo"

using namespace arma;
using namespace std;

namespace Ui {
class DialogVisualization;
}

class DialogVisualization : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogVisualization(QWidget *parent = 0);
    ~DialogVisualization();

    void loadData(int currentFragment, GrafixParticipant* participant, mat &p_roughM_in, mat &p_smoothM_in, mat &p_fixAllM_in);
    
private:

    Ui::DialogVisualization *ui;

    int currentFragment;
    GrafixParticipant* _participant;
    mat* p_roughM;
    mat* p_smoothM;
    mat* p_fixAllM;
    int typeData; // Smooth or Rough
    int secsFragment;
    int hz;
    int expWidth;
    int expHeight;
    int currentFixIndexStart;

    int playOnOff;

    void paintXYAxes();
    void paintRoughXY();
    void paintSmoothXY();
    void playSmooth();
    void playRough();
    void playRoughAndSmooth();
    void paintBlankPanel();
    void paintFixations();
    void paintRoughAndSmooth();

    int getMilliCount();
    int getMilliSpan(int nTimeStart);


public slots:
    // Buttons
    void fncPress_bPlay();
    void fncPress_bCancel();
    void fncChange_Visualization(bool type);
    void closeEvent(QCloseEvent *);

};

#endif // DialogVisualization_H

