#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>



#include <sstream>
#include <string>

#include <iostream>
#include "armadillo"
#include "GPMatrixFunctions.h"

using namespace arma;
using namespace std;

namespace Ui {
class DialogStatistics;
}

class DialogStatistics : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogStatistics(QWidget *parent = 0);
    ~DialogStatistics();

    mat fixAllM;
    mat autoFixAllM;

    void loadData(mat* p_fixAllM_in, mat* p_autoFixAllM_in, mat* p_experimentalSegmentsM);
    
private:
    Ui::DialogStatistics *ui;

    void computeStatistics();
};

#endif // STATISTICSDIALOG_H

