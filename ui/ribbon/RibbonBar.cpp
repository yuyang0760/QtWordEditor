#include "RibbonBar.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QDebug>

namespace QtWordEditor {

class RibbonBarPrivate
{
public:
    // Currently active tab index
    int currentTab = -1;
    // Map from tab index to list of group names (for debugging)
    QHash<int, QStringList> tabGroups;
};

RibbonBar::RibbonBar(QWidget *parent)
    : QTabWidget(parent)
    , d(new RibbonBarPrivate)
{
    setDocumentMode(true);
    setTabPosition(QTabWidget::North);
    // Create a default "Home" tab
    addTab(tr("Home"));
}

RibbonBar::~RibbonBar()
{
}

int RibbonBar::addTab(const QString &title)
{
    // Create a container widget for the tab
    QWidget *tabWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(tabWidget);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(4);
    tabWidget->setLayout(layout);

    int index = QTabWidget::addTab(tabWidget, title);
    d->currentTab = index;
    return index;
}

void RibbonBar::addGroup(const QString &groupName)
{
    if (d->currentTab < 0 || d->currentTab >= count())
        return;

    QWidget *tabWidget = widget(d->currentTab);
    if (!tabWidget || !tabWidget->layout())
        return;

    QVBoxLayout *tabLayout = qobject_cast<QVBoxLayout*>(tabWidget->layout());
    if (!tabLayout)
        return;

    // Create a group container (horizontal layout with label)
    QWidget *groupWidget = new QWidget(tabWidget);
    QHBoxLayout *groupLayout = new QHBoxLayout(groupWidget);
    groupLayout->setContentsMargins(6, 6, 6, 6);
    groupLayout->setSpacing(8);

    QLabel *label = new QLabel(groupName, groupWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-weight: bold; color: #666;");
    groupLayout->addWidget(label);

    // Add a stretch to push label to left
    groupLayout->addStretch();

    tabLayout->addWidget(groupWidget);

    // Store group name for debugging
    d->tabGroups[d->currentTab].append(groupName);
}

void RibbonBar::addAction(QAction *action)
{
    if (d->currentTab < 0 || d->currentTab >= count())
        return;

    QWidget *tabWidget = widget(d->currentTab);
    if (!tabWidget)
        return;

    // Find the last group widget (simplified: assume last child is a group)
    QLayout *tabLayout = tabWidget->layout();
    if (!tabLayout || tabLayout->count() == 0)
        return;

    QWidget *lastGroup = tabLayout->itemAt(tabLayout->count() - 1)->widget();
    if (!lastGroup)
        return;

    QLayout *groupLayout = lastGroup->layout();
    if (!groupLayout)
        return;

    // Create a tool button for the action
    QToolButton *button = new QToolButton(lastGroup);
    button->setDefaultAction(action);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    groupLayout->addWidget(button);
}

void RibbonBar::updateFromSelection()
{
    // Placeholder: update button states based on current selection
    qDebug() << "RibbonBar::updateFromSelection() placeholder";
}

} // namespace QtWordEditor