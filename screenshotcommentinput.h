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

    QPixmap pixmap();

private:
    void setColor(const QColor &);

private slots:
    void slotFontDown();
    void slotFontUp();
    void slotChangeColor();
    void slotAlignChanged(bool);

private:
    Ui::ScreenshotCommentInput *ui;
    QColor m_color;
};

#endif // SCREENSHOTCOMMENTINPUT_H
