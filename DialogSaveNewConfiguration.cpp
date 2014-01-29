#include "DialogSaveNewConfiguration.h"
#include "ui_DialogSaveNewConfiguration.h"

DialogSaveNewConfiguration::DialogSaveNewConfiguration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSaveNewConfiguration)
{
    ui->setupUi(this);


    ui->lwConfigurations->setContextMenuPolicy(Qt::CustomContextMenu);
}

DialogSaveNewConfiguration::~DialogSaveNewConfiguration()
{
    delete ui;
}

void DialogSaveNewConfiguration::loadData(GrafixConfiguration &configuration, GrafixProject &project)
{
    this->_selected_configuration = configuration;
    this->_original_configuration = configuration;
    this->p_project = &project;
    this->p_configurations = this->p_project->GetConfigurations();
    fncPopulateList();
}

GrafixConfiguration DialogSaveNewConfiguration::Selected()
{
    return this->_selected_configuration;
}

void DialogSaveNewConfiguration::fncPopulateList()
{
    //load configurations
    ui->lwConfigurations->clear();
    for (int i = 0; i < (*p_configurations).size(); ++i)
    {
        ui->lwConfigurations->addItem((*p_configurations)[i].first);
    }

    //select current configuration;
    bool selected_item = false;
    for (int i = 0; i < (*p_configurations).size(); ++i)
    {
        if ((*p_configurations)[i].first == _selected_configuration.first)
        {
            ui->lwConfigurations->item(i)->setSelected(true);
            selected_item = true;
            break;
        }
    }

   if (!selected_item)
       ui->lwConfigurations->item(0)->setSelected(true);

    ui->lActiveConfiguration->setText("Selected configuration: " + _selected_configuration.first);
}

void DialogSaveNewConfiguration::on_bAdd_clicked()
{
    QString name = ui->leNewConfig->text().trimmed();
    if (name.isEmpty()) return;

    //first check if already has a name
    bool unique_name = true;
    for (int i = 0; i < (*p_configurations).size(); ++i)
    {
        if ((*p_configurations)[i].first == ui->leNewConfig->text())
            unique_name = false;
    }

    if (!unique_name)
    {
        ui->leNewConfig->setText("");
        return;
    }

    GrafixConfiguration gc;
    gc.first = name;

    //now find a unique id number
    bool unique_id = false;
    gc.second = 0;
    while(!unique_id)
    {
        ++(gc.second);
        unique_id = true;
        for (int i = 0; i < (*p_configurations).size(); ++i)
        {
            if ((*p_configurations)[i].second == gc.second)
                unique_id = false;
        }
    }
    int new_item_index = (*p_configurations).size();
    (*p_configurations).insert(new_item_index,gc);

    //now copy active configuration
    this->p_project->SaveConfiguration((*p_configurations)[new_item_index]);

    //set active
    _selected_configuration = (*p_configurations)[new_item_index];

    fncPopulateList();
}

void DialogSaveNewConfiguration::on_buttonBox_accepted()
{
    //activate selected configuration
    QString selected_item = ui->lwConfigurations->selectedItems().first()->text();
    for (int i = 0; i < (*p_configurations).size(); ++i)
    {
        if ((*p_configurations)[i].first == selected_item)
        {
            _selected_configuration = (*p_configurations)[i];
            break;
        }
    }
}

void DialogSaveNewConfiguration::on_buttonBox_rejected()
{
    _selected_configuration = _original_configuration;
}

void DialogSaveNewConfiguration::on_lwConfigurations_itemClicked(QListWidgetItem *item)
{
    QString selected_item = item->text();
    for (int i = 0; i < (*p_configurations).size(); ++i)
    {
        if ((*p_configurations)[i].first == selected_item)
        {
            _selected_configuration = (*p_configurations)[i];
            break;
        }
    }
    ui->lActiveConfiguration->setText("Selected configuration: " + _selected_configuration.first);
}

void DialogSaveNewConfiguration::on_lwConfigurations_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
}

void DialogSaveNewConfiguration::on_lwConfigurations_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->lwConfigurations->mapToGlobal(pos);

    QListWidgetItem* item = ui->lwConfigurations->itemAt(pos);

    if (item->text() == Consts::ACTIVE_CONFIGURATION().first) return;

    QMenu myMenu;
    myMenu.addAction("Remove");

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        if (selectedItem->text() == "Remove")
        {
            QMessageBox msgBox;
            msgBox.setText("Removing Configuration");
            msgBox.setInformativeText("Are you sure you want to remove the configuration '" + item->text() +"'?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
            msgBox.setDefaultButton(QMessageBox::Save);
            msgBox.setButtonText(QMessageBox::Discard,"Delete");
            msgBox.setButtonText(QMessageBox::Save,"Keep");
            int ret = msgBox.exec();

            switch (ret)
            {
            case QMessageBox::Discard:

                //find index of selected item and delete
                for (int i = 1; i < (*p_configurations).size(); ++i)
                {
                    if ((*p_configurations)[i].first == item->text())
                    {
                        //found item
                        if (_selected_configuration == (*p_configurations)[i])
                        {
                            //unselect it
                            _selected_configuration = (*p_configurations)[0];
                        }
                        (*p_configurations).removeAt(i);
                        break;
                    }
                }
                fncPopulateList();
                break;
            case QMessageBox::Save:
                break;
            }
        }
    }
}
