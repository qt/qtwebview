// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include <QtWebView/private/qwebviewloadrequest_p.h>

QT_BEGIN_NAMESPACE

QWebViewLoadRequestPrivate::QWebViewLoadRequestPrivate()
{

}

QWebViewLoadRequestPrivate::QWebViewLoadRequestPrivate(const QUrl &url,
                                                       QWebView::LoadStatus status,
                                                       const QString &errorString)
    : m_url(url)
    , m_status(status)
    , m_errorString(errorString)
{

}

QWebViewLoadRequestPrivate::~QWebViewLoadRequestPrivate()
{

}

QT_END_NAMESPACE
