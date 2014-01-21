#ifndef DIALOGSAVENEWCONFIGURATION_H
#define DIALOGSAVENEWCONFIGURATION_H

#include "MyConstants.h"
#include "GrafixProject.h"

#include <QListWidget>
#include <QDialog>
#include <QMenu>

namespace Ui {
class DialogSaveNewConfiguration;
}

class DialogSaveNewConfiguration : public QDialog
{
    Q_OBJECT
private:
    QList<GrafixConfiguration>* p_configurations;
    GrafixConfiguration* p_configuration;
    GrafixConfiguration* p_original_configuration;
    GrafixProject* p_project;

public:
    explicit DialogSaveNewConfiguration(QWidget *parent = 0);
    ~DialogSaveNewConfiguration();

    void loadData(GrafixConfiguration *configuration, GrafixProject *project);

private slots:
    void on_bAdd_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_lwConfigurations_clicked(const QModelIndex &index);

    void on_lwConfigurations_itemClicked(QListWidgetItem *item);

    void on_lwConfigurations_customContextMenuRequested(const QPoint &pos);

private:
    Ui::DialogSaveNewConfiguration *ui;

    void fncPopulateList();
};

#endif // DIALOGSAVENEWCONFIGURATION_H
