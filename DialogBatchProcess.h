#ifndef DialogBatchProcess_H
#define DialogBatchProcess_H

#include "GPTableModel.h"
#include "GrafixProject.h"
#include "GPMatrixFunctions.h"
#include "MyConstants.h"
#include "DialogGrafixError.h"
#include "armadillo"
#include <QDialog>

namespace Ui {
class DialogBatchProcess;
}

class DialogBatchProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBatchProcess(GrafixProject *project, Consts::BATCH_TASK task, QWidget *parent = 0);
    ~DialogBatchProcess();

    void changeTask(Consts::BATCH_TASK task);

public slots:

    void fncItemDoubleClicked(QModelIndex index);
    void fnc_bSelectAllClicked();

private:
    Ui::DialogBatchProcess*     ui;
    GPTabelModel*           _model;
    GrafixProject*          _project;
    Consts::BATCH_TASK _current_task;


    void accept();



};

#endif // DialogBatchProcess_H
