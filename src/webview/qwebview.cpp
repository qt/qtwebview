/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWebView module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwebview_p.h"

QT_BEGIN_NAMESPACE

QWebViewPrivate::QWebViewPrivate(QWebView *q)
    : m_loadProgress(0)
    , m_loading(false)
    , q_ptr(q)
{
    connect(this,SIGNAL(pageStarted(QString)),SLOT(onPageStartedPrivate(QString)));
    connect(this,SIGNAL(pageFinished(QString)),SLOT(onPageFinishedPrivate(QString)));
    connect(this,SIGNAL(titleChanged(QString)),SLOT(onTitleChangedPrivate(QString)));
    connect(this,SIGNAL(progressChanged(int)),SLOT(onProgressChangedPrivate(int)));
}

QWebViewPrivate::~QWebViewPrivate()
{
}

void QWebViewPrivate::onPageStartedPrivate(QString url)
{
    Q_Q(QWebView);
    const QUrl newUrl = QUrl::fromUserInput(url);
    if (m_url != newUrl) {
        m_url = newUrl;
        Q_EMIT q->urlChanged();
    }

    if (!m_loading) {
        m_loading = true;
        Q_EMIT q->loadingChanged();
    }
}

void QWebViewPrivate::onPageFinishedPrivate(QString url)
{
    Q_Q(QWebView);
    const QUrl newUrl = QUrl::fromUserInput(url);
    if (m_url != newUrl) {
        m_url = newUrl;
        Q_EMIT q->urlChanged();
    }

    if (m_loading) {
        m_loading = false;
        Q_EMIT q->loadingChanged();
    }
}

void QWebViewPrivate::onTitleChangedPrivate(QString title)
{
    Q_Q(QWebView);
    if (m_title == title)
        return;

    m_title = title;
    Q_EMIT q->titleChanged();
}

void QWebViewPrivate::onProgressChangedPrivate(int progress)
{
    Q_Q(QWebView);
    if (m_loadProgress == progress)
        return;

    m_loadProgress = progress;
    Q_EMIT q->loadProgressChanged();
}

// This is called by QWindowControllerItem::componentComplete
// to create the native web view on the parent window. Alternatively,
// it can be called from QWebViewPrivate::create() if no delayed
// creation is desired.
void QWebViewPrivate::ensureNativeWebView()
{
    Q_Q(QWebView);
    if (q->controlledWindow())
        return;
    if (const WId id = WId(nativeWebView()))
        q->setNativeWindow(id);
    else
        qWarning("No native web view. Missing platform support?");
}

/*!
    \qmltype WebView
    \inqmlmodule QtWebView
    \ingroup qtwebview
    \brief A component for displaying web content

    WebView is a component for displaying web content which is implemented using native
    APIs on the platforms where this is available, thus it does not necessarily require
    including a full web browser stack as part of the application.

    WebView is currently supported on the following platforms: Android.

    \note Due to platform limitations overlapping the WebView and other QML components
    is not supported.
*/

QWebView::QWebView(QQuickItem *parent)
    : QWindowControllerItem(parent)
    , d_ptr(QWebViewPrivate::create(this))
{
}

QWebView::~QWebView()
{
}

/*!
  \qmlproperty url QtWebView::WebView::url

  The url of currently loaded web page. Changing this will trigger
  loading new content.

  The url is used as-is. Urls that originate from user input should
  be parsed with QUrl::fromUserInput.
*/

void QWebView::setUrl(const QUrl &url)
{
    Q_D(QWebView);
    if (d->m_url == url)
        return;

    d->loadUrl(url.toString());
}

/*!
  \qmlproperty string QtWebView::WebView::title

  The title of the currently loaded web page.
*/

QString QWebView::title() const
{
    Q_D(const QWebView);
    return d->m_title;
}

QUrl QWebView::url() const
{
    Q_D(const QWebView);
    return d->m_url;
}

/*!
  \qmlproperty bool QtWebView::WebView::canGoBack

  True if it's currently possible to navigate back in the web history.
*/

bool QWebView::canGoBack() const
{
    Q_D(const QWebView);
    return d->canGoBack();
}

/*!
  \qmlproperty bool QtWebView::WebView::canGoForward

  True if it's currently possible to navigate forward in the web history.
*/

bool QWebView::canGoForward() const
{
    Q_D(const QWebView);
    return d->canGoForward();
}

/*!
  \qmlproperty int QtWebView::WebView::loadProgress

  The current load progress of the web content, represented as
  an integer between 0 and 100.
*/
int QWebView::loadProgress() const
{
    Q_D(const QWebView);
    return d->m_loadProgress;
}

/*!
  \qmlproperty bool QtWebView::WebView::loading

  True if the WebView is currently in the process of loading
  new content, and false if not.
*/
bool QWebView::isLoading() const
{
    Q_D(const QWebView);
    return d->m_loading;
}

/*!
    \qmlmethod void QtWebView::WebView::goBack()

    Navigates back in the web history.
*/
void QWebView::goBack()
{
    Q_D(QWebView);
    d->goBack();
}

/*!
    \qmlmethod void QtWebView::WebView::goForward()

    Navigates forward in the web history.
*/
void QWebView::goForward()
{
    Q_D(QWebView);
    d->goForward();
}

/*!
    \qmlmethod void QtWebView::WebView::reload()

    Reloads the current url.
*/
void QWebView::reload()
{
    Q_D(QWebView);
    if (d->m_url.isValid())
        d->loadUrl(d->m_url.toString());
}

/*!
    \qmlmethod void QtWebView::WebView::stop()

    Stops loading the current url.
*/
void QWebView::stop()
{
    Q_D(QWebView);
    d->stopLoading();
}

void QWebView::componentComplete()
{
    Q_D(QWebView);
    QWindowControllerItem::componentComplete();
    d->ensureNativeWebView();
}

QT_END_NAMESPACE
