/****************************************************************************
**
** Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QQUICKWEBVIEW_H
#define QQUICKWEBVIEW_H

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

#include "qwebviewinterface_p.h"

#if !defined(QT_WEBVIEW_WEBENGINE_BACKEND)

#include "qwebview_p.h"
#include "qquickviewcontroller_p.h"

QT_BEGIN_NAMESPACE

class QQuickWebViewLoadRequest;
class QWebViewLoadRequestPrivate;

class Q_WEBVIEW_EXPORT QQuickWebView : public QQuickViewController, public QWebViewInterface
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(int loadProgress READ loadProgress NOTIFY loadProgressChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY loadingChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY loadingChanged)
    Q_ENUMS(LoadStatus)

public:
    enum LoadStatus { // Changes here needs to be done in QWebView as well
        LoadStartedStatus,
        LoadStoppedStatus,
        LoadSucceededStatus,
        LoadFailedStatus
    };

    QQuickWebView(QQuickItem *parent = 0);
    ~QQuickWebView();

    QUrl url() const Q_DECL_OVERRIDE;
    void setUrl(const QUrl &url) Q_DECL_OVERRIDE;
    int loadProgress() const Q_DECL_OVERRIDE;
    QString title() const Q_DECL_OVERRIDE;
    bool canGoBack() const Q_DECL_OVERRIDE;
    bool isLoading() const Q_DECL_OVERRIDE;
    bool canGoForward() const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void goBack() Q_DECL_OVERRIDE;
    void goForward() Q_DECL_OVERRIDE;
    void reload() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl()) Q_DECL_OVERRIDE;
    void runJavaScript(const QString& script,
                       const QJSValue &callback = QJSValue());

Q_SIGNALS:
    void titleChanged();
    void urlChanged();
    void loadingChanged(QQuickWebViewLoadRequest *loadRequest);
    void loadProgressChanged();

protected:
    void runJavaScriptPrivate(const QString& script,
                              int callbackId) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onRunJavaScriptResult(int id, const QVariant &variant);
    void onFocusRequest(bool focus);
    void onLoadingChanged(const QWebViewLoadRequestPrivate &loadRequest);

private:
    QScopedPointer<QWebView> m_webView;
};

QT_END_NAMESPACE

#else

#include <QtWebEngine/private/qquickwebengineview_p.h>

QT_BEGIN_NAMESPACE

class Q_WEBVIEW_EXPORT QQuickWebView : public QQuickWebEngineView
{
    Q_OBJECT
public:
    QQuickWebView(QQuickItem *parent = 0) : QQuickWebEngineView(parent) { }
};

QT_END_NAMESPACE

#endif

#endif // QQUICKWEBVIEW_H
