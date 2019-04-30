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

public:
    WiFiNative *m_native = NULL;
};

WiFiNativeStubPrivate::WiFiNativeStubPrivate() : QObjectPrivate()
{
}

WiFiNativeStubPrivate::~WiFiNativeStubPrivate()
{
}

WiFiNativeStub::WiFiNativeStub(WiFiNative *native)
    : QObject(*(new WiFiNativeStubPrivate), native)
{
    Q_D(WiFiNativeStub);
    d->m_native = native;
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
