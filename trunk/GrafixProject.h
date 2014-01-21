#ifndef GRAFIXPROJECT_H
#define GRAFIXPROJECT_H

#include "MyConstants.h"
#include "GPMatrixFunctions.h"

#include "DialogGrafixError.h"

#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QFileDialog>

#include "armadillo"

using namespace arma;

class GrafixProject;

//typedef struct { QString first; int second; } GrafixConfiguration;

//Class holds participant information as well as methods for
//accessing settings, matrixes, and file names.
class GrafixParticipant
{
public:

    //Constructor
    GrafixParticipant(GrafixProject* parent, QString directory);

    // Paths, names and notes;
    QString PATH_ROUGH;
    QString EXPE_SEGMETS_PATH;
    QString Notes;

    //Returns a const pointer to the project
    GrafixProject* GetProject() const;

    //Returns full path and file name for a given type of matrix
    QString     GetMatrixPath(Consts::MATRIX_TYPE type);

    //Returns participant.ini with full path
    QString     GetParticipantSettingsPath();

    //Returns a setting from the participant setting (use MyConstants.SETTINGS_...)
    QVariant    GetParticipantSetting(QString setting);

    //Sets a setting from the participant setting (use MyConstants.SETTINGS_...)
    void        SetParticipantSetting(QString setting, QVariant value);

    //Returns true if participant has matrix data for a given type
    bool        HasDataMatrix(Consts::MATRIX_TYPE type);

    //Returns true if participant has a settings file
    bool        HasSettings();

    //Returns true if participant has a valid directory
    bool        HasDirectory();

    //Saves paths, name and notes
    void        SaveSettings();

    //Loads paths name and notes
    void        LoadSettings();

    //Deletes entire participant directory
    void        DeleteDirectory();

    //Gets directory name relative to project path
    QString     GetRelativeDirectory();

    //Gets full path to directory
    QString     GetFullDirectory();

    //Returns a path relative to the participant directory
    QString     GetRelativePath(QString path);

    //Returns a path relative to participant directory,
    //after checking CSV is ok, and offering to copy it.
    QString     OpenAndCopyCSVFile(Consts::MATRIX_TYPE type,
                                   QWidget *parent,
                                   QString title,
                                   QString filter);

    //Renames participant and all files
    bool        Rename(QString new_name);

    QString     GetName();

private:
    GrafixProject*  _project;
    QString         _participantdirectory;
    QString         _name;
};

//class holds all participant objects, and provides methods to save
//project wide settings, and create new projects etc
class GrafixProject
{
    friend class GrafixParticipant;

public:

    //Constructor / destructor
    GrafixProject();
    ~GrafixProject();
	
    //Returns pointer to a specific participant at a certain id
    GrafixParticipant * GetParticipant(int id);

    //Adds and returns a participant to project using existing_directory
    GrafixParticipant*  AddParticipant(QString existing_directory);

    //Checks that a participant exists
    bool        HasParticipant(int pos);

    //Adds a participant at a certain position
    void        AddParticipant(int position);

    //Removes a particiapnt at a certain position
    void        RemoveParticipant(int position);

    //Returns the number of particiapnts
    int         numParticipants();

    //Replaces participant in a certain position - can lead to duplication
    //void        ReplaceParticipant(int position, GrafixParticipant * participant);

    //Creates a new blank project with directory (you must first check it exists)
    void        NewBlankProject(QString dir);

    //Creates a blank project from an existing directory structure
    //  - give _parent for dialog boxes
    void        NewProjectFromExistingDirectory(QString dir, QWidget* _parent);

    //Returns true if _directory exists
    bool        HasDirectory();

    //Returns _directory
    QString     GetFullDirectory();

    //Returns true if project has a settings file
    bool        HasSettings();

    //Saves all project settings related to participants
    void        SaveSettings();

    //Loads all participants and their settings
    bool        LoadProjectSettings(QString d);

    //Returns full path to project.ini
    QString     GetProjectSettingsPath();

    //Sets a project.ini setting
    QVariant    GetProjectSetting(QString setting, GrafixConfiguration configuration);

    //Gets a project.ini setting
    void        SetProjectSetting(QString setting, GrafixConfiguration configuration, QVariant value);

    //Saves a new configuration
    void        SaveConfiguration(GrafixConfiguration configuration);

    //Loads a new configuration
    void        ActivateConfiguration(GrafixConfiguration configuration);

    //gets all configurations
    QList<GrafixConfiguration> *GetConfigurations();

private:
    QList<GrafixConfiguration> _configurations;
    QList<GrafixParticipant> _participants;
    QString _directory;

    //deletes all participant objects and clears list
    void cleanParticipants();

    //Copies a configuration to a new slot
    void CopyConfiguration(GrafixConfiguration from, GrafixConfiguration to);
};

#endif // GRAFIXPROJECT_H


