#ifndef WIFIGLOBAL_H
#define WIFIGLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/qloggingcategory.h>

Q_DECLARE_LOGGING_CATEGORY(logWiFi)

#ifndef QT_STATIC
    #if defined(QT_BUILD_WIFI_LIB)
        #define WIFI_EXPORT Q_DECL_EXPORT
    #else
        #define WIFI_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define WIFI_EXPORT
#endif

#endif // WIFIGLOBAL_H
