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

#include "qquickwebviewloadrequest_p.h"
#include <QtWebView/private/qwebviewloadrequest_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype WebViewLoadRequest
    //! \instantiates QQuickWebViewLoadRequest
    \inqmlmodule QtWebView

    \brief A utility type for \l {WebView}'s \l {WebView::}{loadingChanged()} signal.

    The WebViewLoadRequest type contains load status information for the requested URL.

    \sa {WebView::loadingChanged()}{WebView.loadingChanged()}
*/
QQuickWebViewLoadRequest::QQuickWebViewLoadRequest(const QWebViewLoadRequestPrivate &d)
    : d_ptr(new QWebViewLoadRequestPrivate(d))
{
}

QQuickWebViewLoadRequest::~QQuickWebViewLoadRequest() { }

/*!
    \qmlproperty url QtWebView::WebViewLoadRequest::url
    \readonly

    The URL of the load request.
 */
QUrl QQuickWebViewLoadRequest::url() const
{
    Q_D(const QWebViewLoadRequest);
    return d->m_url;
}

/*!
    \qmlproperty enumeration WebViewLoadRequest::status
    \readonly

    This enumeration represents the load status of a web page load request.

    \value WebView.LoadStartedStatus The page is currently loading.
    \value WebView.LoadSucceededStatus The page was loaded successfully.
    \value WebView.LoadFailedStatus The page could not be loaded.

    \sa {WebView::loadingChanged()}{WebView.loadingChanged}
*/
QQuickWebView::LoadStatus QQuickWebViewLoadRequest::status() const
{
    Q_D(const QWebViewLoadRequest);
    return QQuickWebView::LoadStatus(d->m_status);
}

/*!
    \qmlproperty string QtWebView::WebViewLoadRequest::errorString
    \readonly

    Holds the error message if the load request failed.
*/
QString QQuickWebViewLoadRequest::errorString() const
{
    Q_D(const QWebViewLoadRequest);
    return d->m_errorString;
}

QT_END_NAMESPACE
