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

#ifndef WIFISERVICE_H
#define WIFISERVICE_H

#include <WiFi/wifiglobal.h>
#include <QtCore/qthread.h>

class WIFI_EXPORT WiFiService : public QThread
{
    Q_OBJECT
public:
    explicit WiFiService(QObject *parent = nullptr);

public slots:
    virtual void run() Q_DECL_OVERRIDE;
};

#endif // WIFISERVICE_H
