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

#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>

#include "qquickwifimanager_p.h"
#include "qquickwifiscanresultmodel_p.h"
#include "qquickwifisortfiltermodel_p.h"


static void initResources()
{
#ifdef QT_STATIC
    Q_INIT_RESOURCE(qmake_WiFi);
#endif
}

class QQuickWiFiPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    QQuickWiFiPlugin(QObject *parent = 0) : QQmlExtensionPlugin(parent)
    {
        initResources();
    }

    void registerTypes(const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("WiFi"));

        qmlRegisterType<QQuickWiFiManager>(uri, 1, 0, "WiFiManager");
        qmlRegisterType<QQuickWiFiScanResultModel>(uri, 1, 0, "WiFiScanResultModel");
        qmlRegisterType<QQuickWiFiSortFilterModel>(uri, 1, 0, "WiFiSortFilterModel");
    }

};

#include "wifiplugin.moc"
