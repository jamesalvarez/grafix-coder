#include "GrafixProject.h"


GrafixProject* GrafixParticipant::GetProject() const
{
    return this->_project;
}

bool GrafixParticipant::Rename(QString new_name)
{
    bool succeed = true;
    QString old_name = _name;
    QString old_path, new_path;

    if (old_name == new_name) return succeed;

    old_path = this->GetFullDirectory() + "/smooth_" + old_name + ".csv";
    new_path = this->GetFullDirectory() + "/smooth_" + new_name + ".csv";
    if (succeed && QFile(old_path).exists())
        succeed = QFile(old_path).copy(new_name);
    old_path = this->GetFullDirectory() + "/fix_all_" + old_name + ".csv";
    new_path = this->GetFullDirectory() + "/fix_all_" + new_name + ".csv";
    if (succeed && QFile(old_path).exists())
        succeed = QFile(old_path).copy(new_path);
    old_path = this->GetFullDirectory() + "/fix_auto_" + old_name + ".csv";
    new_path = this->GetFullDirectory() + "/fix_auto_" + new_name + ".csv";
    if (succeed && QFile(old_path).exists())
        succeed = QFile(old_path).copy(new_path);

    if (succeed)
    {
        _name = new_name;
        old_path = this->GetFullDirectory() + "/smooth_" + old_name + ".csv";
        QFile(old_path).remove();
        old_path = this->GetFullDirectory() + "/fix_all_" + old_name + ".csv";
        QFile(old_path).remove();
        old_path = this->GetFullDirectory() + "/fix_auto_" + old_name + ".csv";
        QFile(old_path).remove();
    }
    else
    {
        //remove files that we created but are no longer necessary
        new_path = this->GetFullDirectory() + "/smooth_" + new_name + ".csv";
        QFile(new_path).remove();
        new_path = this->GetFullDirectory() + "/fix_all_" + new_name + ".csv";
        QFile(new_path).remove();
        new_path = this->GetFullDirectory() + "/fix_auto_" + new_name + ".csv";
        QFile(new_path).remove();
    }
    return succeed;
}

QString GrafixParticipant::GetMatrixPath(Consts::MATRIX_TYPE type)
{
    switch (type)
    {
    case Consts::MATRIX_ROUGH:
        if (!this->PATH_ROUGH.isEmpty())
            return this->GetFullDirectory() + "/" + this->PATH_ROUGH;
        else
            return QString("");
        break;
    case Consts::MATRIX_SMOOTH:
        return this->GetFullDirectory() + "/smooth_" + this->_name + ".csv";
        break;
    case Consts::MATRIX_FIXALL:
        return this->GetFullDirectory() + "/fix_all_" + this->_name + ".csv";
        break;
    case Consts::MATRIX_AUTOFIXALL:
        return this->GetFullDirectory() + "/fix_auto_" + this->_name + ".csv";
        break;
    case Consts::MATRIX_SEGMENTS:
        if (!this->EXPE_SEGMETS_PATH.isEmpty())
            return this->GetFullDirectory() + "/" + this->EXPE_SEGMETS_PATH;
        else
            return QString("");
        break;
    }

    return "";
}

QString GrafixParticipant::GetName()
{
    return _name;
}

GrafixConfiguration GrafixParticipant::GetConfiguration()
{
    return _configuration;
}

void GrafixParticipant::SetConfiguration(GrafixConfiguration configuration)
{
    QSettings settings(this->GetParticipantSettingsPath(), QSettings::IniFormat);
    settings.setValue(Consts::PSETTING_CONFIGURATION, configuration);
    settings.setValue(Consts::PSETTING_CONFIGURATION_CHANGE_DATE, QDateTime::currentDateTime());
    _configuration = configuration;
}


void GrafixParticipant::SetParticipantSetting(QString setting, QVariant value)
{
    QSettings settings(this->GetParticipantSettingsPath(), QSettings::IniFormat);
    settings.setValue(setting, value);
}

QVariant GrafixParticipant::GetParticipantSetting(QString setting)
{
     QSettings settings(this->GetParticipantSettingsPath(), QSettings::IniFormat);
     QVariant value = settings.value(setting);
     return value;
}

GrafixParticipant::GrafixParticipant(GrafixProject* parent, QString directory)
{
    this->_project = parent;
    this->_participantdirectory = directory;
    this->_name = directory;
    this->_configuration = 0;
}

QString GrafixParticipant::GetParticipantSettingsPath()
{
    return this->GetFullDirectory() + "/participant.ini";
}



bool GrafixParticipant::HasDataMatrix(Consts::MATRIX_TYPE type)
{
    //check if file exists
    QString path = GetMatrixPath(type);
    if (!path.isEmpty())
    {
        QFile file(path);
        return file.exists();
    }
    else
    {
        return false;
    }
}


QString GrafixParticipant::GetRelativePath(QString path)
{
    QDir directory(this->GetFullDirectory());
        return directory.relativeFilePath(path);
}


bool GrafixParticipant::HasSettings()
{
    return QFile(GetParticipantSettingsPath()).exists();
}

bool GrafixParticipant::HasDirectory()
{
    //check if current _directory is actually a directory
    if (!_participantdirectory.isEmpty())//if string is empty QDir says it exists...
    {
        return QDir(_participantdirectory).exists();
    }
    else
    {
        return false;
    }
}

void GrafixParticipant::SaveSettings()
{
    QSettings settings(GetParticipantSettingsPath(), QSettings::IniFormat);
    settings.setValue(Consts::PSETTING_LAST_SAVED_DATE,QDateTime::currentDateTime());
    settings.setValue(Consts::PSETTING_NAME,this->_name);
    settings.setValue(Consts::PSETTING_ROUGH_PATH,this->PATH_ROUGH);
    settings.setValue(Consts::PSETTING_SEGMENTS_PATH,this->EXPE_SEGMETS_PATH);
    settings.setValue(Consts::PSETTING_NOTES, this->Notes);
    settings.setValue(Consts::PSETTING_CONFIGURATION, this->_configuration);
}

void GrafixParticipant::LoadSettings()
{
    QSettings settings(GetParticipantSettingsPath(), QSettings::IniFormat);
    this->_name = settings.value(Consts::PSETTING_NAME).toString();
    this->PATH_ROUGH = settings.value(Consts::PSETTING_ROUGH_PATH).toString();
    this->EXPE_SEGMETS_PATH = settings.value(Consts::PSETTING_SEGMENTS_PATH).toString();
    this->Notes = settings.value(Consts::PSETTING_NOTES).toString();
    this->_configuration = settings.value(Consts::PSETTING_CONFIGURATION).toUInt();
}

QString GrafixParticipant::GetRelativeDirectory()
{
    return this->_participantdirectory;
}

QString GrafixParticipant::GetFullDirectory()
{
    return _project->GetFullDirectory() + "/" + this->_participantdirectory;
}

void GrafixParticipant::DeleteDirectory()
{
    QDir(this->GetFullDirectory()).removeRecursively();
}

QString GrafixParticipant::OpenAndCopyCSVFile(Consts::MATRIX_TYPE type, QWidget* parent = 0, QString title = "Open File", QString filter = "*.*")
{
    QString fileName = QFileDialog::getOpenFileName(parent, title, this->GetFullDirectory(), filter);
    if (!fileName.isEmpty())
    {
        bool file_is_ok = true;

        switch (type)
        {
        case Consts::MATRIX_ROUGH:
            {
                int n_cols = GPMatrixFunctions::fncGetMatrixColsFromFile(fileName);
                if (n_cols == 6 || n_cols == 8 )
                    file_is_ok = true;
                else
                {
                    DialogGrafixError::AlertNewError("Error: Incorrect file format for rough file");
                    file_is_ok = false;
                }
            }
            break;
        default:
            file_is_ok = true;
            break;
        }
        QString relativeFileName = this->GetRelativePath(fileName);
        if (file_is_ok && relativeFileName[0] == '.')//TODO better way of seeing if in directory?
        {
            QMessageBox qmb(parent);
            qmb.setText("File not in participant directory.");
            qmb.setInformativeText("Do you want to copy this file to the participant folder?");
            qmb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            qmb.setDefaultButton(QMessageBox::Yes);
            int ret = qmb.exec();
            switch (ret)
            {
            case QMessageBox::Yes:
                if (QDir(this->GetFullDirectory()).exists())
                {
                    QString newFileName =  this->GetFullDirectory() + "/" + QFileInfo(relativeFileName).fileName();
                    if (QFile::copy(fileName,newFileName))
                    {
                        fileName = newFileName;
                        relativeFileName = this->GetRelativePath(fileName);
                    }
                    else
                    {
                        DialogGrafixError::AlertNewError("Error: Could not copy file.");
                    }
                }
                else
                {
                    DialogGrafixError::AlertNewError("Error: Directory not found.");
                }
                break;
            case QMessageBox::No:
                break;
            case QMessageBox::Cancel:
                break;
            }
        }

        if (file_is_ok)
            return relativeFileName;
        else
            return "";
    }
    return "";
}

GrafixProject::GrafixProject()
{

}

GrafixProject::~GrafixProject()
{
    cleanParticipants();
}

void GrafixProject::cleanParticipants()
{
    //Delete Grafix Participant
    foreach (GrafixParticipant* p, _participants)
        delete p;
    _participants.clear();
}

int GrafixProject::numParticipants()
{
    return _participants.size();
}

GrafixParticipant* GrafixProject::GetParticipant(int id)
{
    return _participants[id];
}

bool GrafixProject::HasParticipant(int pos)
{
    if (pos>=0 && pos<_participants.size())
        return true;
    return false;
}

GrafixParticipant* GrafixProject::AddParticipant(QString existing_directory)
{
    QString newpath = this->_directory + "/" + existing_directory;
    if(QDir(newpath).exists())
    {
        GrafixParticipant *n = new GrafixParticipant(this,existing_directory);
        _participants.insert(_participants.size(),n); //should i push back this with pointer?
        return n;
    }
    else
    {
        DialogGrafixError::AlertNewError("Error: Directory does not exist.");
    }
    return NULL;
}

void GrafixProject::AddParticipant(int position)
{
    int pn = _participants.size();
    QString oss = "";

    while (QDir(this->_directory + "/" + oss).exists())
    {
        ++pn;
        QString text;
        text = QString::number(pn);
        for(int zeros = text.length(); zeros<5;zeros++)
        {
            text = "0" + text;
        }
        oss = "P" + text;
        if ((pn) > (10+_participants.size()))
        {
            DialogGrafixError::AlertNewError("Cannot create new folder for participant");
            return;
        }
    }

    QString newpath = this->_directory + "/" + oss;
    if(QDir().mkdir(newpath))
    {
        GrafixParticipant *n = new GrafixParticipant(this,oss);
        _participants.insert(position,n); //should i push back this with pointer?
    }
    else
    {
        DialogGrafixError::AlertNewError("Cannot create new folder for participant");
    }

}

void GrafixProject::RemoveParticipant(int position)
{
    delete _participants[position];
    _participants.removeAt(position);
}


bool GrafixProject::HasDirectory()
{
    //check if current _directory is actually a directory
    if (!_directory.isEmpty())//if string is empty QDir says it exists...
    {
        return QDir(_directory).exists();
    }
    else
    {
        return false;
    }
}

QString GrafixProject::GetFullDirectory()
{
    return _directory;
}


QString GrafixProject::GetProjectSettingsPath()
{
    return _directory + "/project.ini";
}

void GrafixProject::SetProjectSetting(QString setting, GrafixConfiguration configuration, QVariant value)
{
    QSettings settings(this->GetProjectSettingsPath(), QSettings::IniFormat);
    settings.setValue(setting + "_" + QString::number(configuration), value);
    settings.setValue(Consts::SETTINGS_CONFIGURATION_CHANGED_DATE, QDateTime::currentDateTime());
}

QVariant GrafixProject::GetProjectSetting(QString setting, GrafixConfiguration configuration)
{
     QSettings settings(this->GetProjectSettingsPath(), QSettings::IniFormat);

     QVariant value = settings.value(setting + "_" + QString::number(configuration));

     if (value.isNull())
     {
         if (setting == Consts::SETTING_HZ)
             return QVariant(Consts::DEFAULT_SETTING_HZ);
         else if (setting == Consts::SETTING_EXP_WIDTH)
             return QVariant(Consts::DEFAULT_SETTING_EXP_WIDTH);
         else if (setting == Consts::SETTING_EXP_HEIGHT)
             return QVariant(Consts::DEFAULT_SETTING_EXP_HEIGHT);
         else if (setting == Consts::SETTING_SECS_FRAGMENT)
             return QVariant(Consts::DEFAULT_SETTING_SECS_FRAGMENT);
         else if (setting == Consts::SETTING_DEGREE_PER_PIX)
             return QVariant(Consts::DEFAULT_SETTING_DEGREE_PER_PIXEL);
         else if (setting == Consts::SETTING_INTERP_LATENCY)
             return QVariant(Consts::DEFAULT_SETTING_INTERP_LATENCY);
         else if (setting == Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL)
             return QVariant(Consts::DEFAULT_SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL);
         else if (setting == Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT)
             return QVariant(Consts::DEFAULT_SETTING_INTERP_MAXIMUM_DISPLACEMENT);
         else if (setting == Consts::SETTING_POSTHOC_MIN_DURATION_VAL)
             return QVariant(Consts::DEFAULT_SETTING_POSTHOC_MIN_DURATION_VAL);
         else if (setting == Consts::SETTING_INTERP_VELOCITY_THRESHOLD)
             return QVariant(Consts::DEFAULT_SETTING_INTERP_VELOCITY_THRESHOLD);
         else if (setting == Consts::SETTING_POSTHOC_LIMIT_RMS_VAL)
             return QVariant(Consts::DEFAULT_SETTING_POSTHOC_LIMIT_RMS_VAL);
         else if (setting == Consts::SETTING_SMOOTHING_USE_OTHER_EYE)
             return QVariant(Consts::DEFAULT_SETTING_SMOOTHING_USE_OTHER_EYE);
         else if (setting == Consts::SETTING_SMOOTHING_SIGMA_R)
             return QVariant(Consts::DEFAULT_SETTING_SMOOTHING_SIGMA_R);
         else if (setting == Consts::SETTING_SMOOTHING_SIGMA_S)
             return QVariant(Consts::DEFAULT_SETTING_SMOOTHING_SIGMA_S);
         else
             return value;
     }
     return value;
}

bool GrafixProject::HasSettings()
{
    return QFile(this->GetProjectSettingsPath()).exists();
}

void GrafixProject::SaveSettings()
{
    //Save project
    QSettings settings(this->GetProjectSettingsPath(), QSettings::IniFormat);
    settings.setValue("Last Saved:",QDateTime::currentDateTime());
    settings.setValue("Participants:",QString::number(_participants.size()));
    //Save participants
    for (int i=0;i<_participants.size();i++)
    {
        QString pcode = "ParticipantD:" + QString::number(i);
        settings.setValue(pcode,_participants[i]->GetRelativeDirectory());
        _participants[i]->SaveSettings();
    }
}

void GrafixProject::NewBlankProject(QString dir)
{
    _directory = dir;
    QString PATH_ROUGH = "";
    QString EXPE_SEGMETS_PATH = "";
    QString Name = "";
    cleanParticipants();
}

void GrafixProject::NewProjectFromExistingDirectory(QString dir, QWidget* _parent)
{
    QDir currentDir(dir);
    this->NewBlankProject(dir);

    //rename existing project.ini, delete oldproject.ini if already exists
    //if none of this is possible then
    if (QFile(dir+"/project.ini").exists())
    {
        QString newName = dir+"/oldproject.ini";
        if(!QFile(dir+"/project.ini").rename(newName))
        {
            if (QFile(newName).remove())
            {
                if (QFile(dir+"/project.ini").rename(newName))
                {
                    //it worked
                }
                else
                {
                    DialogGrafixError::AlertNewError("Cannot create project.ini, please delete this file manually");
                    return;
                }
            }
            else
            {
                DialogGrafixError::AlertNewError("Cannot create project.ini, please delete this file manually");
                return;
            }
        }
    }

    currentDir.setFilter(QDir::Dirs);
    QStringList entries = currentDir.entryList();
    QStringList roughFilter("*rough*.csv");
    QStringList segmentsFilter("*segments*.csv");
    QStringList csvFilter("*.csv");

    DialogGrafixError::LogNewError(NULL,"Building project from existing files...");
    for( QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry )
    {
        QString dirname=*entry;
        if(dirname != "." && dirname != "..")
        {
            GrafixParticipant* p = this->AddParticipant(dirname);
            QDir subDir(dir + "/" + dirname); //add dirname to path
    //ROUGH search for rough files
            subDir.setNameFilters(roughFilter);
            subDir.setFilter(QDir::Files);
            QStringList roughFiles = subDir.entryList();
            if (roughFiles.count()>1)
            {
                //open box to select which one
                p->PATH_ROUGH = p->OpenAndCopyCSVFile(Consts::MATRIX_ROUGH,_parent,"Select Rough File for participant " + dirname, roughFilter.at(0));
            }
            else if (roughFiles.count() == 1)
            {
                //just one file found
                QString roughname = roughFiles.at(0);
                if (p != NULL)
                {
                    p->PATH_ROUGH = roughname;
                }
            }
            else
            {
                //no rough files found but any .csv files?
                subDir.setNameFilters(csvFilter);
                subDir.setFilter(QDir::Files);
                QStringList csvFiles = subDir.entryList();
                if (csvFiles.count()>=1)
                {
                    //open box to select which one
                    p->PATH_ROUGH = p->OpenAndCopyCSVFile(Consts::MATRIX_ROUGH, _parent, "Select Rough File for participant " + dirname, csvFilter.at(0));
                }
                else
                {
                    DialogGrafixError::LogNewError(p,"Error: Could not find rough file.");
                }
            }
    //SEGMENT seearch for segment files
            subDir.setNameFilters(segmentsFilter);
            QStringList segmentFiles = subDir.entryList();
            if (segmentFiles.count()>1)
            {
                //open box to select which one
                //open box to select which one
                p->EXPE_SEGMETS_PATH = p->OpenAndCopyCSVFile(Consts::MATRIX_SEGMENTS, _parent,"Select Segments File for participant " + dirname, segmentsFilter.at(0));
            }
            else if (segmentFiles.count() == 1)
            {
                //just one file found
                QString segmentname = segmentFiles.at(0);

                if (p != NULL)
                {
                    p->EXPE_SEGMETS_PATH = segmentname;
                }
            }
            else
            {
                DialogGrafixError::LogNewError(p,"Error: Could not find segments file.");
            }

        }
    }
    DialogGrafixError::LogNewError(NULL,"Project built.");
}
void GrafixProject::LoadSettings(QString d)
{

    _directory = d;

    if (this->HasDirectory() && this->HasSettings())
    {
        QSettings settings(this->GetProjectSettingsPath(), QSettings::IniFormat);
        int numberparticipants = settings.value("Participants:").toInt();
        for (int i = 0; i < numberparticipants; i++)
        {
            QString pcode = "ParticipantD:" + QString::number(i);
            QString path = settings.value(pcode).toString();
            GrafixParticipant *n = new GrafixParticipant(this,path);
            _participants.insert(i,n);
            n->LoadSettings();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Cannot load project or no project.ini file found - please create new project.");
        msgBox.exec();
    }

}

