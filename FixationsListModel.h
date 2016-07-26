#ifndef FIXATIONSLISTMODEL_H
#define FIXATIONSLISTMODEL_H

#include <QAbstractTableModel>
#include "armadillo"
#include <QFont>

using namespace arma;

class FixationsListModel : public QAbstractTableModel
{
    Q_OBJECT;
public:
    FixationsListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void updateWithFixations(mat &newFixations, int highlight);
private:
    mat fixations;
    int highlightedRow;
    QFont defaultFont;
    QFont highlightFont;
};

#endif // FIXATIONSLISTMODEL_H
