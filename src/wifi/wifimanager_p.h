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

#ifndef WIFIMANAGER_P_H
#define WIFIMANAGER_P_H

#include "wifimanager.h"
#include "wifisupplicantparser_p.h"
#include "wifisupplicanttool_p.h"

#include <private/qobject_p.h>
#include <QtCore/qtimer.h>
#include <QtCore/qmap.h>

class WiFiManagerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(WiFiManager)
public:
    WiFiManagerPrivate();
    ~WiFiManagerPrivate();

    void onSupplicantStarted();
    void onSupplicantFinished();
    void onMessageReceived(const QString &msg);

    void _q_updateInfoTimeout();

    WiFiSupplicantParser parser;
    WiFiSupplicantTool *tool = NULL;
    QTimer *timer_Info = NULL;

    WiFi::State m_state = WiFi::StateDisabled;
    WiFiInfo m_info;
    WiFiScanResultList m_scanResults;
    WiFiNetworkList m_networks;
    QMap<QString, int> m_netIdMapping;
};

#endif // WIFIMANAGER_P_H
