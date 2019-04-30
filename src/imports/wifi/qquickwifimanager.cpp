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

#include "qquickwifimanager_p.h"
#include <WiFi/wifimacaddress.h>

QQuickWiFiManager::QQuickWiFiManager(QObject *parent)
    : QObject(parent)
    , m_manager(new WiFiManager(this))
{
    connect(m_manager, SIGNAL(isWiFiEnabledChanged()),
            SIGNAL(isWiFiEnabledChanged()));
    connect(m_manager, SIGNAL(connectionInfoChanged()),
            SLOT(onConnectionInfoChanged()));
}

bool QQuickWiFiManager::isWiFiEnabled() const
{
    return m_manager->isWiFiEnabled();
}
void QQuickWiFiManager::setWiFiEnabled(bool enabled)
{
    m_manager->setWiFiEnabled(enabled);
}

QString QQuickWiFiManager::macAddress() const
{
    return m_macAddress;
}
QString QQuickWiFiManager::bssid() const
{
    return m_bssid;
}
QString QQuickWiFiManager::ssid() const
{
    return m_ssid;
}
qint16 QQuickWiFiManager::rssi() const
{
    return m_rssi;
}
int QQuickWiFiManager::frequency() const
{
    return m_frequency;
}
QString QQuickWiFiManager::ipAddress() const
{
    return m_ipAddress;
}

void QQuickWiFiManager::addNetwork(const QString &ssid, const QString &password)
{
    WiFiNetwork net(-1, ssid);
    net.setAuthFlags(WiFi::WPA2_PSK);
    net.setPreSharedKey(password);
    m_manager->addNetwork(net);
}

void QQuickWiFiManager::onConnectionInfoChanged()
{
    WiFiInfo info = m_manager->connectionInfo();
    if(WiFiMacAddress(m_macAddress) != info.macAddress()) {
        m_macAddress = info.macAddress().toString();
        emit macAddressChanged();
    }
    if(WiFiMacAddress(m_bssid) != info.bssid()) {
        m_bssid = info.bssid().toString();
        emit bssidChanged();
    }
    if(m_ssid != info.ssid()) {
        m_ssid = info.ssid();
        emit ssidChanged();
    }
    if(m_rssi != info.rssi()) {
        m_rssi = info.rssi();
        emit rssiChanged();
    }
    if(m_frequency != info.frequency()) {
        m_frequency = info.frequency();
        emit frequencyChanged();
    }
    if(m_ipAddress != info.ipAddress()) {
        m_ipAddress = info.ipAddress();
        emit ipAddressChanged();
    }
}
