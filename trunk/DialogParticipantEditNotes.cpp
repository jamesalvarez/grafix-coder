#include "DialogParticipantEditNotes.h"
#include "ui_DialogParticipantEditNotes.h"

DialogParticipantEditNotes::DialogParticipantEditNotes(GrafixParticipant *participant, QWidget *parent) :
    QDialog(parent),
    _participant(participant),
    ui(new Ui::DialogParticipantEditNotes)
{
    ui->setupUi(this);
    ui->tName->setText(this->_participant->GetName());
    ui->tNotes->setPlainText(this->_participant->Notes);
}

DialogParticipantEditNotes::~DialogParticipantEditNotes()
{


    delete ui;
}

void DialogParticipantEditNotes::accept()
{
    this->_participant->Rename(ui->tName->text());
    this->_participant->Notes = ui->tNotes->document()->toPlainText();
    this->_participant->SaveSettings();
    QDialog::accept();
}
