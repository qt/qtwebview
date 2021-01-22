/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebView module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>

#include <QtWebView/private/qquickwebviewloadrequest_p.h>
#include <QtWebView/private/qquickwebview_p.h>

QT_BEGIN_NAMESPACE

class QWebViewModule : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    QWebViewModule(QObject *parent = 0) : QQmlExtensionPlugin(parent) { }
    void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtWebView"));

        // @uri QtWebView
        const QString &msg = QObject::tr("Cannot create separate instance of WebViewLoadRequest");
        qmlRegisterType<QQuickWebView>(uri, 1, 0, "WebView");
        qmlRegisterType<QQuickWebView, 1>(uri, 1, 1, "WebView");
        qmlRegisterType<QQuickWebView, 14>(uri, 1, 14, "WebView");
        qmlRegisterUncreatableType<QQuickWebViewLoadRequest>(uri, 1, 1, "WebViewLoadRequest", msg);

        // Make sure we're always available under version x.QT_VERSION_MINOR
        qmlRegisterModule(uri, 1, QT_VERSION_MINOR);
    }

    void initializeEngine(QQmlEngine *engine, const char *uri)
    {
        Q_UNUSED(uri);
        Q_UNUSED(engine);
    }
};

QT_END_NAMESPACE

#include "webview.moc"



