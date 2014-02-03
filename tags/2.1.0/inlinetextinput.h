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

#ifndef INLINETEXTINPUT_H
#define INLINETEXTINPUT_H

#include <QWidget>

namespace Ui
{
    class InlineTextInput;
}

class InlineTextInput : public QWidget
{
    Q_OBJECT

public:
    explicit InlineTextInput(QWidget *parent = 0);
    ~InlineTextInput();

    void startEditing(const QString &t = QString(), bool startEditing = false);
    void stopEditing();

    QString text() const;

    void setUseTickerValidator(bool);

    void setMaximumLength(int);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *);

signals:
    void textChanged(const QString &);
    void next();
    void cancel();
    void accept();

private:
    Ui::InlineTextInput *ui;
    bool m_active;
};

inline
void InlineTextInput::stopEditing()
{
    m_active = false;
}

#endif // INLINETEXTINPUT_H
