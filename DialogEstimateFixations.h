#ifndef ESTIMATEFIXATIONSDIALOG_H
#define ESTIMATEFIXATIONSDIALOG_H

#include <QDialog>
#include "GrafixProject.h"
#include "GPMatrixFunctions.h"
#include "DialogBatchProcess.h"

namespace Ui {
class DialogEstimateFixations;
}

class DialogEstimateFixations : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEstimateFixations(QWidget *parent = 0);
    ~DialogEstimateFixations();

    void loadData(GrafixParticipant &participant,mat &roughM, mat &smoothM, mat &autoFixAllM);


private:
    Ui::DialogEstimateFixations *ui;
    GrafixParticipant*  _participant;
    GrafixProject*      _project;
    mat*                p_roughM;
    mat*                p_smoothM;
    mat*                p_autoFixAllM;

public slots:
    void fncPress_bAccept();
    void fncPress_bClose();
    void fncPress_bBatch();
};

#endif // ESTIMATEFIXATIONSDIALOG_H
