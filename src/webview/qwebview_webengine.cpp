/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include "qwebview_webengine_p.h"
#include "qwebview_p.h"
#include "qwebviewloadrequest_p.h"

#include <QtWebView/private/qquickwebview_p.h>

#include <QtCore/qmap.h>
#include <QtGui/qguiapplication.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>
#include <QtCore/qrunnable.h>

#include <QtQuick/qquickwindow.h>
#include <QtQuick/qquickview.h>
#include <QtQuick/qquickitem.h>

#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcontext.h>

#include <QtWebEngine/private/qquickwebengineview_p.h>
#include <QtWebEngine/private/qquickwebengineloadrequest_p.h>

QT_BEGIN_NAMESPACE

static QByteArray qmlSource()
{
    return QByteArrayLiteral("import QtWebEngine 1.1\n"
                             "    WebEngineView {\n"
                             "}\n");
}

QWebViewPrivate *QWebViewPrivate::create(QWebView *q)
{
    return new QWebEngineWebViewPrivate(q);
}

QWebEngineWebViewPrivate::QWebEngineWebViewPrivate(QObject *p)
    : QWebViewPrivate(p)
    , m_webEngineView(0)
{

}

QWebEngineWebViewPrivate::~QWebEngineWebViewPrivate()
{
}

QUrl QWebEngineWebViewPrivate::url() const
{
    if (!m_webEngineView)
        return QUrl();

    return m_webEngineView->url();
}

void QWebEngineWebViewPrivate::setUrl(const QUrl &url)
{
    if (m_webEngineView)
        m_webEngineView->setUrl(url);
}

void QWebEngineWebViewPrivate::loadHtml(const QString &html, const QUrl &baseUrl)
{
    if (m_webEngineView)
        m_webEngineView->loadHtml(html, baseUrl);
}

bool QWebEngineWebViewPrivate::canGoBack() const
{
    if (!m_webEngineView)
        return false;

    return m_webEngineView->canGoBack();
}

void QWebEngineWebViewPrivate::goBack()
{
    if (m_webEngineView)
        m_webEngineView->goBack();
}

bool QWebEngineWebViewPrivate::canGoForward() const
{
    if (!m_webEngineView)
        return false;

    return m_webEngineView->canGoForward();
}

void QWebEngineWebViewPrivate::goForward()
{
    if (m_webEngineView)
        m_webEngineView->goForward();
}

void QWebEngineWebViewPrivate::reload()
{
    if (m_webEngineView)
        m_webEngineView->reload();
}

QString QWebEngineWebViewPrivate::title() const
{
    if (!m_webEngineView)
        return QString();

    return m_webEngineView->title();
}

void QWebEngineWebViewPrivate::setGeometry(const QRect &geometry)
{
    if (m_webEngineView)
        m_webEngineView->setSize(geometry.size());
}

void QWebEngineWebViewPrivate::setVisibility(QWindow::Visibility visibility)
{
    setVisible(visibility != QWindow::Hidden ? true : false);
}

void QWebEngineWebViewPrivate::runJavaScriptPrivate(const QString &script,
                                                    int callbackId)
{
    if (m_webEngineView)
        m_webEngineView->runJavaScript(script, QQuickWebView::takeCallback(callbackId));
}

void QWebEngineWebViewPrivate::setVisible(bool visible)
{
    if (m_webEngineView)
        m_webEngineView->setVisible(visible);
}

int QWebEngineWebViewPrivate::loadProgress() const
{
    if (!m_webEngineView)
        return 0;

    return m_webEngineView->loadProgress();
}

bool QWebEngineWebViewPrivate::isLoading() const
{
    if (!m_webEngineView)
        return false;

    return m_webEngineView->isLoading();
}

void QWebEngineWebViewPrivate::setParentView(QObject *parentView)
{
    if (m_webEngineView != 0 || parentView == 0)
        return;

    QObject *p = parent();
    QQuickItem *parentItem = 0;
    while (p != 0) {
        p = p->parent();
        parentItem = qobject_cast<QQuickWebView *>(p);
        if (parentItem != 0)
            break;
    }

    if (!parentItem)
        return;

    QQmlContext *ctx = QQmlEngine::contextForObject(parentItem);
    if (!ctx)
        return;

    QQmlEngine *engine = ctx->engine();
    if (!engine)
        return;

    QQmlComponent *component = new QQmlComponent(engine);
    component->setData(qmlSource(), QUrl::fromLocalFile(QLatin1String("")));
    QQuickWebEngineView *webEngineView = qobject_cast<QQuickWebEngineView *>(component->create());
    connect(webEngineView, &QQuickWebEngineView::urlChanged, this, &QWebEngineWebViewPrivate::q_urlChanged);
    connect(webEngineView, &QQuickWebEngineView::loadProgressChanged, this, &QWebEngineWebViewPrivate::q_loadProgressChanged);
    connect(webEngineView, &QQuickWebEngineView::loadingChanged, this, &QWebEngineWebViewPrivate::q_loadingChanged);
    connect(webEngineView, &QQuickWebEngineView::titleChanged, this, &QWebEngineWebViewPrivate::q_titleChanged);
    webEngineView->setParentItem(parentItem);
    m_webEngineView.reset(webEngineView);
}

QObject *QWebEngineWebViewPrivate::parentView() const
{
    return m_webEngineView ? m_webEngineView->window() : 0;
}

void QWebEngineWebViewPrivate::stop()
{
    if (m_webEngineView)
        m_webEngineView->stop();
}

void QWebEngineWebViewPrivate::q_urlChanged()
{
    Q_EMIT urlChanged(m_webEngineView->url());
}

void QWebEngineWebViewPrivate::q_loadProgressChanged()
{
    Q_EMIT loadProgressChanged(m_webEngineView->loadProgress());
}

void QWebEngineWebViewPrivate::q_titleChanged()
{
    Q_EMIT titleChanged(m_webEngineView->title());
}

void QWebEngineWebViewPrivate::q_loadingChanged(QQuickWebEngineLoadRequest *loadRequest)
{
    QWebViewLoadRequestPrivate lr(loadRequest->url(),
                                  static_cast<QWebView::LoadStatus>(loadRequest->status()), // These "should" match...
                                  loadRequest->errorString());

    Q_EMIT loadingChanged(lr);
}

QT_END_NAMESPACE
