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

    int fncGetMatrixColsFromFile(QString fullpath);
    double string_to_double_fast(const unsigned char *p, unsigned int &move_on);

public slots:

    void fncPress_buttonOpenRough();
    void fncPress_buttonOpenSegments();
    void fncPress_buttonNotes();
    void fncPress_buttonOpenDirectory();
};

#endif // GPPARTICIPDIALOG_H

