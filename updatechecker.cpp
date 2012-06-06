#include <QStringList>
#include <QRegExp>
#include <QTimer>

#include "networkaccess.h"
#include "updatechecker.h"

static const char * const THT_UPDATE_URL = "http://traders-home-task-ng.googlecode.com/svn/trunk/THT-version.tag";

UpdateChecker *UpdateChecker::instance()
{
    static UpdateChecker *m_inst = new UpdateChecker;

    return m_inst;
}

UpdateChecker::UpdateChecker() : QObject()
{
    m_lastVersion = NVER_STRING;

    m_url = QUrl(THT_UPDATE_URL);

    m_rxVersion = QRegExp("^(\\d+)\\.(\\d+)\\.(\\d+)$");

    m_net = new NetworkAccess(this);

    connect(m_net, SIGNAL(finished()), this, SLOT(slotFinished()));

    startRequest();
}

void UpdateChecker::startRequest()
{
    m_net->get(m_url);
}

void UpdateChecker::slotFinished()
{
    if(m_net->error() != QNetworkReply::NoError)
        return;

    QStringList list = QString(m_net->data()).split(QRegExp("\\r?\\n"), QString::SkipEmptyParts);

    if(list.isEmpty() || !m_rxVersion.exactMatch(list[0]))
    {
        qWarning("Update checker: answer is broken");
        return;
    }

    if(m_lastVersion != list[0])
    {
        m_lastVersion = list[0];

        bool okmajor, okminor, okpatch;

        int major = m_rxVersion.cap(1).toInt(&okmajor);
        int minor = m_rxVersion.cap(2).toInt(&okminor);
        int patch = m_rxVersion.cap(3).toInt(&okpatch);

        if(okmajor && okminor && okpatch)
        {
            if(major > NVER1
                    || (major == NVER1 && minor > NVER2)
                    || (major == NVER1 && minor == NVER2 && patch > NVER3)
                    )
            {
                qDebug("Update checker: new version is \"%s\"", qPrintable(m_lastVersion));
                emit newVersion(m_lastVersion);
            }
            else
                qDebug("Update checker: current version is better than \"%s\"", qPrintable(m_lastVersion));
        }
        else
        {
            qDebug("Update checker: new version is \"%s\"", qPrintable(m_lastVersion));
            emit newVersion(m_lastVersion);
        }
    }
    else
        qDebug("Update checker: version is up-to-date");

    // check every 4 hours
    QTimer::singleShot(4*3600*1000, this, SLOT(startRequest()));
}
