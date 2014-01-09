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

#include <QXmlParseException>
#include <QXmlDefaultHandler>
#include <QXmlSimpleReader>
#include <QXmlInputSource>

#include "stylereader.h"

/****************************************/

class StyleReaderHandler : public QXmlDefaultHandler
{
public:
    virtual bool error(const QXmlParseException &exception)
    {
        m_error = exceptionToString(exception);
        return false;
    }

    virtual QString errorString() const
    {
        return m_error;
    }

    virtual bool fatalError(const QXmlParseException &exception)
    {
        m_error = exceptionToString(exception);
        return false;
    }

    virtual bool warning(const QXmlParseException &exception)
    {
        qDebug("Style parsing warning: %s", qPrintable(exceptionToString(exception)));
        return true;
    }

    virtual bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
    {
        Q_UNUSED(namespaceURI)
        Q_UNUSED(qName)

        if(localName == "style")
        {
            m_version = atts.value("version");
            m_name = atts.value("name");
            m_previewColor = QColor(atts.value("preview-color"));
        }

        return true;
    }

    virtual bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
    {
        Q_UNUSED(namespaceURI)
        Q_UNUSED(qName)

        if(localName == "style")
            m_css = m_characters;

        m_characters.clear();

        return true;
    }

    virtual bool characters(const QString &ch)
    {
        m_characters += ch;
        return true;
    }

    QString error() const
    {
        return m_error;
    }

    QColor previewColor() const
    {
        return m_previewColor;
    }

    QString name()
    {
        return m_name;
    }

    QString css()
    {
        return m_css;
    }

    void resetError()
    {
        m_error.clear();
    }

private:
    QString exceptionToString(const QXmlParseException &exception) const;

private:
    QString m_error;
    QColor m_previewColor;
    QString m_name, m_css;
    QString m_characters;
    QString m_version;
};

QString StyleReaderHandler::exceptionToString(const QXmlParseException &exception) const
{
    return exception.message()
            + " ("
            + QString::number(exception.lineNumber())
            + ','
            + QString::number(exception.columnNumber())
            + ')';
}


/****************************************/

StyleReader::StyleReader() :
    QXmlSimpleReader()
{
    m_handler = new StyleReaderHandler;

    setContentHandler(m_handler);
    setErrorHandler(m_handler);
}

StyleReader::~StyleReader()
{
    delete m_handler;
}

bool StyleReader::parse(const QString &filePath)
{
    m_handler->resetError();

    QFile source(filePath);

    if(!source.open(QIODevice::ReadOnly | QIODevice::Text))
        qWarning("Cannot open style, error: %s", qPrintable(source.errorString()));

    return QXmlSimpleReader::parse(&source);
}

QString StyleReader::error() const
{
    return m_handler->errorString();
}

QColor StyleReader::previewColor() const
{
    return m_handler->previewColor();
}

QString StyleReader::name()
{
    return m_handler->name();
}

QString StyleReader::css()
{
    return m_handler->css();
}
