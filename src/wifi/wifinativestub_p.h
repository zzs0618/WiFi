/**
 ** This file is part of the WiFi project.
 ** Copyright 2019 张作深 <zhangzuoshen@hangsheng.com.cn>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef WIFINATIVESTUB_P_H
#define WIFINATIVESTUB_P_H

#include <QtCore/qobject.h>
#include <QtCore/qloggingcategory.h>

#include <WiFi/wifinative.h>


class WiFiNativeStubPrivate;
class WiFiNativeStub : public QObject
{
    Q_OBJECT
public:
    explicit WiFiNativeStub(WiFiNative *native);


public: // PROPERTIES
    Q_PROPERTY(QString ConnectionInfo READ connectionInfo)
    QString connectionInfo() const;

    Q_PROPERTY(bool IsWiFiAutoScan READ isWiFiAutoScan)
    bool isWiFiAutoScan() const;

    Q_PROPERTY(bool IsWiFiEnabled READ isWiFiEnabled)
    bool isWiFiEnabled() const;

    Q_PROPERTY(QString Networks READ networks)
    QString networks() const;

    Q_PROPERTY(QString ScanResults READ scanResults)
    QString scanResults() const;

public Q_SLOTS: // METHODS
    void AddNetwork(const QString &network);
    void RemoveNetwork(const QString &network);
    void SetWiFiAutoScan(bool autoScan);
    void SetWiFiEnabled(bool enabled);
Q_SIGNALS: // SIGNALS
    void ConnectionInfoChanged(const QString &info);
    void ScanResultFound(const QString &bss);
    void ScanResultLost(const QString &bss);
    void ScanResultUpdated(const QString &bss);
    void WifiStateChanged(bool enabled);
    void WiFiAutoScanChanged(bool autoScan);

private:
    Q_DECLARE_PRIVATE(WiFiNativeStub)
};

#endif // WIFINATIVESTUB_P_H
