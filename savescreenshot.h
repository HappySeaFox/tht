#ifndef SAVESCREENSHOT_H
#define SAVESCREENSHOT_H

#include <QDialog>

namespace Ui
{
    class SaveScreenshot;
}

class SaveScreenshot : public QDialog
{
    Q_OBJECT
    
public:
    enum SaveScreenshotTo { SaveScreenshotToClipboard, SaveScreenshotToFile };
    explicit SaveScreenshot(QWidget *parent = 0);
    ~SaveScreenshot();

    SaveScreenshotTo destination() const;

    QString fileName() const;

private slots:
    void slotClipboard();
    void slotFile();

private:
    Ui::SaveScreenshot *ui;
    SaveScreenshotTo m_dest;
    QString m_fileName;
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
