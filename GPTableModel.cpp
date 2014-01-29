
#include "GPTableModel.h"

GPTabelModel::GPTabelModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

GPTabelModel::GPTabelModel(GrafixProject* project, FourthColumn fc, QObject *parent)
    : QAbstractTableModel(parent)
{
    _project = project;
    _fc = fc;
    _parent = (QWidget*)parent;
}


int GPTabelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (_project != NULL)
        return _project->numParticipants();
    else
        return 0;
}

int GPTabelModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}


QVariant GPTabelModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= _project->numParticipants() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        int row = index.row();
        int col = index.column();

        if (col == 0)
            return row;
        else if (col == 1)
            return _project->GetParticipant(row)->GetName();
        else if (col == 2)
            return _project->GetParticipant(row)->GetRelativeDirectory();
        else if (col == 3)
        {
            switch(_fc)
            {
            case FC_ROUGHFILE:
                if (_project->GetParticipant(row)->HasDataMatrix(Consts::MATRIX_ROUGH) &&
                        _project->GetParticipant(row)->HasDataMatrix(Consts::MATRIX_SEGMENTS))
                    return "Valid"; //replace with picture;
                else if (_project->GetParticipant(row)->HasDataMatrix(Consts::MATRIX_ROUGH))
                    return "No segment file.";
                else if (_project->GetParticipant(row)->HasDataMatrix(Consts::MATRIX_SEGMENTS))
                    return "No rough file.";
                else
                    return "Missing rough and segment files.";
            case FC_SMOOTHFILE:
                if (_project->GetParticipant(row)->HasDataMatrix(Consts::MATRIX_SMOOTH))
                    return "Smoothed"; //replace with picture;
                else
                    return "Missing";
            case FC_INTERPOLATE:
            {
                if (!_project->GetParticipant(row)->HasDataMatrix(Consts::MATRIX_SMOOTH))
                    return "Smooth first.";

                QVariant p = _project->GetParticipant(row)->GetParticipantSetting(Consts::PSETTING_INTERPOLATED_DATE);

                if (p.isNull())
                {
                    return "Never interpolated";
                }
                else
                {
                    QVariant s = _project->GetParticipant(row)->GetParticipantSetting(Consts::PSETTING_SMOOTHED_DATE);
                    if (p.toDateTime() < s.toDateTime())
                        return "Out of date.";
                    else
                        return "Up to date.";
                }
            }
            case FC_ESTIMATE_FIX:
            {
                if (!_project->GetParticipant(row)->HasDataMatrix(Consts::MATRIX_SMOOTH))
                    return "Smooth/interpolate first.";


                QVariant r = _project->GetParticipant(row)->GetParticipantSetting(Consts::PSETTING_ESTIMATED_FIX_DATE);

                if (r.isNull())
                {
                    return "Never estimated";
                }
                else
                {
                    QVariant p = _project->GetParticipant(row)->GetParticipantSetting(Consts::PSETTING_INTERPOLATED_DATE);
                    QVariant s = _project->GetParticipant(row)->GetParticipantSetting(Consts::PSETTING_SMOOTHED_DATE);
                    if (r.toDateTime() < s.toDateTime() ||
                        r.toDateTime() < p.toDateTime())
                        return "Out of date.";
                    else
                        return "Up to date.";
                }
            }
            }
        }
    }
    return QVariant();
}


QVariant GPTabelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0:
                return tr("Subject No.");
            case 1:
                return tr("Name");
            case 2:
                return tr("Directory");
            case 3:
                switch(_fc)
                {
                case FC_ROUGHFILE:
                    return "Rough Data";
                case FC_SMOOTHFILE:
                    return "Data Smoothed";
                case FC_INTERPOLATE:
                    return "Last interpolated";
                case FC_ESTIMATE_FIX:
                    return "Last estimated";
                }
            default:
                return QVariant();
        }
    }
    return QVariant();
}


bool GPTabelModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
    {
        _project->AddParticipant(position);
    }

    endInsertRows();
    return true;
}


bool GPTabelModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
    {
        _project->RemoveParticipant(position);
    }

    endRemoveRows();
    return true;
}


bool GPTabelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value)
    if (index.isValid() && role == Qt::EditRole)
    {
        //int row = index.row();


        if (index.column() == 1)
        {
            //TODO: allow change directory name?
            return false;
        }
        else
            return false;

        emit(dataChanged(index, index));

        return true;
    }

    return false;
}


Qt::ItemFlags GPTabelModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    //if (index.column()==1)
    //    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    //else
        return QAbstractTableModel::flags(index);//check
}


GrafixProject * GPTabelModel::getList()
{
    return _project;
}

void GPTabelModel::LoadData(QString dir)
{
    //check for project.ini file
    this->beginResetModel();
    _project->LoadProjectSettings(dir);
    this->endResetModel();
}

void GPTabelModel::NewProject(QString dir)
{
    if(QDir(dir).exists())
    {
        this->beginResetModel();
        _project->NewBlankProject(dir);
        this->endResetModel();
        DialogConfig w;
        w.loadData(_project);
        w.setWindowTitle(tr("Configure Default Options"));
        w.exec();
    }
    else
    {
        DialogGrafixError::AlertNewError("Directory does not exist.");
    }
}

void GPTabelModel::NewProjectFromExistingDirectory(QString dir)
{
    QDir currentDir(dir);
    if(!currentDir.exists())
    {
        DialogGrafixError::AlertNewError("Directory does not exist.");
    }
    else
    {
        this->beginResetModel();
        _project->NewProjectFromExistingDirectory(dir,_parent);
        this->endResetModel();
        DialogConfig w;
        w.loadData(_project);
        w.setWindowTitle(tr("Configure Default Options"));
        w.exec();
    }
}

void GPTabelModel::EditParticipant(int position)
{
    DialogParticipantPaths gpd(_project->GetParticipant(position));
    gpd.setWindowTitle(tr("Select participants files or create new ones"));
    this->beginResetModel();
    gpd.exec();
    this->endResetModel();
}


