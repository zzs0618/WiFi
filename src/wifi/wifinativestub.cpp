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

#include "wifinativestub_p.h"

#include <private/qobject_p.h>

#include "station_adaptor.h"

class WiFiNativeStubPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(WiFiNativeStub)
public:
    WiFiNativeStubPrivate();
    ~WiFiNativeStubPrivate();

    void onConnectionInfoChanged();
    void onScanResultFound(const WiFiScanResult &result);
    void onScanResultUpdated(const WiFiScanResult &result);
    void onScanResultLost(const WiFiScanResult &result);

public:
    WiFiNative *m_native = NULL;
};

WiFiNativeStubPrivate::WiFiNativeStubPrivate() : QObjectPrivate()
{
}

WiFiNativeStubPrivate::~WiFiNativeStubPrivate()
{
}

void WiFiNativeStubPrivate::onConnectionInfoChanged()
{
    Q_Q(WiFiNativeStub);

    const QByteArray &json = m_native->connectionInfo().toJson();
    Q_EMIT q->ConnectionInfoChanged(QString::fromUtf8(json));
}

void WiFiNativeStubPrivate::onScanResultFound(const WiFiScanResult &result)
{
    Q_Q(WiFiNativeStub);
    const QByteArray &json = result.toJson();
    Q_EMIT q->ScanResultFound(QString::fromUtf8(json));
}
void WiFiNativeStubPrivate::onScanResultUpdated(const WiFiScanResult &result)
{
    Q_Q(WiFiNativeStub);
    const QByteArray &json = result.toJson();
    Q_EMIT q->ScanResultUpdated(QString::fromUtf8(json));
}
void WiFiNativeStubPrivate::onScanResultLost(const WiFiScanResult &result)
{
    Q_Q(WiFiNativeStub);
    const QByteArray &json = result.toJson();
    Q_EMIT q->ScanResultLost(QString::fromUtf8(json));
}

WiFiNativeStub::WiFiNativeStub(WiFiNative *native)
    : QObject(*(new WiFiNativeStubPrivate), native)
{
    Q_D(WiFiNativeStub);
    d->m_native = native;

    QObjectPrivate::connect(d->m_native, &WiFiNative::connectionInfoChanged,
                            d, &WiFiNativeStubPrivate::onConnectionInfoChanged);
    QObjectPrivate::connect(d->m_native, &WiFiNative::scanResultFound,
                            d, &WiFiNativeStubPrivate::onScanResultFound);
    QObjectPrivate::connect(d->m_native, &WiFiNative::scanResultUpdated,
                            d, &WiFiNativeStubPrivate::onScanResultUpdated);
    QObjectPrivate::connect(d->m_native, &WiFiNative::scanResultLost,
                            d, &WiFiNativeStubPrivate::onScanResultLost);
}

QString WiFiNativeStub::connectionInfo() const
{
    Q_D(const WiFiNativeStub);
    const QByteArray &json = d->m_native->connectionInfo().toJson();
    return QString::fromUtf8(json);
}

bool WiFiNativeStub::isWiFiEnabled() const
{
    Q_D(const WiFiNativeStub);
    return d->m_native->isWiFiEnabled();
}

QString WiFiNativeStub::networks() const
{
    Q_D(const WiFiNativeStub);
    const QByteArray &json = d->m_native->networks().toJson();
    return QString::fromUtf8(json);
}

QString WiFiNativeStub::scanResults() const
{
    Q_D(const WiFiNativeStub);
    const QByteArray &json = d->m_native->scanResults().toJson();
    return QString::fromUtf8(json);
}

void WiFiNativeStub::AddNetwork(const QString &network)
{
    Q_D(WiFiNativeStub);
    WiFiNetwork net = WiFiNetwork::fromJson(network.toUtf8());
    d->m_native->addNetwork(net);
}

void WiFiNativeStub::RemoveNetwork(const QString &network)
{
    Q_UNUSED(network);
}

void WiFiNativeStub::setWiFiEnabled(bool enabled)
{
    Q_D(WiFiNativeStub);
    d->m_native->setWiFiEnabled(enabled);
}
