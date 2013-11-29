/****************************************************************************
**
** Copyright (C) 2012 Jeremy Lainé <jeremy.laine@m4x.org>
** Contact: http://www.qt-project.org/
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDNSLOOKUP_H
#define QDNSLOOKUP_H

#include <QList>
#include <QObject>
#include <QSharedData>
#include <QSharedPointer>
#include <QString>

#include "QXmppGlobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

class QHostAddress;
class QDnsLookupPrivate;
class QDnsDomainNameRecordPrivate;
class QDnsHostAddressRecordPrivate;
class QDnsMailExchangeRecordPrivate;
class QDnsServiceRecordPrivate;
class QDnsTextRecordPrivate;

class QXMPP_EXPORT QDnsDomainNameRecord
{
public:
    QDnsDomainNameRecord();
    QDnsDomainNameRecord(const QDnsDomainNameRecord &other);
    ~QDnsDomainNameRecord();

    QString name() const;
    quint32 timeToLive() const;
    QString value() const;

    QDnsDomainNameRecord &operator=(const QDnsDomainNameRecord &other);

private:
    QSharedDataPointer<QDnsDomainNameRecordPrivate> d;
    friend class QDnsLookupRunnable;
};

class QXMPP_EXPORT QDnsHostAddressRecord
{
public:
    QDnsHostAddressRecord();
    QDnsHostAddressRecord(const QDnsHostAddressRecord &other);
    ~QDnsHostAddressRecord();

    QString name() const;
    quint32 timeToLive() const;
    QHostAddress value() const;

    QDnsHostAddressRecord &operator=(const QDnsHostAddressRecord &other);

private:
    QSharedDataPointer<QDnsHostAddressRecordPrivate> d;
    friend class QDnsLookupRunnable;
};

class QXMPP_EXPORT QDnsMailExchangeRecord
{
public:
    QDnsMailExchangeRecord();
    QDnsMailExchangeRecord(const QDnsMailExchangeRecord &other);
    ~QDnsMailExchangeRecord();

    QString exchange() const;
    QString name() const;
    quint16 preference() const;
    quint32 timeToLive() const;

    QDnsMailExchangeRecord &operator=(const QDnsMailExchangeRecord &other);

private:
    QSharedDataPointer<QDnsMailExchangeRecordPrivate> d;
    friend class QDnsLookupRunnable;
};

class QXMPP_EXPORT QDnsServiceRecord
{
public:
    QDnsServiceRecord();
    QDnsServiceRecord(const QDnsServiceRecord &other);
    ~QDnsServiceRecord();

    QString name() const;
    quint16 port() const;
    quint16 priority() const;
    QString target() const;
    quint32 timeToLive() const;
    quint16 weight() const;

    QDnsServiceRecord &operator=(const QDnsServiceRecord &other);

private:
    QSharedDataPointer<QDnsServiceRecordPrivate> d;
    friend class QDnsLookupRunnable;
};

class QXMPP_EXPORT QDnsTextRecord
{
public:
    QDnsTextRecord();
    QDnsTextRecord(const QDnsTextRecord &other);
    ~QDnsTextRecord();

    QString name() const;
    quint32 timeToLive() const;
    QList<QByteArray> values() const;

    QDnsTextRecord &operator=(const QDnsTextRecord &other);

private:
    QSharedDataPointer<QDnsTextRecordPrivate> d;
    friend class QDnsLookupRunnable;
};

class QXMPP_EXPORT QDnsLookup : public QObject
{
    Q_OBJECT
    Q_ENUMS(Error Type)
    Q_PROPERTY(Error error READ error NOTIFY finished)
    Q_PROPERTY(QString errorString READ errorString NOTIFY finished)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)

public:
    enum Error
    {
        NoError = 0,
        ResolverError,
        OperationCancelledError,
        InvalidRequestError,
        InvalidReplyError,
        ServerFailureError,
        ServerRefusedError,
        NotFoundError
    };

    enum Type
    {
        A = 1,
        AAAA = 28,
        ANY = 255,
        CNAME = 5,
        MX = 15,
        NS = 2,
        PTR = 12,
        SRV = 33,
        TXT = 16
    };

    QDnsLookup(QObject *parent = 0);
    QDnsLookup(Type type, const QString &name, QObject *parent = 0);
    ~QDnsLookup();

    Error error() const;
    QString errorString() const;
    bool isFinished() const;

    QString name() const;
    void setName(const QString &name);

    Type type() const;
    void setType(QDnsLookup::Type);

    QList<QDnsDomainNameRecord> canonicalNameRecords() const;
    QList<QDnsHostAddressRecord> hostAddressRecords() const;
    QList<QDnsMailExchangeRecord> mailExchangeRecords() const;
    QList<QDnsDomainNameRecord> nameServerRecords() const;
    QList<QDnsDomainNameRecord> pointerRecords() const;
    QList<QDnsServiceRecord> serviceRecords() const;
    QList<QDnsTextRecord> textRecords() const;


public Q_SLOTS:
    void abort();
    void lookup();

Q_SIGNALS:
    void finished();
    void nameChanged(const QString &name);
    void typeChanged(Type type);

private:
    QDnsLookupPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QDnsLookup)
    Q_PRIVATE_SLOT(d_func(), void _q_lookupFinished(const QDnsLookupReply &reply))
};

QT_END_NAMESPACE

QT_END_HEADER

#include "qdnslookup_p.h"

#endif // QDNSLOOKUP_H
