/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWebView module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickwebviewloadrequest_p.h"
#include <QtWebView/private/qwebviewloadrequest_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype WebViewLoadRequest
    \instantiates QQuickWebViewLoadRequest
    \inqmlmodule QtWebView

    \brief A utility class for the QtWebView::WebView::loadingChanged signal.

    This class contains load status information for the requested URL.

    \sa QtWebView::WebView::loadingChanged
*/
QQuickWebViewLoadRequest::QQuickWebViewLoadRequest(const QWebViewLoadRequestPrivate &d)
    : d_ptr(new QWebViewLoadRequestPrivate(d))
{

}

QQuickWebViewLoadRequest::~QQuickWebViewLoadRequest()
{

}

/*!
    \qmlproperty url QtWebView::WebView::WebViewLoadRequest::url

    The URL of the load request.
 */
QUrl QQuickWebViewLoadRequest::url() const
{
    Q_D(const QWebViewLoadRequest);
    return d->m_url;
}

/*!
    \qmlproperty enumeration WebViewLoadRequest::status

    This enumeration represents the load status of a web page load request.

    \value QtWebView::WebView::LoadStartedStatus The page is currently loading.
    \value QtWebView::WebView::LoadSucceededStatus The page was loaded successfully.
    \value QtWebView::WebView::LoadFailedStatus The page could not be loaded.

    \sa QtWebView::WebView::loadingChanged
*/
QQuickWebView::LoadStatus QQuickWebViewLoadRequest::status() const
{
    Q_D(const QWebViewLoadRequest);
    return QQuickWebView::LoadStatus(d->m_status);
}

/*!
    \qmlproperty string QtWebView::WebView::WebViewLoadRequest::errorString

    Holds the error message if the load request fails.
*/
QString QQuickWebViewLoadRequest::errorString() const
{
    Q_D(const QWebViewLoadRequest);
    return d->m_errorString;
}

QT_END_NAMESPACE
