#include "ProjectsHubPage.h"

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

ProjectsHubPage::ProjectsHubPage(PlanovaCore& core, CoreBridge& bridge, QWidget* parent)
    : QWidget(parent), core_(core), list_(new QListWidget(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Continue Your Projects", this));
    layout->addWidget(list_);

    connect(&bridge, &CoreBridge::changed, this, &ProjectsHubPage::refresh);
    refresh();
}

void ProjectsHubPage::refresh()
{
    list_->clear();
    for (const auto& project : core_.projects().list())
        list_->addItem(QString::fromStdString(project.name));
}