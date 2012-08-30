#ifndef SCREENSHOTEDITOR_H
#define SCREENSHOTEDITOR_H

#include <QDialog>
#include <QPixmap>

class SelectableLabel;

namespace Ui
{
    class ScreenshotEditor;
}

class ScreenshotEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenshotEditor(const QPixmap &px, QWidget *parent = 0);
    ~ScreenshotEditor();

    QPixmap pixmap();

protected:
    virtual void keyPressEvent(QKeyEvent *);

private slots:
    void slotSelected(SelectableLabel *, bool);
    void slotSelectAll();

private:
    Ui::ScreenshotEditor *ui;
};

#endif // SCREENSHOTEDITOR_H
