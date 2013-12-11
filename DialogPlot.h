#ifndef DIALOGPLOT_H
#define DIALOGPLOT_H

#include <QDialog>
#include "qcustomplot.h"
#include "armadillo"
#include "GrafixProject.h"
#include "GPMatrixFunctions.h"

namespace Ui {
class DialogPlot;
}

class DialogPlot : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPlot(QWidget *parent = 0);
    ~DialogPlot();
    void loadRMSData(GrafixParticipant *p);
    void loadDurData(GrafixParticipant *p);
    void loadVelData(GrafixParticipant *p);
    void loadSpikeData(GrafixParticipant *p);
    mat _fixAllM;
private:
    Ui::DialogPlot *ui;
    GrafixParticipant* p_participant;
};

#endif // DIALOGPLOT_H
