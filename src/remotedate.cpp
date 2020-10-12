/*
 * This file is part of THT-lib.
 *
 * THT-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation.
 *
 * THT-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with THT-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QThreadStorage>
#include <QString>

#include <cstring>

#include "remotedate.h"

namespace
{

struct REG_TZI_FORMAT
{
    LONG Bias;
    LONG StandardBias;
    LONG DaylightBias;
    SYSTEMTIME StandardDate;
    SYSTEMTIME DaylightDate;
};

}

class RemoteDatePrivate
{
public:
    RemoteDatePrivate()
    {
        memset(&tzInfo, 0, sizeof(TIME_ZONE_INFORMATION));
        error = false;
    }

    QString zone;
    TIME_ZONE_INFORMATION tzInfo;
    bool error;
};

/*****************************************/

RemoteDate::RemoteDate(const QString &zone)
{
    d = new RemoteDatePrivate;

    d->zone = zone;

    LONG ret;
    DWORD type;
    HKEY registry;
    REG_TZI_FORMAT tzi;
    DWORD maxLength = sizeof(REG_TZI_FORMAT);

    if((ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                           stringToTChar("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\" + zone),
                           0,
                           KEY_READ,
                           &registry)) == ERROR_SUCCESS)
    {
        ret = RegQueryValueEx(registry, TEXT("TZI"), 0, &type, (LPBYTE)&tzi, &maxLength);

        if(ret != ERROR_SUCCESS || type != REG_BINARY || maxLength != sizeof(REG_TZI_FORMAT))
        {
            d->error = true;
            qWarning("Cannot query TZ (%ld)", ret);
        }
        else
        {
            d->tzInfo.Bias = tzi.Bias;
            d->tzInfo.DaylightBias = tzi.DaylightBias;
            d->tzInfo.DaylightDate = tzi.DaylightDate;
            d->tzInfo.StandardBias = tzi.StandardBias;
            d->tzInfo.StandardDate = tzi.StandardDate;
        }

        RegCloseKey(registry);
    }
    else
    {
        d->error = true;
        qWarning("Cannot open registry (%ld)", ret);
    }
}

RemoteDate::~RemoteDate()
{
    delete d;
}

QDateTime RemoteDate::dateTime()
{
    SYSTEMTIME utc, remote;
    QDateTime dt;

    GetSystemTime(&utc);

    if(!d->error && SystemTimeToTzSpecificLocalTime(&d->tzInfo, &utc, &remote))
    {
        dt.setDate(QDate(remote.wYear, remote.wMonth, remote.wDay));
        dt.setTime(QTime(remote.wHour, remote.wMinute, remote.wSecond, remote.wMilliseconds));
    }
    else
        qWarning("Cannot get the local time for \"%s\" (%ld)", qPrintable(d->zone), d->error ? -1 : GetLastError());

    return dt;
}

// TLS code
typedef QThreadStorage<TCHAR *> TCharStorage;
Q_GLOBAL_STATIC(TCharStorage, tCharStorageTls)

TCHAR *RemoteDate::stringToTChar(const QString &s) const
{
#ifdef UNICODE
    WCHAR *tkey = new WCHAR[s.length()+1];

    if(!tkey)
        return 0;

    const int l = s.toWCharArray(tkey);
    tkey[l] = L'\0';

    tCharStorageTls()->setLocalData(tkey);

    return tkey;
#else
    return s.toLocal8Bit().constData();
#endif
}
