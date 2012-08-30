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

    void restoreLabels();
    void saveLabels();
    void clearLabels();

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual bool event(QEvent *e);

private slots:
    void slotSelected(SelectableLabel *, bool);

private:
    Ui::ScreenshotEditor *ui;
};

#endif // SCREENSHOTEDITOR_H
