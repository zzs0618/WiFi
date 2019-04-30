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

#include "wifiservice.h"
#include "wifinative.h"
#include "wifinativestub_p.h"

#include "wifidbus_p.h"
#include "station_adaptor.h"
#include "peers_adaptor.h"


WiFiService::WiFiService(QObject *parent) : QThread(parent)
{

}

void WiFiService::run()
{
    WiFiNative *native = new WiFiNative;
    WiFiNativeStub *station = new WiFiNativeStub(native);
    //    WifiDbusPeersStub *peers = new WifiDbusPeersStub(wpa);

    new StationAdaptor(station);
    //    new PeersAdaptor(peers);
    WiFiDBus::connection().registerObject(WiFiDBus::stationPath, station);
    //    connection.registerObject(WiFiDBus::peersPath, peers);
    WiFiDBus::connection().registerService(WiFiDBus::serviceName);

    QThread::exec();
}
