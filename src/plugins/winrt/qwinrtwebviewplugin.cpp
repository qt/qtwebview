// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "qwinrtwebview_p.h"
#include <QtWebView/private/qwebviewplugin_p.h>

QT_BEGIN_NAMESPACE

class QWinRtWebViewPlugin : public QWebViewPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWebViewPluginInterface_iid FILE "winrt.json")

public:
    QAbstractWebView *create(const QString &key) const override
    {
        return (key == QLatin1String("webview")) ? new QWinRTWebViewPrivate() : nullptr;
    }
};

QT_END_NAMESPACE

#include "qwinrtwebviewplugin.moc"
