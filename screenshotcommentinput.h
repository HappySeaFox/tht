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

#ifndef SCREENSHOTCOMMENTINPUT_H
#define SCREENSHOTCOMMENTINPUT_H

#include <QDialog>
#include <QPixmap>
#include <QColor>

namespace Ui
{
    class ScreenshotCommentInput;
}

class ScreenshotCommentInput : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenshotCommentInput(QWidget *parent = 0);
    ~ScreenshotCommentInput();

    /*
     *  Text rendered to pixmap
     */
    QPixmap pixmap() const;

    void saveSettings() const;

private:
    void setColor(const QColor &);
    void setBackgroundColor(const QColor &);

private slots:
    void slotFontDown();
    void slotFontUp();
    void slotChangeColor();
    void slotChangeBackgroundColor();
    void slotUseBackgroundColor(bool);
    void slotAlignChanged(bool);

private:
    Ui::ScreenshotCommentInput *ui;
    QColor m_color;
    QColor m_backgroundColor, m_originalBackgroundColor;
    Qt::AlignmentFlag m_align;
    QString m_textCss, m_backgroundCss;
};

#endif // SCREENSHOTCOMMENTINPUT_H
