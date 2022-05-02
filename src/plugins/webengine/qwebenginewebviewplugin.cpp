/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
**
**
**
******************************************************************************/

#include "qwebenginewebview_p.h"
#include <private/qwebviewplugin_p.h>

QT_BEGIN_NAMESPACE

class QWebEngineWebViewPlugin : public QWebViewPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWebViewPluginInterface_iid FILE "webengine.json")

public:
    QAbstractWebView *create(const QString &key) const override
    {
        return (key == QLatin1String("webview")) ? new QWebEngineWebViewPrivate() : nullptr;
    }

    void prepare() const override
    {
        QtWebEngineQuick::initialize();
    }
};

QT_END_NAMESPACE

#include "qwebenginewebviewplugin.moc"
