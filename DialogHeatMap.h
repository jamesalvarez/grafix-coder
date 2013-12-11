#ifndef HEATMAPDIALOG_H
#define HEATMAPDIALOG_H

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>

#include <sstream>
#include <string>

#include <iostream>
#include "armadillo"
#include "GrafixProject.h"

using namespace arma;
using namespace std;

namespace Ui {
class DialogHeatMap;
}

class DialogHeatMap : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogHeatMap(QWidget *parent = 0);
    ~DialogHeatMap();

    void loadData(GrafixParticipant* participant, mat &roughM, mat* p_fixAllM, mat* p_segmentsM);
    
private:
    Ui::DialogHeatMap *ui;

    GrafixParticipant* _participant;
    mat* p_fixAllM ;
    int currentSegment;
    mat SegmentsM ;
    std::vector< pair<int,string> > pathsImages;

    void fncUpdateSegment();
    void paintBlankPanel();
    void paintBackgroundImage();
    void saveCurrentImage(int id, QString pathImg );
    void loadImages();
    void paintHeatMap(mat* p_fixations);
    void deleteHeatMap();

    void saveImages();
    //void  resizeEvent(QResizeEvent* event);

public slots:
    // Buttons
    void fncPress_bCalculateHeatMap();
    void fncPress_bCancel();
    //void closeEvent(QCloseEvent *);

    void fncPress_bNextSegment();
    void fncPress_bPrevSegment() ;
    void fncPress_bInputImage();
};

#endif // HEATMAPDIALOG_H

