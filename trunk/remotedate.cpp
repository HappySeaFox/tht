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

RemoteDate::RemoteDate(const QString &zone)
    : m_zone(zone),
      m_error(false)
{
    memset(&m_tzInfo, 0, sizeof(TIME_ZONE_INFORMATION));

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
            m_error = true;
            qWarning("Cannot query TZ (%ld)", ret);
        }
        else
        {
            m_tzInfo.Bias = tzi.Bias;
            m_tzInfo.DaylightBias = tzi.DaylightBias;
            m_tzInfo.DaylightDate = tzi.DaylightDate;
            m_tzInfo.StandardBias = tzi.StandardBias;
            m_tzInfo.StandardDate = tzi.StandardDate;
        }

        RegCloseKey(registry);
    }
    else
    {
        m_error = true;
        qWarning("Cannot open registry (%ld)", ret);
    }
}

QDateTime RemoteDate::dateTime()
{
    SYSTEMTIME utc, remote;
    QDateTime dt;

    GetSystemTime(&utc);

    if(!m_error && SystemTimeToTzSpecificLocalTime(&m_tzInfo, &utc, &remote))
    {
        dt.setDate(QDate(remote.wYear, remote.wMonth, remote.wDay));
        dt.setTime(QTime(remote.wHour, remote.wMinute, remote.wSecond, remote.wMilliseconds));
    }
    else
        qWarning("Cannot get the local time for \"%s\" (%ld)", qPrintable(m_zone), m_error ? -1 : GetLastError());

    return dt;
}

TCHAR *RemoteDate::stringToTChar(const QString &s) const
{
#ifdef UNICODE
    static WCHAR *tkey = 0;

    delete tkey;
    tkey = new WCHAR[s.length()+1];

    if(!tkey)
        return 0;

    int l = s.toWCharArray(tkey);
    tkey[l] = '\0';

    return tkey;
#else
    return s.toLocal8Bit().constData();
#endif
}
