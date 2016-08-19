#include "DialogBatchProcess.h"
#include "ui_DialogBatchProcess.h"

DialogBatchProcess::DialogBatchProcess(GrafixProject *project, Consts::BATCH_TASK task, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBatchProcess)
{
    ui->setupUi(this);

    _model = NULL;
    _project = project;
    _current_task = task;

    //attach model to table view
    changeTask(_current_task);


    connect(ui->tableBatch,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(fncItemDoubleClicked(QModelIndex)));
    connect(ui->bSelectAll, SIGNAL(clicked()), this, SLOT(fnc_bSelectAllClicked()));
}

DialogBatchProcess::~DialogBatchProcess()
{
    delete _model;
    delete ui;
}

void DialogBatchProcess::changeTask(Consts::BATCH_TASK task)
{
    _current_task = task;
    if (_model != NULL)
        delete _model;
    switch (_current_task)
    {
    case Consts::TASK_SMOOTH:
        _model = new GPTabelModel(_project,GPTabelModel::FC_SMOOTHFILE,0);
        break;
    case Consts::TASK_INTERPOLATE:
        _model = new GPTabelModel(_project, GPTabelModel::FC_INTERPOLATE, 0);
        break;
    case Consts::TASK_ESTIMATE_FIXATIONS:
        _model = new GPTabelModel(_project, GPTabelModel::FC_ESTIMATE_FIX, 0);
    case Consts::TASK_EXPORT:
        _model = new GPTabelModel(_project, GPTabelModel::FC_EXPORT, 0);
    }
    ui->tableBatch->setModel(_model);
}


void DialogBatchProcess::accept()
{

    QItemSelectionModel *select = ui->tableBatch->selectionModel();


    if (select->hasSelection())
    {
        QString dir; // Get user to choose a directory to output to

        if (_current_task == Consts::TASK_EXPORT)
            dir = QFileDialog::getExistingDirectory(this, "Choose directory to export to.",
                                                     QString(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);

        DialogGrafixError::LogNewError(NULL,"Beginning Batch Process...");

        GPMatrixProgressBar gpProgress(this);
        QModelIndexList selected = select->selectedRows();
        gpProgress.beginBatch(selected.count());
        for (int i = 0; i < selected.count(); i++)
        {
            QModelIndex s = selected[i];
            GrafixParticipant* p = _project->GetParticipant(s.row());

            try
            {
                switch (_current_task)
                {
                    case Consts::TASK_SMOOTH: {
                        mat roughM;
                        mat smoothM;
                        if (!GPMatrixFunctions::readFile(roughM,p->GetMatrixPath(Consts::MATRIX_ROUGH))) break;

                        GPMatrixFunctions::smoothRoughMatrixFBF(roughM,
                                                    this->_project->GetProjectSettingsPath(),
                                                    Consts::ACTIVE_CONFIGURATION(),
                                                    &smoothM,
                                                    &gpProgress);

                        if (GPMatrixFunctions::saveFile(smoothM,p->GetMatrixPath(Consts::MATRIX_SMOOTH)))
                            p->SetParticipantSetting(Consts::PSETTING_SMOOTHED_DATE, QDateTime::currentDateTime());
                    }

                    break;
                    case Consts::TASK_INTERPOLATE: {
                        mat smoothM;
                        if (!GPMatrixFunctions::readFile(smoothM, p->GetMatrixPath(Consts::MATRIX_SMOOTH))) break;

                        GPMatrixFunctions::interpolateData(smoothM,
                                                          _project->GetProjectSettingsPath(),
                                                          gpProgress);

                        if (GPMatrixFunctions::saveFile(smoothM,p->GetMatrixPath(Consts::MATRIX_SMOOTH)))
                            p->SetParticipantSetting(Consts::PSETTING_INTERPOLATED_DATE,QDateTime::currentDateTime());
                    }

                    break;
                    case Consts::TASK_ESTIMATE_FIXATIONS: {
                        mat SmoothM, AutoFixAllM, RoughM, SegmentsM;
                        bool loaded = GPMatrixFunctions::readFileSafe(SmoothM, p->GetMatrixPath(Consts::MATRIX_SMOOTH)) &&
                            GPMatrixFunctions::readFileSafe(RoughM, p->GetMatrixPath(Consts::MATRIX_ROUGH));

                        if (!loaded) break;

                        GrafixSettingsLoader settingsLoader((*_project));

                        GPMatrixFunctions::estimateFixations(RoughM,
                                                             SmoothM,
                                                             AutoFixAllM,
                                                             settingsLoader,
                                                             gpProgress);

                        if (GPMatrixFunctions::saveFileSafe(SmoothM, p->GetMatrixPath(Consts::MATRIX_SMOOTH)) &
                            GPMatrixFunctions::saveFileSafe(AutoFixAllM, p->GetMatrixPath(Consts::MATRIX_AUTOFIXALL)))
                        {
                            //if there is a segments files only save the segment fixations
                            if (GPMatrixFunctions::readFileSafe(SegmentsM, p->GetMatrixPath(Consts::MATRIX_SEGMENTS)))
                            {
                                GPMatrixFunctions::fncReturnFixationinSegments( &AutoFixAllM, &SegmentsM);
                            }
                            GPMatrixFunctions::saveFileSafe(AutoFixAllM, p->GetMatrixPath(Consts::MATRIX_FIXALL));
                            p->SetParticipantSetting(Consts::PSETTING_ESTIMATED_FIX_DATE, QDateTime::currentDateTime());
                        }
                    }

                    break;
                    case Consts::TASK_EXPORT: {


                        QString filename = dir + "/" + p->GetName() + ".csv";
                        GrafixSettingsLoader settingsLoader(*_project);

                        //get all matrixes
                        mat roughM, smoothM, fixAllM;

                        if (GPMatrixFunctions::readFileSafe(roughM, p->GetMatrixPath(Consts::MATRIX_ROUGH))) {
                            GPMatrixFunctions::readFileSafe(smoothM, p->GetMatrixPath(Consts::MATRIX_SMOOTH));
                            GPMatrixFunctions::readFileSafe(fixAllM, p->GetMatrixPath(Consts::MATRIX_FIXALL));
                            GPMatrixFunctions::exportFile(roughM, smoothM, fixAllM, filename, settingsLoader);
                        }

                    }

                    break;
                }
            }
            catch(std::exception &err)
            {
                DialogGrafixError::LogNewError(p,QString(err.what()));
            }
            catch(...)
            {
                DialogGrafixError::LogNewError(p,"Unknown Error");
            }

            DialogGrafixError::LogNewError(p, "Processing finished.");

        }

        gpProgress.endBatch();
        DialogGrafixError::LogNewError(NULL,"Batch Process ended.");

        //show error dialog
        DialogGrafixError::ShowDialog();
    }

    QDialog::accept();
}

void DialogBatchProcess::fncItemDoubleClicked(QModelIndex index)
{
    int row = index.row();
    int col = index.column();
    // generate a log message when this method gets called
    qDebug() << QString("Double clicked: row %1, col%2, role %3")
         .arg(row).arg(col);

    _model->EditParticipant(row);

}

void DialogBatchProcess::fnc_bSelectAllClicked()
{
    ui->tableBatch->selectAll();
}
