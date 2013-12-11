#ifndef GPTABLEMODEL_H
#define GPTABLEMODEL_H

#include "GrafixProject.h"
#include "DialogParticipantPaths.h"
#include "DialogConfig.h"
#include "DialogGrafixError.h"
#include <QMessageBox>
#include <QAbstractTableModel>

class GPTabelModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    enum FourthColumn {
        FC_ROUGHFILE,
        FC_SMOOTHFILE,
        FC_INTERPOLATE,
        FC_ESTIMATE_FIX
    };

    GPTabelModel(QObject *parent = 0);
    GPTabelModel(GrafixProject* project, FourthColumn fc, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
    GrafixProject* getList();

    void LoadData(QString dir);
    void NewProject(QString dir);
    void NewProjectFromExistingDirectory(QString dir);
    void EditParticipant(int position);
private:
    GrafixProject* _project;
    FourthColumn _fc;
    QWidget* _parent;
};




#endif // GPTABLEMODEL_H

