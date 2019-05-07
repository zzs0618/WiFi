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

#include "qquickwifiscanresultmodel_p.h"

QQuickWiFiScanResultModel::QQuickWiFiScanResultModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_manager(new WiFiManager(this))
    , m_complete(false)
{
    connect(m_manager, &WiFiManager::scanResultsChanged, [this]() {
        this->beginResetModel();
        this->endResetModel();
    });
}

int QQuickWiFiScanResultModel::rowCount(const QModelIndex &) const
{
    return m_manager->scanResults().count();
}

QVariant QQuickWiFiScanResultModel::data(const QModelIndex &index,
        int role) const
{
    if (!index.isValid() || index.row() < 0) {
        return QVariant();
    }

    if (index.row() >= m_manager->scanResults().count()) {
        qWarning() << "WifiAccessPointModel: Index out of bound";
        return QVariant();
    }

    QVariantMap scanResult = m_manager->scanResults().value(index.row()).toMap();
    QVariant value;
    switch (role) {
        case Qt::DisplayRole + 7:
            value = WiFiManager::CalculateSignalLevel(scanResult["rssi"].toInt(), 4);
            break;
        default:
            value = scanResult.value(roleNames().value(role));
            break;
    }
    return value;
}

QHash<int, QByteArray> QQuickWiFiScanResultModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {Qt::DisplayRole + 1, "ssid"},
        {Qt::DisplayRole + 2, "bssid"},
        {Qt::DisplayRole + 3, "rssi"},
        {Qt::DisplayRole + 4, "frequency"},
        {Qt::DisplayRole + 5, "flags"},
        {Qt::DisplayRole + 6, "networkId"},
        {Qt::DisplayRole + 7, "signalLevel"}
    };

    return roles;
}

void QQuickWiFiScanResultModel::classBegin()
{
}

void QQuickWiFiScanResultModel::componentComplete()
{
    m_complete = true;
}
