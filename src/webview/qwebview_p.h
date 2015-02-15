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

#ifndef QWEBVIEW_H
#define QWEBVIEW_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtWebView/qwebview_global.h>

#if !defined(QT_WEBVIEW_WEBENGINE_BACKEND)

#include "qwindowcontrolleritem_p.h"
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtGui/qimage.h>


QT_BEGIN_NAMESPACE

class QWebView;
class QWebViewPrivate : public QObject
{
    Q_OBJECT
public:
    QWebViewPrivate(QWebView *q);
    ~QWebViewPrivate();

    virtual QString getUrl() const = 0;
    virtual bool canGoBack() const = 0;
    virtual bool canGoForward() const = 0;
    virtual QString getTitle() const = 0;
    virtual void *nativeWebView() const = 0;

    static QWebViewPrivate *create(QWebView *q);

public Q_SLOTS:
    virtual void loadUrl(const QString &url) = 0;
    virtual void goBack() const = 0;
    virtual void goForward() const = 0;
    virtual void stopLoading() const = 0;

private Q_SLOTS:
    void onPageStartedPrivate(QString url);
    void onPageFinishedPrivate(QString url);
    void onTitleChangedPrivate(QString title);
    void onProgressChangedPrivate(int progress);

Q_SIGNALS:
    void pageStarted(QString url);
    void pageFinished(QString url);
    void progressChanged(int progress);
    void titleChanged(QString title);

protected:
    Q_DECLARE_PUBLIC(QWebView)
    void ensureNativeWebView();

private:
    int m_loadProgress;
    bool m_loading;
    QString m_title;
    QUrl m_url;
    QWebView *q_ptr;
};

class Q_WEBVIEW_EXPORT QWebView : public QWindowControllerItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(int loadProgress READ loadProgress NOTIFY loadProgressChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY loadingChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY loadingChanged)

public:
    QWebView(QQuickItem *parent = 0);
    ~QWebView();

    QUrl url() const;
    void setUrl(const QUrl &url);
    int loadProgress() const;
    QString title() const;
    bool canGoBack() const;
    bool isLoading() const;
    bool canGoForward() const;

public Q_SLOTS:
    void goBack();
    void goForward();
    void reload();
    void stop();

Q_SIGNALS:
    void titleChanged();
    void urlChanged();
    void loadingChanged();
    void loadProgressChanged();

protected:
    void componentComplete() Q_DECL_OVERRIDE;

private:
    void load(const QUrl &url);

    Q_DECLARE_PRIVATE(QWebView)
    QScopedPointer<QWebViewPrivate> d_ptr;
};

QT_END_NAMESPACE

#else

#  include <QtWebEngine/private/qquickwebengineview_p.h>

QT_BEGIN_NAMESPACE

class Q_WEBVIEW_EXPORT QWebView : public QQuickWebEngineView
{
    Q_OBJECT
};

QT_END_NAMESPACE

#endif


#endif
