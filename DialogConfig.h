#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtWidgets/QDialog>
#include "GrafixProject.h"
#include "MyConstants.h"
#include "QSettings"
#include <sstream>
#include <string>
#include <stdlib.h>
#include <iostream>

namespace Ui {
class DialogConfig;
}

class DialogConfig : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogConfig(QWidget *parent = 0);
    void loadData(GrafixProject* project);
    ~DialogConfig();

public slots:

    void fncChange_secsSegment(int index);
    void fncChange_HZ(int index);
    void fncPress_bAccept();
    
private:
    Ui::DialogConfig *ui;
    GrafixProject* _project;
    GrafixConfiguration _configuration;

    int     _hz;
    int     _expWidth;
    int     _expHeight;
    int     _secsFragment;
    double  _degreePerPixel;
};

#endif // CONFIGDIALOG_H

