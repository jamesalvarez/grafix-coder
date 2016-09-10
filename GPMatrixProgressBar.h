#ifndef GPMATRIXPROGRESSBAR_H
#define GPMATRIXPROGRESSBAR_H

#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QProgressDialog>

class GPMatrixProgressBar
{

public:
    GPMatrixProgressBar(QWidget* parent);
    ~GPMatrixProgressBar();

    bool             _use_progress_dialog;
    bool             _batch_processing;
    QProgressDialog*  _progress_dialog;
    QWidget*         _parent_widget;
    int              _current_process_progress;
    int              _current_process_end;
    int              _batch_number_tasks;
    int              _batch_current_task;

    void beginBatch(int number_batch_files);
    void beginProcessing(QString process_name, int process_length);
    void updateProgressDialog(int progress, QString label);
    void endProcessing();
    void endBatch();
    bool wasCanceled();
};
#endif // GPMATRIXPROGRESSBAR_H
