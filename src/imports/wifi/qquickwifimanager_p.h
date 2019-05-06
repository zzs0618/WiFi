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

#ifndef QQUICKWIFIMANAGER_P_H
#define QQUICKWIFIMANAGER_P_H

#include <QtQml/qqml.h>
#include <QtQml/qqmlparserstatus.h>
#include <WiFi/wifimanager.h>

class QQuickWiFiManager : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(bool isWiFiEnabled READ isWiFiEnabled WRITE setWiFiEnabled NOTIFY
               isWiFiEnabledChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QString bssid READ bssid NOTIFY bssidChanged)
    Q_PROPERTY(QString ssid READ ssid NOTIFY ssidChanged)
    Q_PROPERTY(qint16 rssi READ rssi NOTIFY rssiChanged)
    Q_PROPERTY(int frequency READ frequency NOTIFY frequencyChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
public:
    explicit QQuickWiFiManager(QObject *parent = nullptr);

    bool isWiFiEnabled() const;
    void setWiFiEnabled(bool enabled);

    QString macAddress() const;
    QString bssid() const;
    QString ssid() const;
    qint16 rssi() const;
    int frequency() const;
    QString ipAddress() const;

    Q_INVOKABLE void addNetwork(const QString &ssid, const QString &password);

    //From QQmlParserStatus
    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

signals:
    void isWiFiEnabledChanged();
    void macAddressChanged();
    void bssidChanged();
    void ssidChanged();
    void rssiChanged();
    void frequencyChanged();
    void ipAddressChanged();

private slots:
    void onConnectionInfoChanged();

private:
    bool m_componentCompleted = false;
    WiFiManager *m_manager = NULL;
    QString m_macAddress;
    QString m_bssid;
    QString m_ssid;
    qint16 m_rssi = -100;
    int m_frequency = 0;
    QString m_ipAddress;
};

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QQuickWiFiManager))

#endif // QQUICKWIFIMANAGER_P_H
