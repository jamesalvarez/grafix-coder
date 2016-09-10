#ifndef GPPARTICIPDIALOG_H
#define GPPARTICIPDIALOG_H

//#include <QtWidgets\QDialog>
#include <QFileDialog>
#include "GrafixProject.h"
#include "DialogParticipantEditNotes.h"

namespace Ui {
class DialogParticipantPaths;
}

class DialogParticipantPaths : public QDialog
{
    Q_OBJECT

public:
    explicit DialogParticipantPaths(GrafixParticipant* participant, QWidget *parent = 0);
    ~DialogParticipantPaths();
    void fncSavePaths();

private:
    Ui::DialogParticipantPaths *ui;
    GrafixParticipant *_participant;
    void accept();

public slots:

    void fncPress_buttonOpenRough();
    void fncPress_buttonOpenSegments();
    void fncPress_buttonNotes();
    void fncPress_buttonOpenDirectory();
};

#endif // GPPARTICIPDIALOG_H

