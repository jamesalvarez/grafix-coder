#include "GPMatrixProgressBar.h"


GPMatrixProgressBar::GPMatrixProgressBar(QWidget* parent)
{
    _progress_dialog = new QProgressDialog(parent);
    (*_progress_dialog).setMinimumDuration(0);

    _use_progress_dialog = true;
    _batch_processing = false;
}

GPMatrixProgressBar::~GPMatrixProgressBar()
{
    delete _progress_dialog;
}

void GPMatrixProgressBar::beginBatch(int number_tasks)
{
    //TODO check number of tasks is feasible?
    _batch_number_tasks = number_tasks;
    _batch_current_task = 0;
    _batch_processing = true;
    ((QWidget*)((*_progress_dialog).parent()))->setCursor(Qt::WaitCursor);

    if (_use_progress_dialog)
    {
       (*_progress_dialog).setWindowModality(Qt::WindowModal);
        (*_progress_dialog).setAutoClose(false);
        (*_progress_dialog).setAutoReset(false);
        (*_progress_dialog).setCancelButtonText(("Cancel"));
        (*_progress_dialog).setRange(0, number_tasks*100);
        (*_progress_dialog).setValue(0);
        (*_progress_dialog).setWindowTitle(QString("Processing task %1 of %2").arg(0).arg(number_tasks));
        (*_progress_dialog).setLabelText("");
        (*_progress_dialog).show();
    }
    qApp->processEvents();
}


void GPMatrixProgressBar::beginProcessing(QString process_name, int process_length)
{
    if (!_batch_processing)
    {
        ((QWidget*)((*_progress_dialog).parent()))->setCursor(Qt::WaitCursor);

        if (_use_progress_dialog)
        {
           (*_progress_dialog).setWindowModality(Qt::WindowModal);
            (*_progress_dialog).setAutoClose(false);
            (*_progress_dialog).setAutoReset(false);
            (*_progress_dialog).setCancelButtonText(("Cancel"));
            (*_progress_dialog).setRange(0, process_length);
            (*_progress_dialog).setValue(0);
            (*_progress_dialog).setWindowTitle("Processing Data");
            (*_progress_dialog).setLabelText(process_name);
            (*_progress_dialog).show();
        }
        qApp->processEvents();
    }
    else
    {
        //beginning new task in batch processing
        _batch_current_task++;
        _current_process_end = process_length;
        _current_process_progress = 0;
        (*_progress_dialog).setValue(100*_batch_current_task);
        (*_progress_dialog).setWindowTitle(QString("Processing participant %1 of %2").arg(_batch_current_task).arg(_batch_number_tasks));
        (*_progress_dialog).setLabelText(process_name);
        int height = (*_progress_dialog).height();
        (*_progress_dialog).resize(300,height);
        (*_progress_dialog).show();

        qApp->processEvents();
    }
}

void GPMatrixProgressBar::updateProgressDialog(int progress, QString label)
{
    if (!_batch_processing)
    {
        (*_progress_dialog).setValue(progress);
        (*_progress_dialog).setLabelText(label);
        (*_progress_dialog).show();
        qApp->processEvents();
    }
    else
    {
        _current_process_progress = progress;
        int batch = 100*(_batch_current_task-1);
        int current = 100*_current_process_progress / _current_process_end; //out of 100
        (*_progress_dialog).setValue(batch + current);
        (*_progress_dialog).setLabelText(label);
        (*_progress_dialog).show();
        qApp->processEvents();
    }
}



 void GPMatrixProgressBar::endProcessing()
{
     if(!_batch_processing)
     {
        ((QWidget*)((*_progress_dialog).parent()))->setCursor(Qt::ArrowCursor);
         (*_progress_dialog).setValue((*_progress_dialog).maximum());
     }
}

 void GPMatrixProgressBar::endBatch()
 {
     ((QWidget*)((*_progress_dialog).parent()))->setCursor(Qt::ArrowCursor);
     (*_progress_dialog).setValue((*_progress_dialog).maximum());
     _batch_processing = false;
 }

 bool GPMatrixProgressBar::wasCanceled()
 {
     return (*_progress_dialog).wasCanceled();
 }
