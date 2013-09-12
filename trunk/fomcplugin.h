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

#ifndef FOMCPLUGIN_H
#define FOMCPLUGIN_H

#include "plugin.h"

class QTimer;

class ClickableLabel;
class RemoteDate;

class FomcPlugin : public Plugin
{
    Q_OBJECT

public:
    explicit FomcPlugin();
    ~FomcPlugin();

    virtual bool init();

    virtual Type type() const
    {
        return Common;
    }

protected slots:
    virtual void delayedInit();

private slots:
    void slotCheck();
    void slotClicked();

private:
    QTimer *m_timerFomcCheck;
    RemoteDate *m_newYorkDate;
    ClickableLabel *m_label;
};

#endif // FOMCPLUGIN_H