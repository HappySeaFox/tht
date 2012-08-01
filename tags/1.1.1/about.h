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

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

class UpdateChecker;

namespace Ui {
class About;
}

class QTimer;

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);
    ~About();

private slots:
    void slotNewVersion(const QString &);
    void slotError(const QString &);
    void slotExtendedAbout();
    void slotSetWaitIcon();

private:
    Ui::About *ui;
    bool m_showExt;
    UpdateChecker *m_checker;
    QTimer *m_timer;
};

#endif // ABOUT_H
