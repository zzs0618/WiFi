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


#if defined(Q_COMPILER_VARIADIC_MACROS) || defined(Q_MOC_RUN)
class WIFI_EXPORT WiFiTraceLogger
{
    Q_DISABLE_COPY(WiFiTraceLogger)
public:
    Q_DECL_CONSTEXPR WiFiTraceLogger(const char *file, int line, const char *function, const char *category, bool enabled)
        : context(file, line, function, category)
        , isTraceEnabled(enabled)
    {
    }

    void trace() {
        if(isTraceEnabled)
            QMessageLogger(context.file, context.line, context.function, context.category).info("[ START ] %s", context.function);
    }

    ~WiFiTraceLogger() {
        if(isTraceEnabled)
            QMessageLogger(context.file, context.line, context.function, context.category).info("[ END   ] %s", context.function);
    }

private:
    QMessageLogContext context;
    bool isTraceEnabled;
};

#define wifiTrace(category) \
    WiFiTraceLogger wifiTraceLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, category().categoryName(), category().isInfoEnabled()); \
    wifiTraceLogger.trace()

#else // defined(Q_COMPILER_VARIADIC_MACROS) || defined(Q_MOC_RUN)
// check for enabled category inside QMessageLogger.
#define wifiTrace qInfo
#endif // Q_COMPILER_VARIADIC_MACROS || defined(Q_MOC_RUN)


#if defined(QT_NO_INFO_OUTPUT)
#  undef wifiTrace
#  define wifiTrace(category) QT_NO_QDEBUG_MACRO()
#endif

#endif // WIFIGLOBAL_H
