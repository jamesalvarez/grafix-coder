
#include "DialogParticipantPaths.h"
#include "ui_DialogParticipantPaths.h"


DialogParticipantPaths::DialogParticipantPaths(GrafixParticipant* participant, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogParticipantPaths)
{
    ui->setupUi(this);

    _participant = participant;

    connect( ui->buttonOpenRough, SIGNAL( clicked() ), this, SLOT( fncPress_buttonOpenRough() ) );
    connect( ui->buttonOpenSegments, SIGNAL( clicked() ), this, SLOT( fncPress_buttonOpenSegments() ) );
    connect(ui ->buttonNotes, SIGNAL(clicked()),this,SLOT(fncPress_buttonNotes()));
    connect(ui->buttonOpenDirectory, SIGNAL(clicked()),this,SLOT(fncPress_buttonOpenDirectory()));


    if (_participant->GetName() != "")
    {
        ui->tParticipantName->setText(_participant->GetName());
    }
    else
    {
        ui->tParticipantName->setText(_participant->GetRelativeDirectory());
    }

    ui->tRoughPath->setText(_participant->PATH_ROUGH);
    ui->tSegmentsPath->setText(_participant->EXPE_SEGMETS_PATH);
}

DialogParticipantPaths::~DialogParticipantPaths()
{
    //Set rough and segments paths
    delete ui;
}

void DialogParticipantPaths::accept()
{
    fncSavePaths();
    QDialog::accept();
}

void DialogParticipantPaths::fncPress_buttonOpenRough()
{
    QString fileName = _participant->OpenAndCopyCSVFile(Consts::MATRIX_ROUGH, this, "Open Rough CSV File", "*.csv");
    if (!fileName.isEmpty())
    {
        ui->tRoughPath->setText(_participant->GetRelativePath(fileName));
        ui->lInfo->setText("Rough file loaded.");
    }
    else
    {
        ui->lInfo->setText("File error: Check rough file has the right format. Columns=[miliseconds, 0, XRight, YRight, XLeft, YLeft].");
    }
}

void DialogParticipantPaths::fncPress_buttonOpenSegments()
{
    QString fileName = _participant->OpenAndCopyCSVFile(Consts::MATRIX_SEGMENTS, this, "Open Segments CSV File", "*.csv");
    if (!fileName.isEmpty())
    {
        ui->tSegmentsPath->setText(_participant->GetRelativePath(fileName));
        ui->lInfo->setText("Segments file loaded.");
    }
    else
    {
        ui->lInfo->setText("File error: Check segments file has the right format.");
    }
}

void DialogParticipantPaths::fncPress_buttonNotes()
{
    DialogParticipantEditNotes notes(_participant);
    notes.exec();
}


void DialogParticipantPaths::fncSavePaths()
{
    _participant->PATH_ROUGH = ui->tRoughPath->text();
    _participant->EXPE_SEGMETS_PATH = ui->tSegmentsPath->text();
    if (!_participant->Rename(ui->tParticipantName->text()))
    {
        //error renaming
        DialogGrafixError::AlertNewError("Error changing filenames, check files are not open");
    }
}

void DialogParticipantPaths::fncPress_buttonOpenDirectory()
{
    QString filePath = _participant->GetFullDirectory();
    QDesktopServices::openUrl(QUrl("file:///" + filePath, QUrl::TolerantMode));
}



