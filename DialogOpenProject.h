#ifndef DialogOpenProject_H
#define DialogOpenProject_H

#include <QMenu>
#include <QFont>
#include "GPTableModel.h"
#include "GrafixProject.h"
#include "DialogParticipantPaths.h"


namespace Ui {
    class DialogOpenProject;
}

class QMenu;

class DialogOpenProject : public QDialog {


public:
    explicit DialogOpenProject(GrafixProject *project, QWidget *parent = 0);
    ~DialogOpenProject();

public slots:
    void fncItemDoubleClicked(QModelIndex index);
    void fncAddParticipantClicked();
    void fncNewProjectClicked();
    void fncLoadProjectClicked();
    void fncPress_buttonOpenDirectory();

private slots:
    void on_tableViewParticipants_customContextMenuRequested(const QPoint &pos);

private:

    Q_OBJECT

    Ui::DialogOpenProject *ui;
    GPTabelModel *_model;
    GrafixProject *_project;

    //void fncEditParticipant(int position);
    void accept();

    QAction* editParticipant;
    QAction* deleteParticipant;
};

#endif // DialogOpenProject_H

