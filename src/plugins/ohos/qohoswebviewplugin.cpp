// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohoswebview_p.h"
#include <QtCore/private/qohoslogger_p.h>
#include <QtWebView/private/qwebviewplugin_p.h>

QT_BEGIN_NAMESPACE

class QOhosWebViewPlugin : public QWebViewPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWebViewPluginInterface_iid FILE "ohos.json")

public:
    QOhosWebViewPlugin(QObject *parent = nullptr)
        : QWebViewPlugin(parent)
    {
    }

    QWebViewPrivate *create(const QString &key, QWebView *view,
                            QWebViewFactory::Hint hint) const override
    {
        Q_UNUSED(hint);
        qOhosDebug(QtForOhos) << Q_FUNC_INFO << key;
        return (key == QLatin1String("webview")) ? makeOhosWebView(view) : nullptr;
    }
};

QT_END_NAMESPACE

#include "qohoswebviewplugin.moc"
