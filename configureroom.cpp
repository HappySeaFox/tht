/*
 * This file is part of THT.
 *
 * THT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * THT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with THT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QGridLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QTextEdit>
#include <QDebug>
#include <QMovie>
#include <Qt>

#include "QXmppMucManager.h"
#include "QXmppDataForm.h"

#include "configureroom.h"
#include "ui_configureroom.h"

typedef QPair<QString, QString> QStringPair;

ConfigureRoom::ConfigureRoom(QXmppMucRoom *room, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureRoom),
    m_room(room)
{
    ui->setupUi(this);

    m_loadingMovie = new QMovie(":/images/wait.gif", "GIF", this);
    ui->labelLoading->setMovie(m_loadingMovie);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->treeAffiliations->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->treeAffiliations->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    ui->treeAffiliations->topLevelItem(0)->setData(0, Qt::UserRole, QXmppMucItem::OwnerAffiliation);
    ui->treeAffiliations->topLevelItem(1)->setData(0, Qt::UserRole, QXmppMucItem::AdminAffiliation);
    ui->treeAffiliations->topLevelItem(2)->setData(0, Qt::UserRole, QXmppMucItem::MemberAffiliation);
    ui->treeAffiliations->topLevelItem(3)->setData(0, Qt::UserRole, QXmppMucItem::OutcastAffiliation);
    ui->treeAffiliations->topLevelItem(4)->setData(0, Qt::UserRole, QXmppMucItem::NoAffiliation);

    connect(m_room, SIGNAL(permissionsReceived(QList<QXmppMucItem>)),
            this, SLOT(slotPermissionsReceived(QList<QXmppMucItem>)));
    connect(m_room, SIGNAL(configurationReceived(QXmppDataForm)),
            this, SLOT(slotConfigurationReceived(QXmppDataForm)));

    connect(ui->tabsConfiguration, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)));
    slotCurrentTabChanged(0);
}

ConfigureRoom::~ConfigureRoom()
{
    delete ui;
}

QList<QXmppMucItem> ConfigureRoom::permissions() const
{
    QList<QXmppMucItem> list;
    int index = 0;
    QTreeWidgetItem *item;

    while((item = ui->treeAffiliations->topLevelItem(index++)))
    {
        QTreeWidgetItem *childItem;
        int childIndex = 0;
        QXmppMucItem::Affiliation aff = static_cast<QXmppMucItem::Affiliation>(item->data(0, Qt::UserRole).toInt());

        while((childItem = item->child(childIndex++)))
        {
            QXmppMucItem mi;
            mi.setJid(childItem->text(0));
            mi.setAffiliation(aff);
            list.append(mi);
        }
    }

    return list;
}

void ConfigureRoom::stopLoadingMovie()
{
    ui->labelLoading->hide();
    m_loadingMovie->stop();
}

QLineEdit *ConfigureRoom::addLineEdit(QBoxLayout *layout, const QString &label, const QString &value) const
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(new QLabel(label, layout->parentWidget()));
    QLineEdit *line = new QLineEdit(layout->parentWidget());
    line->setText(value);
    hlayout->addWidget(line);
    hlayout->addStretch(1);
    layout->addLayout(hlayout);

    return line;
}

QTextEdit *ConfigureRoom::addTextEdit(QBoxLayout *layout, const QString &label, const QString &value) const
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(new QLabel(label, layout->parentWidget()));
    QTextEdit *edit = new QTextEdit(layout->parentWidget());
    edit->setText(value);
    hlayout->addWidget(edit);
    hlayout->addStretch(1);
    layout->addLayout(hlayout);

    return edit;
}

void ConfigureRoom::slotCurrentTabChanged(int index)
{
    if(!index)
        qDebug("Permissions request sent: %s", m_room->requestPermissions() ? "yes" : "no");
    else
        qDebug("Configuration request sent: %s", m_room->requestConfiguration() ? "yes" : "no");

    ui->labelLoading->show();
    m_loadingMovie->start();
}

void ConfigureRoom::slotPermissionsReceived(const QList<QXmppMucItem> &list)
{
    // clear old items
    int index = 0;
    QTreeWidgetItem *item;

    while((item = ui->treeAffiliations->topLevelItem(index++)))
    {
        QTreeWidgetItem *childItem;

        while((childItem = item->takeChild(0)))
            delete childItem;
    }

    foreach(QXmppMucItem i, list)
    {
        switch(i.affiliation())
        {
            case QXmppMucItem::OwnerAffiliation: item = ui->treeAffiliations->topLevelItem(0); break;
            case QXmppMucItem::AdminAffiliation: item = ui->treeAffiliations->topLevelItem(1); break;
            case QXmppMucItem::MemberAffiliation: item = ui->treeAffiliations->topLevelItem(2); break;
            case QXmppMucItem::OutcastAffiliation: item = ui->treeAffiliations->topLevelItem(3); break;

            default:
                item = ui->treeAffiliations->topLevelItem(4);
            break;
        }

        new QTreeWidgetItem(item, QStringList() << i.jid() << i.reason());
    }

    index = 0;

    while((item = ui->treeAffiliations->topLevelItem(index++)))
    {
        item->sortChildren(0, Qt::AscendingOrder);
        item->setExpanded(true);
    }

    stopLoadingMovie();
}

void ConfigureRoom::slotConfigurationReceived(const QXmppDataForm &df)
{
    // recreate widget
    QWidget *generalWidget = new QWidget;
    QVBoxLayout *vlayout = new QVBoxLayout;
    generalWidget->setLayout(vlayout);
    ui->scrollArea->setWidget(generalWidget);
    generalWidget->show();

    QList<QXmppDataForm::Field> fields = df.fields();

    foreach(QXmppDataForm::Field f, fields)
    {
        switch(f.type())
        {
            case QXmppDataForm::Field::BooleanField:
            {
                QCheckBox *box = new QCheckBox(f.label(), generalWidget);
                box->setChecked(f.value().toBool());
                vlayout->addWidget(box);
            }
            break;
            case QXmppDataForm::Field::FixedField:
            {
                QLabel *label = new QLabel(f.value().toString(), generalWidget);
                vlayout->addWidget(label);
            }
            break;
            case QXmppDataForm::Field::HiddenField:
            break;
            case QXmppDataForm::Field::JidMultiField:
                addTextEdit(vlayout, f.label(), f.value().toString());
            break;
            case QXmppDataForm::Field::JidSingleField:
                addLineEdit(vlayout, f.label(), f.value().toString());
            break;
            case QXmppDataForm::Field::ListMultiField:
            {
                QHBoxLayout *hlayout = new QHBoxLayout;
                hlayout->addWidget(new QLabel(f.label(), generalWidget));
                QListWidget *list = new QListWidget(generalWidget);
                list->setSelectionMode(QAbstractItemView::ExtendedSelection);
                QList<QStringPair> options = f.options();
                QStringList values = f.value().toStringList();

                foreach(QStringPair pair, options)
                {
                    QListWidgetItem *i = new QListWidgetItem(pair.first, list);
                    i->setData(Qt::UserRole, pair.second);

                    if(values.indexOf(pair.second) >= 0)
                        i->setSelected(true);
                }

                hlayout->addWidget(list);
                hlayout->addStretch(1);
                vlayout->addLayout(hlayout);
            }
            break;
            case QXmppDataForm::Field::ListSingleField:
            {
                QHBoxLayout *hlayout = new QHBoxLayout;
                hlayout->addWidget(new QLabel(f.label(), generalWidget));
                QComboBox *combo = new QComboBox(generalWidget);
                QList<QStringPair> options = f.options();

                foreach(QStringPair pair, options)
                {
                    combo->addItem(pair.first, pair.second);
                }

                combo->setCurrentIndex(combo->findData(f.value().toString()));
                hlayout->addWidget(combo);
                hlayout->addStretch(1);
                vlayout->addLayout(hlayout);
            }
            break;
            case QXmppDataForm::Field::TextMultiField:
                addTextEdit(vlayout, f.label(), f.value().toString());
            break;
            case QXmppDataForm::Field::TextPrivateField:
            {
                QLineEdit *line = addLineEdit(vlayout, f.label(), f.value().toString());
                line->setEchoMode(QLineEdit::Password);
            }
            break;
            case QXmppDataForm::Field::TextSingleField:
                addLineEdit(vlayout, f.label(), f.value().toString());
            break;
        }
    }

    stopLoadingMovie();
}

void ConfigureRoom::slotAddJid()
{
    qDebug("Adding JID");

    QTreeWidgetItem *item = ui->treeAffiliations->currentItem();

    if(!item)
        return;

    if(item->parent())
        item = item->parent();

    bool ok;
    QString jid = QInputDialog::getText(this, tr("Add affiliation"), tr("JID:"), QLineEdit::Normal, QString(), &ok);

    if(!ok)
        return;

    QList<QTreeWidgetItem *> items = ui->treeAffiliations->findItems(jid,  Qt::MatchFixedString | Qt::MatchRecursive);

    foreach(QTreeWidgetItem *i, items)
    {
        if(i->parent() == item)
        {
            QMessageBox::information(this, tr("Adding JID"), tr("JID \"%1\" is already in this section").arg(jid));
            return;
        }
    }

    new QTreeWidgetItem(item, QStringList() << jid);
}

void ConfigureRoom::slotRemoveJid()
{
    QTreeWidgetItem *item = ui->treeAffiliations->currentItem();

    if(!item)
        return;

    if(item->parent())
    {
        qDebug("Removing JID \"%s\"", qPrintable(item->text(0)));
        delete item;
    }
}

void ConfigureRoom::slotCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)

    if(!current || !current->parent())
        ui->pushRemoveJid->setEnabled(false);
    else
        ui->pushRemoveJid->setEnabled(true);
}
