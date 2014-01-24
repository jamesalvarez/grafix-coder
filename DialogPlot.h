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
    void loadRMSData(const GrafixParticipant *p);
    void loadDurData(const GrafixParticipant *p);
    void loadVelData(const GrafixParticipant *p);
    void loadSpikeData(const GrafixParticipant *p);
    mat _fixAllM;
private:
    Ui::DialogPlot *ui;
    const GrafixParticipant* p_participant;
};

#endif // DIALOGPLOT_H
