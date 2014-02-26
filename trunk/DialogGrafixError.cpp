#include "DialogGrafixError.h"
#include "ui_DialogGrafixError.h"


QList<DialogGrafixError::BatchError> DialogGrafixError::_error_list;
DialogGrafixError*                   DialogGrafixError::_mainobject = NULL;

DialogGrafixError::DialogGrafixError(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGrafixError)
{
    ui->setupUi(this);
    ui->tErrorDisplay->setPlainText(DialogGrafixError::GetErrorList());
    connect( ui->bClear, SIGNAL(clicked()),this,SLOT(fnc_bClearPress()));
    DialogGrafixError::_mainobject = this;
}

bool DialogGrafixError::new_log_file_created = false;

DialogGrafixError::~DialogGrafixError()
{
    if (DialogGrafixError::_mainobject != NULL)
        delete DialogGrafixError::_mainobject;
    DialogGrafixError::_mainobject = NULL;
    delete ui;
}

void DialogGrafixError::ShowDialog()
{
    if (DialogGrafixError::_mainobject == NULL)
        DialogGrafixError::_mainobject = new DialogGrafixError();

    DialogGrafixError::_mainobject->show();
    DialogGrafixError::_mainobject->raise();
    DialogGrafixError::_mainobject->activateWindow();
}

void DialogGrafixError::LogNewError(GrafixParticipant* participant, QString Error)
{
    if (!new_log_file_created)
    {
        std::ofstream outfile ("grafix.err");
        outfile << "Log: " << QDateTime::currentDateTime().toString().toStdString() << "\n";
        outfile.close();
        new_log_file_created = true;
    }

    std::ofstream log("grafix.err", std::ios_base::app | std::ios_base::out);

    log << QDateTime::currentDateTime().time().toString().toStdString() << ": ";
    DialogGrafixError::BatchError err;

    if (participant != NULL)
    {
        err.ParticipantName = participant->GetName();
        log << "Participant: " << err.ParticipantName.toStdString() << " ";
    }
    else
    {
        err.ParticipantName = "";

    }


    err.Error = Error;
    log << "- " << Error.toStdString() << "\n";

    log.close();

    DialogGrafixError::_error_list.append(err);

    //now update main object (if visible
    if (_mainobject != NULL)
        _mainobject->ui->tErrorDisplay->setPlainText(DialogGrafixError::GetErrorList());

}

void DialogGrafixError::AlertNewError(QString Error)
{
    QMessageBox msgBox;
    msgBox.setText(Error);
    msgBox.exec();
}

QString DialogGrafixError::GetErrorList()
{
    if (DialogGrafixError::_error_list.size() == 0)
        return "";

    QString output;
    QList<DialogGrafixError::BatchError>::iterator i;
    for (i = DialogGrafixError::_error_list.begin(); i != DialogGrafixError::_error_list.end(); ++i)
    {
        if ((*i).ParticipantName != "")
            output += "Name: " + (*i).ParticipantName + "\n";

        output += (*i).Error + "\n\n";
    }
    return output;
}

void DialogGrafixError::fnc_bClearPress()
{
    DialogGrafixError::_error_list.clear();
    ui->tErrorDisplay->setPlainText(DialogGrafixError::GetErrorList());
}
