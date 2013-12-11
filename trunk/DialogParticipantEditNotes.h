#ifndef GPPARTICIPANTEDITNOTES_H
#define GPPARTICIPANTEDITNOTES_H

#include <QDialog>
#include "GrafixProject.h"

namespace Ui {
class DialogParticipantEditNotes;
}

class DialogParticipantEditNotes : public QDialog
{


    Q_OBJECT

public:
    explicit DialogParticipantEditNotes(GrafixParticipant* participant, QWidget *parent = 0);
    ~DialogParticipantEditNotes();

private:
    GrafixParticipant *_participant;
    Ui::DialogParticipantEditNotes *ui;
    void accept();
};

#endif // GPPARTICIPANTEDITNOTES_H
