// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSWEBCOMPONENTLISTENER_H
#define QOHOSWEBCOMPONENTLISTENER_H

#include "qohoswebresourceerror.h"
#include <QtCore/qobject.h>
#include <string>
#include <memory>

class QOhosWebComponentListener
{
public:
    virtual ~QOhosWebComponentListener();

    virtual void onErrorReceived(const std::string &url, const WebResourceError &error) = 0;
    virtual void onPageBegan(const std::string &url) = 0;
    virtual void onPageEnded(const std::string &url) = 0;
    virtual void onProgressChanged(int progress) = 0;
    virtual void onTitleReceived(const std::string &title) = 0;

protected:
    QOhosWebComponentListener();
};

std::shared_ptr<QOhosWebComponentListener> makeOhosWebComponentListenerExecutingInContextThread(
    std::weak_ptr<QOhosWebComponentListener> weakBaseListener, QObject *baseListenerContext);

#endif // QOHOSWEBCOMPONENTLISTENER_H
