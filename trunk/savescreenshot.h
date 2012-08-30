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

#ifndef SAVESCREENSHOT_H
#define SAVESCREENSHOT_H

#include <QPixmap>
#include <QDialog>
#include <QList>

class ScreenshotEditor;

namespace Ui
{
    class SaveScreenshot;
}

class SaveScreenshot : public QDialog
{
    Q_OBJECT

public:
    enum SaveScreenshotTo { SaveScreenshotToClipboard, SaveScreenshotToFile };
    explicit SaveScreenshot(const QPixmap &px, QWidget *parent = 0);
    ~SaveScreenshot();

    SaveScreenshotTo destination() const;

    QString fileName() const;

    QPixmap pixmap() const;

private slots:
    void slotClipboard();
    void slotFile();
    void slotEdit();

private:
    Ui::SaveScreenshot *ui;
    SaveScreenshotTo m_dest;
    QString m_fileName;
    ScreenshotEditor *m_editor;
};

inline
SaveScreenshot::SaveScreenshotTo SaveScreenshot::destination() const
{
    return m_dest;
}

inline
QString SaveScreenshot::fileName() const
{
    return m_fileName;
}

#endif // SAVESCREENSHOT_H
