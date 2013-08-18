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

#ifndef CONFIGUREROOM_H
#define CONFIGUREROOM_H

#include <QDialog>
#include <QList>

class QTreeWidgetItem;
class QMovie;

class QXmppDataForm;
class QXmppMucItem;
class QXmppMucRoom;

namespace Ui
{
    class ConfigureRoom;
}

class ConfigureRoom : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureRoom(QXmppMucRoom *room, QWidget *parent = 0);
    ~ConfigureRoom();

    QList<QXmppMucItem> permissions() const;

private:
    void stopLoadingMovie();

private slots:
    void slotCurrentTabChanged(int);
    void slotPermissionsReceived(const QList<QXmppMucItem> &list);
    void slotConfigurationReceived(const QXmppDataForm &);
    void slotAddJid();
    void slotRemoveJid();
    void slotCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::ConfigureRoom *ui;
    QXmppMucRoom *m_room;
    QMovie *m_loadingMovie;
};

#endif // CONFIGUREROOM_H
