/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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

QWebView::QWebView(QObject *p)
    : QObject(p),
      d_ptr(QWebViewPrivate::create(this))
{
    Q_D(QWebView);
    connect(d, SIGNAL(titleChanged()), this, SIGNAL(titleChanged()));
    connect(d, SIGNAL(urlChanged()), this, SIGNAL(urlChanged()));
    connect(d, SIGNAL(loadingChanged()), this, SIGNAL(loadingChanged()));
    connect(d, SIGNAL(loadProgressChanged()), this, SIGNAL(loadProgressChanged()));
    connect(d, SIGNAL(requestFocus(bool)), this, SIGNAL(requestFocus(bool)));
}

QWebView::~QWebView()
{
}

QUrl QWebView::url() const
{
    Q_D(const QWebView);
    return d->url();
}

void QWebView::setUrl(const QUrl &url)
{
    Q_D(QWebView);
    d->setUrl(url);
}

bool QWebView::canGoBack() const
{
    Q_D(const QWebView);
    return d->canGoBack();
}

void QWebView::goBack()
{
    Q_D(QWebView);
    d->goBack();
}

bool QWebView::canGoForward() const
{
    Q_D(const QWebView);
    return d->canGoForward();
}

void QWebView::goForward()
{
    Q_D(QWebView);
    d->goForward();
}

void QWebView::reload()
{
    Q_D(QWebView);
    d->reload();
}

void QWebView::stop()
{
    Q_D(QWebView);
    d->stop();
}

QString QWebView::title() const
{
    Q_D(const QWebView);
    return d->title();
}

int QWebView::loadProgress() const
{
    Q_D(const QWebView);
    return d->loadProgress();
}

bool QWebView::isLoading() const
{
    Q_D(const QWebView);
    return d->isLoading();
}

void QWebView::setParentView(QObject *view)
{
    Q_D(QWebView);
    d->setParentView(view);
}

void QWebView::setGeometry(const QRect &geometry)
{
    Q_D(QWebView);
    d->setGeometry(geometry);
}

void QWebView::setVisibility(QWindow::Visibility visibility)
{
    Q_D(QWebView);
    d->setVisibility(visibility);
}

void QWebView::setVisible(bool visible)
{
    Q_D(QWebView);
    d->setVisible(visible);
}

void QWebView::setFocus(bool focus)
{
    Q_D(QWebView);
    d->setFocus(focus);
}

void QWebView::init()
{
    Q_D(QWebView);
    d->init();
}

QT_END_NAMESPACE
