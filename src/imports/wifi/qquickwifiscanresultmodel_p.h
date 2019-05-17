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

#ifndef QQUICKWIFISCANRESULTMODEL_P_H
#define QQUICKWIFISCANRESULTMODEL_P_H

#include <QtQml/qqml.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtQml/qqmlparserstatus.h>
#include <WiFi/wifimanager.h>

class QQuickWiFiScanResultModel : public QAbstractListModel,
    public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit QQuickWiFiScanResultModel(QObject *parent = nullptr);

    //From QAbstractListModel
    int rowCount(const QModelIndex & = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    void classBegin();
    void componentComplete();

private slots:
    void onNetworkConnecting(int networkId);
    void onNetworkAuthenticated(int networkId);
    void onNetworkConnected(int networkId);
    void onNetworkErrorOccurred(int networkId);

private:
    WiFiManager *m_manager = NULL;
    WiFiScanResultList m_scanResults;
    bool m_complete;
    QPair<int,int> m_status;
};

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QQuickWiFiScanResultModel))

#endif // QQUICKWIFISCANRESULTMODEL_P_H
