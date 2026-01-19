// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include "qwebview.h"

#include "qwebviewsettings.h"
#include "qwebviewplugin_p.h"
#include "qwebviewloadinginfo.h"
#include "qwebviewfactory_p.h"

QT_BEGIN_NAMESPACE

QWebView::QWebView(QWindow *parent) : QWindow(parent), d(QWebViewFactory::createWebView(this))
{
    Q_ASSERT(d);
    d->m_settings.reset(new QWebViewSettings(d->settings()));
    qRegisterMetaType<QWebViewLoadingInfo>();
}

QWebView::QWebView(QScreen *screen) : QWindow(screen), d(QWebViewFactory::createWebView(this))
{
    Q_ASSERT(d);
    d->m_settings.reset(new QWebViewSettings(d->settings()));
    qRegisterMetaType<QWebViewLoadingInfo>();
}

QWebView::~QWebView() { }

QString QWebView::httpUserAgent() const
{
    return d->httpUserAgent();
}

void QWebView::setHttpUserAgent(const QString &userAgent)
{
    return d->setHttpUserAgent(userAgent);
}

QUrl QWebView::url() const
{
    return d->url();
}

void QWebView::setUrl(const QUrl &url)
{
    d->setUrl(url);
}

bool QWebView::canGoBack() const
{
    return d->canGoBack();
}

void QWebView::goBack()
{
    d->goBack();
}

bool QWebView::canGoForward() const
{
    return d->canGoForward();
}

void QWebView::goForward()
{
    d->goForward();
}

void QWebView::reload()
{
    d->reload();
}

void QWebView::stop()
{
    d->stop();
}

QString QWebView::title() const
{
    return d->title();
}

int QWebView::loadProgress() const
{
    return d->loadProgress();
}

bool QWebView::isLoading() const
{
    return d->isLoading();
}

QWebViewSettings *QWebView::settings()
{
    return d->m_settings.get();
}

void QWebView::loadHtml(const QString &html, const QUrl &baseUrl)
{
    d->loadHtml(html, baseUrl);
}

void QWebView::runJavaScript(const QString &script,
                             const std::function<void(const QVariant &)> &resultCallback)
{
    return d->runJavaScript(script, resultCallback);
}

void QWebView::setCookie(const QString &domain, const QString &name, const QString &value)
{
    d->setCookie(domain, name, value);
}

void QWebView::deleteCookie(const QString &domain, const QString &name)
{
    d->deleteCookie(domain, name);
}

void QWebView::deleteAllCookies()
{
    d->deleteAllCookies();
}

QT_END_NAMESPACE

#include "moc_qwebview.cpp"
#include "moc_qwebview_p.cpp"
