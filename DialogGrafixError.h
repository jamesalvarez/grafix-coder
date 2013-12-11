#ifndef DialogGrafixError_H
#define DialogGrafixError_H

#include <QDialog>
#include "GrafixProject.h"

namespace Ui {
class DialogGrafixError;
}

class GrafixParticipant;

class DialogGrafixError : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGrafixError(QWidget *parent = 0);
    ~DialogGrafixError();
    static void LogNewError(GrafixParticipant* participant, QString Error);
    static void AlertNewError(QString Error);
    static void ShowDialog();
public:
    struct BatchError
    {
    public:
        QString ParticipantName;
        QString Error;
    };


    Ui::DialogGrafixError *ui;

private:
    static QString                 GetErrorList();
    static QList<BatchError>       _error_list;
    static DialogGrafixError*          _mainobject;

public slots:
    void fnc_bClearPress();



};

#endif // DialogGrafixError_H
