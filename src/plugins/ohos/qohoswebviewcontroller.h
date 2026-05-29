// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSWEBVIEWCONTROLLER_H
#define QOHOSWEBVIEWCONTROLLER_H

#include "qohoswebcomponentlistener.h"
#include <QtCore/private/qcore_ohos_p.h>
#include <QtCore/private/qohoscommon_p.h>
#include <QtCore/qglobal.h>
#include <arkui/native_node.h>
#include <memory>
#include <string>

QT_BEGIN_NAMESPACE

class QOhosWebViewController
{
public:
    QOhosWebViewController(const QOhosWebViewController &) = delete;
    QOhosWebViewController &operator=(const QOhosWebViewController &) = delete;

    virtual ~QOhosWebViewController();

    virtual ::ArkUI_NodeHandle createEmbeddedWebComponentNodeOrFail(
        std::shared_ptr<QOhosWebComponentListener> webComponentListener,
        QObject *webComponentListenerContext) = 0;

    virtual bool tryLoadUrl(const std::string &url) = 0;
    virtual std::string getUrl() = 0;

    virtual bool tryLoadHtml(const std::string &data, const std::string &mimeType,
                             const std::string &encoding, const std::string &baseUrl,
                             const std::string &historyUrl) = 0;

    virtual bool canGoBack() = 0;
    virtual bool canGoForward() = 0;

    virtual void goBack() = 0;
    virtual void goForward() = 0;

    virtual void refresh() = 0;
    virtual void stop() = 0;

    virtual std::string getTitle() = 0;

    virtual std::optional<std::string> tryRunJavaScript(const std::string &script) = 0;

protected:
    QOhosWebViewController();
};

std::shared_ptr<QOhosWebViewController> makeOhosWebViewController();

QT_END_NAMESPACE

#endif // QOHOSWEBVIEWCONTROLLER_H
