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

#include "wifinativeproxy_p.h"

#include <private/qobject_p.h>

#include "wifidbus_p.h"
#include "station_interface.h"

class WiFiNativeProxyPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(WiFiNativeProxy)
public:
    WiFiNativeProxyPrivate();
    ~WiFiNativeProxyPrivate();

    void onConnectionInfoChanged(const QString &info);
    void onScanResultFound(const QString &bss);
    void onScanResultLost(const QString &bss);
    void onScanResultUpdated(const QString &bss);
    void onWifiStateChanged(bool enabled);
    void onWiFiAutoScanChanged(bool autoScan);

    void onServiceRegistered(const QString &service);
    void onServiceUnregistered(const QString &service);
    void processEnabled(bool enabled);
    void processServiced(bool serviced);

public:
    wifi::native::Station *m_station = NULL;
    bool m_isServiced = false;

    bool m_isEnabled = false;
    bool m_isAutoScan = false;
    WiFiInfo m_info;
    WiFiScanResultList m_scanResults;
    WiFiNetworkList m_networks;
};

WiFiNativeProxyPrivate::WiFiNativeProxyPrivate() : QObjectPrivate()
{
}

WiFiNativeProxyPrivate::~WiFiNativeProxyPrivate()
{
}

void WiFiNativeProxyPrivate::onConnectionInfoChanged(const QString &info)
{
    Q_Q(WiFiNativeProxy);

    WiFiInfo revInfo = WiFiInfo::fromJson(info.toUtf8());
    if(m_info != revInfo) {
        m_info = revInfo;
        Q_EMIT q->connectionInfoChanged();
    }
}

void WiFiNativeProxyPrivate::onScanResultFound(const QString &bss)
{
    Q_Q(WiFiNativeProxy);

    WiFiScanResult revBSS = WiFiScanResult::fromJson(bss.toUtf8());
    if(!m_scanResults.contains(revBSS)) {
        m_scanResults << revBSS;
        Q_EMIT q->scanResultsChanged();
    }
}

void WiFiNativeProxyPrivate::onScanResultLost(const QString &bss)
{
    Q_Q(WiFiNativeProxy);

    WiFiScanResult revBSS = WiFiScanResult::fromJson(bss.toUtf8());
    if(m_scanResults.contains(revBSS)) {
        m_scanResults.removeAll(revBSS);
        Q_EMIT q->scanResultsChanged();
    }
}

void WiFiNativeProxyPrivate::onScanResultUpdated(const QString &bss)
{
    Q_Q(WiFiNativeProxy);

    WiFiScanResult revBSS = WiFiScanResult::fromJson(bss.toUtf8());
    int index = m_scanResults.indexOf(revBSS);
    if(index >= 0 && index < m_scanResults.length()) {
        m_scanResults.replace(index, revBSS);
        Q_EMIT q->scanResultsChanged();
    }
}

void WiFiNativeProxyPrivate::onWifiStateChanged(bool enabled)
{
    Q_Q(WiFiNativeProxy);

    processEnabled(enabled);
}

void WiFiNativeProxyPrivate::onWiFiAutoScanChanged(bool autoScan)
{
    Q_Q(WiFiNativeProxy);

    if(m_isAutoScan != autoScan) {
        m_isAutoScan = autoScan;
        Q_EMIT q->isWiFiAutoScanChanged();
    }
}


void WiFiNativeProxyPrivate::onServiceRegistered(const QString &service)
{
    qDebug() << Q_FUNC_INFO << service;
    processServiced(true);
}

void WiFiNativeProxyPrivate::onServiceUnregistered(const QString &service)
{
    qDebug() << Q_FUNC_INFO << service;
    processServiced(false);
}

void WiFiNativeProxyPrivate::processEnabled(bool enabled)
{
    Q_Q(WiFiNativeProxy);

    if(m_isEnabled == enabled) {
        return;
    }
    m_isEnabled = enabled;
    Q_EMIT q->isWiFiEnabledChanged();

    if(m_isEnabled) {
        q->initialize();

        m_isAutoScan = m_station->isWiFiAutoScan();
        Q_EMIT q->isWiFiAutoScanChanged();
        m_info = WiFiInfo::fromJson(m_station->connectionInfo().toUtf8());
        Q_EMIT q->connectionInfoChanged();
        m_scanResults = WiFiScanResultList::fromJson(m_station->scanResults().toUtf8());
        Q_EMIT q->scanResultsChanged();
        m_networks = WiFiNetworkList::fromJson(m_station->networks().toUtf8());
        Q_EMIT q->networksChanged();
    }else{
        q->uninitialize();

        m_isAutoScan = false;
        Q_EMIT q->isWiFiAutoScanChanged();
        m_info = WiFiInfo();
        Q_EMIT q->connectionInfoChanged();
        m_scanResults = WiFiScanResultList();
        Q_EMIT q->scanResultsChanged();
        m_networks = WiFiNetworkList();
        Q_EMIT q->networksChanged();
    }
}

void WiFiNativeProxyPrivate::processServiced(bool serviced)
{
    Q_Q(WiFiNativeProxy);

    if(m_isServiced == serviced) {
        return;
    }
    m_isServiced = serviced;
    emit q->isWiFiServicedChanged();

    if(m_isServiced) {
        processEnabled(m_station->isWiFiEnabled());
    } else {
        processEnabled(false);
    }
}

WiFiNativeProxy::WiFiNativeProxy(QObject *parent)
    : QObject(*(new WiFiNativeProxyPrivate), parent)
{
    Q_D(WiFiNativeProxy);

    d->m_station = new wifi::native::Station(WiFiDBus::serviceName,
            WiFiDBus::stationPath, WiFiDBus::connection());

    QObjectPrivate::connect(d->m_station,
                            &WifiNativeStationInterface::WifiStateChanged,
                            d, &WiFiNativeProxyPrivate::onWifiStateChanged);

    QDBusServiceWatcher *wather = new QDBusServiceWatcher(WiFiDBus::serviceName,
            WiFiDBus::connection(), QDBusServiceWatcher::WatchForOwnerChange, this);
    QObjectPrivate::connect(wather, &QDBusServiceWatcher::serviceRegistered, d,
                            &WiFiNativeProxyPrivate::onServiceRegistered);
    QObjectPrivate::connect(wather, &QDBusServiceWatcher::serviceUnregistered, d,
                            &WiFiNativeProxyPrivate::onServiceUnregistered);

    QDBusReply<bool> reply =
                    WiFiDBus::connection().interface()->isServiceRegistered(
                                    WiFiDBus::serviceName);
    d->processServiced(reply.value());
}

void WiFiNativeProxy::initialize()
{
    Q_D(WiFiNativeProxy);

    QObjectPrivate::connect(d->m_station,
                            &WifiNativeStationInterface::ConnectionInfoChanged,
                            d, &WiFiNativeProxyPrivate::onConnectionInfoChanged);
    connect(d->m_station, SIGNAL(NetworkAuthenticated(int)),
            this, SIGNAL(networkAuthenticated(int)));
    connect(d->m_station, SIGNAL(NetworkConnecting(int)),
            this, SIGNAL(networkConnecting(int)));
    connect(d->m_station, SIGNAL(NetworkConnected(int)),
            this, SIGNAL(networkConnected(int)));
    connect(d->m_station, SIGNAL(NetworkErrorOccurred(int)),
            this, SIGNAL(networkErrorOccurred(int)));
    QObjectPrivate::connect(d->m_station,
                            &WifiNativeStationInterface::ScanResultFound,
                            d, &WiFiNativeProxyPrivate::onScanResultFound);
    QObjectPrivate::connect(d->m_station,
                            &WifiNativeStationInterface::ScanResultLost,
                            d, &WiFiNativeProxyPrivate::onScanResultLost);
    QObjectPrivate::connect(d->m_station,
                            &WifiNativeStationInterface::ScanResultUpdated,
                            d, &WiFiNativeProxyPrivate::onScanResultUpdated);
    QObjectPrivate::connect(d->m_station,
                            &WifiNativeStationInterface::WiFiAutoScanChanged,
                            d, &WiFiNativeProxyPrivate::onWiFiAutoScanChanged);
}

void WiFiNativeProxy::uninitialize()
{
    Q_D(WiFiNativeProxy);

    QObjectPrivate::disconnect(d->m_station,
                               &WifiNativeStationInterface::ConnectionInfoChanged,
                               d, &WiFiNativeProxyPrivate::onConnectionInfoChanged);
    disconnect(d->m_station, SIGNAL(NetworkAuthenticated(int)), 0, 0);
    disconnect(d->m_station, SIGNAL(NetworkConnecting(int)), 0, 0);
    disconnect(d->m_station, SIGNAL(NetworkConnected(int)), 0, 0);
    disconnect(d->m_station, SIGNAL(NetworkErrorOccurred(int)), 0, 0);
    QObjectPrivate::disconnect(d->m_station,
                               &WifiNativeStationInterface::ScanResultFound,
                               d, &WiFiNativeProxyPrivate::onScanResultFound);
    QObjectPrivate::disconnect(d->m_station,
                               &WifiNativeStationInterface::ScanResultLost,
                               d, &WiFiNativeProxyPrivate::onScanResultLost);
    QObjectPrivate::disconnect(d->m_station,
                               &WifiNativeStationInterface::ScanResultUpdated,
                               d, &WiFiNativeProxyPrivate::onScanResultUpdated);
    QObjectPrivate::disconnect(d->m_station,
                               &WifiNativeStationInterface::WiFiAutoScanChanged,
                               d, &WiFiNativeProxyPrivate::onWiFiAutoScanChanged);
}

bool WiFiNativeProxy::isWiFiServiced() const
{
    Q_D(const WiFiNativeProxy);
    return d->m_isServiced;
}

bool WiFiNativeProxy::isWiFiEnabled() const
{
    Q_D(const WiFiNativeProxy);
    return d->m_isEnabled;
}

void WiFiNativeProxy::setWiFiEnabled(bool enabled)
{
    Q_D(WiFiNativeProxy);

    if(d->m_isEnabled == enabled) {
        return;
    }

    d->processEnabled(enabled);

    if(d->m_isServiced) {
        d->m_station->SetWiFiEnabled(enabled);
    }
}

bool WiFiNativeProxy::isWiFiAutoScan() const
{
    Q_D(const WiFiNativeProxy);
    return d->m_isAutoScan;
}

void WiFiNativeProxy::setWiFiAutoScan(bool autoScan)
{
    Q_D(WiFiNativeProxy);

    if(d->m_isAutoScan == autoScan) {
        return;
    }

    if(d->m_isServiced) {
        d->m_station->SetWiFiAutoScan(autoScan);
        d->m_isAutoScan = autoScan;
    } else if(d->m_isAutoScan) {
        d->m_isAutoScan = false;
    }
    emit isWiFiAutoScanChanged();
}


WiFiInfo WiFiNativeProxy::connectionInfo() const
{
    Q_D(const WiFiNativeProxy);
    return d->m_info;
}

WiFiScanResultList WiFiNativeProxy::scanResults() const
{
    Q_D(const WiFiNativeProxy);
    return d->m_scanResults;
}

WiFiNetworkList WiFiNativeProxy::networks() const
{
    Q_D(const WiFiNativeProxy);
    return d->m_networks;
}

int WiFiNativeProxy::addNetwork(const WiFiNetwork &network)
{
    Q_D(WiFiNativeProxy);

    if(d->m_isServiced) {
        QDBusPendingReply<int> reply = d->m_station->AddNetwork(QString::fromUtf8(
                network.toJson()));
        reply.waitForFinished();
        return reply.value();
    }
    return -1;
}

void WiFiNativeProxy::selectNetwork(int networkId)
{
    Q_D(WiFiNativeProxy);

    if(d->m_isServiced) {
        d->m_station->SelectNetwork(networkId);
    }
}
void WiFiNativeProxy::removeNetwork(int networkId)
{
    Q_D(WiFiNativeProxy);

    if(d->m_isServiced) {
        d->m_station->RemoveNetwork(networkId);
    }
}
