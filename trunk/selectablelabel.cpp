#include <QMouseEvent>

#include "selectablelabel.h"

SelectableLabel::SelectableLabel(const QPixmap &px, QWidget *parent)
    : QLabel(parent),
      m_selected(true)
{
    setAlignment(Qt::AlignCenter);
    setWordWrap(true);
    setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextEditable);
    setPixmap(px);

    if(!px.isNull())
        setFixedSize(px.size() + QSize(4,4));

    setSelected(false, false);
}

void SelectableLabel::setSelected(bool s, bool loud)
{
    if(s == m_selected)
        return;

    m_selected = s;

    setStyleSheet(QString("QLabel{ border: 2px solid %1; }").arg(m_selected ? "red" : "transparent"));

    if(loud)
        emit selected(m_selected);
}

void SelectableLabel::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    m_wasPress = true;
}

void SelectableLabel::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();

    if(!m_wasPress)
        return;

    setSelected(true);
}
