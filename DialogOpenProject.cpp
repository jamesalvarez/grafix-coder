
#include "DialogOpenProject.h"
#include "ui_DialogOpenProject.h"


DialogOpenProject::DialogOpenProject(GrafixProject * project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOpenProject)
{
    ui->setupUi(this);

    _project = project;

    //attach model to table view
    _model = new GPTabelModel(project,GPTabelModel::FC_ROUGHFILE,0);
    ui->tableViewParticipants->setModel(_model);
    ui->tableViewParticipants->setContextMenuPolicy(Qt::CustomContextMenu);

    // Events
    connect(ui->buttonAddParticipant,SIGNAL(clicked()), this, SLOT(fncAddParticipantClicked()));
    connect(ui->buttonNewProject,SIGNAL(clicked()),this,SLOT(fncNewProjectClicked()));
    connect(ui->buttonLoadProject,SIGNAL(clicked()),this,SLOT(fncLoadProjectClicked()));
    connect(ui->tableViewParticipants,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(fncItemDoubleClicked(QModelIndex)));
    //connect(ui->buttonOpenDirectory, SIGNAL(clicked()),this,SLOT(fncPress_buttonOpenDirectory()));

}

DialogOpenProject::~DialogOpenProject()
{

    delete _model;
    delete ui;
}

void DialogOpenProject::accept()
{
    _project->SaveSettings();
    QDialog::accept();
}

void DialogOpenProject::fncAddParticipantClicked()
{
    if  (_project->HasDirectory())
    {
        int n = _project->numParticipants();
        _model->insertRows(n, 1, QModelIndex());
        //check if successful;
        if (_project->numParticipants() == n + 1)
            _model->EditParticipant(n);
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Error creating new participant.");
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Please create or load a project first.");
        msgBox.exec();
    }
}

void DialogOpenProject::fncNewProjectClicked()
{
    //ask whether want to convert existing directory structure into project
    QMessageBox msgBox;
    msgBox.setText("Creating New Project");
    msgBox.setInformativeText("Do you want to scan an existing directory for participants and files?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setButtonText(QMessageBox::No,"No, create new project");
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::No:
    {
        QString newp = QFileDialog::getSaveFileName(this,"Create new project", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        if (QDir().mkdir(newp))
            _model->NewProject(newp);
        break;
    }
    case QMessageBox::Yes:
    {
        QString dir = QFileDialog::getExistingDirectory(this,"Choose existing directory", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        int ret = QMessageBox::Yes;
        if (QFile(dir + "/project.ini").exists())
        {
            QMessageBox msgBox2;
            msgBox2.setText("Creating New Project");
            msgBox.setInformativeText("Warning: This will erase the detected stored settings associated with this project (have you tried loading it?). Are you sure you wish to continue?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
            msgBox.setDefaultButton(QMessageBox::No);
            ret = msgBox.exec();
        }
        if (ret == QMessageBox::Yes)
        {

                _model->NewProjectFromExistingDirectory(dir);
            break;
        }
    }
    case QMessageBox::Cancel:
        break;
    }
}

void DialogOpenProject::fncLoadProjectClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,"Choose existing project directory", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (!dir.isEmpty()) _model->LoadData(dir);
}


void DialogOpenProject::fncItemDoubleClicked(QModelIndex index)
{
    int row = index.row();
    int col = index.column();
    // generate a log message when this method gets called
    qDebug() << QString("Double clicked: row %1, col%2, role %3")
         .arg(row).arg(col);

    _model->EditParticipant(row);
}


void DialogOpenProject::on_tableViewParticipants_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->tableViewParticipants->mapToGlobal(pos);

    QModelIndex qmi = ui->tableViewParticipants->indexAt(pos);
    QMenu myMenu;
    myMenu.addAction("Edit");
    myMenu.addAction("Remove");

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->text() == "Edit")
        {
            fncItemDoubleClicked(qmi);
        }
        else if (selectedItem->text() == "Remove")
        {
            QMessageBox msgBox;
            msgBox.setText("Removing Participant");
            msgBox.setInformativeText("Do you also want to delete the participant directory?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            msgBox.setButtonText(QMessageBox::Discard,"Delete");
            msgBox.setButtonText(QMessageBox::Save,"Keep");
            int ret = msgBox.exec();

            switch (ret)
            {
            case QMessageBox::Discard:
                //delete folder
                _project->GetParticipant(qmi.row())->DeleteDirectory();
            case QMessageBox::Save:
                int n = _project->numParticipants();
                _model->removeRow(qmi.row());
                //check if successful;
                if (_project->numParticipants() != n - 1)
                {
                    QMessageBox msgBox;
                    msgBox.setText("Error removing participant.");
                    msgBox.exec();
                }
                break;
            }
        }
    }
    else
    {
        // nothing was chosen
    }
}

void DialogOpenProject::fncPress_buttonOpenDirectory()
{
    QString filePath = _project->GetFullDirectory();
    QDesktopServices::openUrl(QUrl("file:///" + filePath, QUrl::TolerantMode));
}
