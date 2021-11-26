/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebView module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWEBVIEW_P_H
#define QWEBVIEW_P_H

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

#include "qabstractwebview_p.h"
#include "qwebviewinterface_p.h"
#include "qnativeviewcontroller_p.h"
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtGui/qimage.h>

QT_BEGIN_NAMESPACE

class QWebViewLoadRequestPrivate;

class Q_WEBVIEW_EXPORT QWebView
        : public QObject
        , public QWebViewInterface
        , public QNativeViewController
{
    Q_OBJECT
public:
    enum LoadStatus { // Changes here needs to be done in QQuickWebView as well
        LoadStartedStatus,
        LoadStoppedStatus,
        LoadSucceededStatus,
        LoadFailedStatus
    };

    explicit QWebView(QObject *p = nullptr);
    ~QWebView() override;

    QString httpUserAgent() const override;
    void setHttpUserAgent(const QString &httpUserAgent) override;
    QUrl url() const override;
    void setUrl(const QUrl &url) override;
    bool canGoBack() const override;
    bool canGoForward() const override;
    QString title() const override;
    int loadProgress() const override;
    bool isLoading() const override;

    void setParentView(QObject *view) override;
    QObject *parentView() const override;
    void setGeometry(const QRect &geometry) override;
    void setVisibility(QWindow::Visibility visibility) override;
    void setVisible(bool visible) override;
    void setFocus(bool focus) override;

public Q_SLOTS:
    void goBack() override;
    void goForward() override;
    void reload() override;
    void stop() override;
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl()) override;

Q_SIGNALS:
    void titleChanged();
    void urlChanged();
    void loadingChanged(const QWebViewLoadRequestPrivate &loadRequest);
    void loadProgressChanged();
    void javaScriptResult(int id, const QVariant &result);
    void requestFocus(bool focus);
    void httpUserAgentChanged();

protected:
    void init() override;
    void runJavaScriptPrivate(const QString &script,
                              int callbackId) override;

private Q_SLOTS:
    void onTitleChanged(const QString &title);
    void onUrlChanged(const QUrl &url);
    void onLoadProgressChanged(int progress);
    void onLoadingChanged(const QWebViewLoadRequestPrivate &loadRequest);
    void onHttpUserAgentChanged(const QString &httpUserAgent);

private:
    friend class QQuickViewController;
    friend class QQuickWebView;

    QAbstractWebView *d;

    // provisional data
    int m_progress;
    QString m_title;
    QUrl m_url;
    mutable QString m_httpUserAgent;
};

QT_END_NAMESPACE

#endif // QWEBVIEW_P_H
