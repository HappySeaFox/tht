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

#include <QCoreApplication>
#include <QStringList>
#include <QByteArray>
#include <QRegExp>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QDir>

/*
 *  Tool to strip unused context names from .ts files
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // config
    QFile file("drop-context-names.conf");

    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning("Cannot open configuration file %s", qPrintable(file.fileName()));
        return 1;
    }

    QByteArray ba, ba2, ba3;
    QList<QRegExp> todrop;

    while(!(ba = file.readLine(1024)).isEmpty())
    {
        todrop.append(QRegExp("<name>" + ba.trimmed() + "</name>"));
    }

    file.close();

    QStringList files = QDir().entryList(QStringList() << "qt_*.ts", QDir::Files | QDir::Readable);

    // parse TS files
    foreach(QString ts, files)
    {
        QFile fts(ts);
        QFile fts_w(ts+"-2");

        if(!fts.open(QIODevice::ReadWrite))
        {
            qWarning("Cannot open TS file %s", qPrintable(fts.fileName()));
            return 1;
        }

        if(!fts_w.open(QIODevice::ReadWrite))
        {
            qWarning("Cannot open TS file %s", qPrintable(fts.fileName()));
            return 1;
        }

        qDebug("Parsing TS file %s", qPrintable(fts.fileName()));

        while(!(ba = fts.readLine(1024)).isEmpty())
        {
            if(ba.trimmed() == "<context>")
            {
                bool dropContext = false;

                if((ba2 = fts.readLine(1024)).isEmpty())
                {
                    qWarning("Context is not closed in TS file %s", qPrintable(fts.fileName()));
                    return 1;
                }

                ba3 = ba2.trimmed();

                foreach(QRegExp rx, todrop)
                {
                    if(rx.exactMatch(ba3))
                    {
                        dropContext = true;
                        break;
                    }
                }

                if(dropContext)
                {
                    while((ba = fts.readLine(1024)).trimmed() != "</context>")
                    {}
                }
                else
                {
                    fts_w.write(ba);
                    fts_w.write(ba2);
                }
            }
            else
            {
                if(fts_w.write(ba) != ba.length())
                {
                    qWarning("Cannot write %d bytes to file %s", ba.length(), qPrintable(fts_w.fileName()));
                    return 1;
                }
            }
        }

        // overwrite original
        if(!fts.seek(0) || !fts.resize(0))
        {
            qWarning("Cannot truncate file %s", qPrintable(fts.fileName()));
            return 1;
        }

        if(!fts_w.seek(0))
        {
            qWarning("Cannot seek to begin on file %s", qPrintable(fts_w.fileName()));
            return 1;
        }

        while(!(ba = fts_w.readLine(1024)).isEmpty())
        {
            if(fts.write(ba) != ba.length())
            {
                qWarning("Cannot write %d bytes to file %s", ba.length(), qPrintable(fts.fileName()));
                return 1;
            }
        }

        fts_w.remove();
    }

    return 0;
}
