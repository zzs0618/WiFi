#ifndef QTWIFIGLOBAL_P_H
#define QTWIFIGLOBAL_P_H

#ifndef QT_STATIC
    #if defined(QT_BUILD_WIFI_LIB)
        #define Q_WIFI_PRIVATE_EXPORT Q_DECL_EXPORT
    #else
        #define Q_WIFI_PRIVATE_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define Q_WIFI_PRIVATE_EXPORT
#endif


#endif // QTWIFIGLOBAL_P_H
