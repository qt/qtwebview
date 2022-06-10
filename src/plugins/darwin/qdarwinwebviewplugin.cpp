// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdarwinwebview_p.h"
#include <private/qwebviewplugin_p.h>
#include <QtCore/qbytearray.h>

QT_BEGIN_NAMESPACE

class QDarwinWebViewPlugin : public QWebViewPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWebViewPluginInterface_iid FILE "darwin.json")

public:
    QAbstractWebView *create(const QString &key) const override
    {
        return (key == QLatin1String("webview")) ? new QDarwinWebViewPrivate() : nullptr;
    }

    void prepare() const override
    {
#ifdef Q_OS_MACOS
        // On macOS, correct WebView / QtQuick compositing and stacking requires running
        // Qt in layer-backed mode, which again resuires rendering on the Gui thread.
        qWarning("Setting QT_MAC_WANTS_LAYER=1 and QSG_RENDER_LOOP=basic");
        qputenv("QT_MAC_WANTS_LAYER", "1");
        qputenv("QSG_RENDER_LOOP", "basic");
#endif // Q_OS_MACOS
    }
};

QT_END_NAMESPACE

#include "qdarwinwebviewplugin.moc"
