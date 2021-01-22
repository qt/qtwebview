/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#ifndef QQUICKWEBVIEWREQUEST_H
#define QQUICKWEBVIEWREQUEST_H

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

#include <QtWebView/private/qquickwebview_p.h>
#include <QtWebView/qwebview_global.h>
#include <QObject>

QT_BEGIN_NAMESPACE

class QWebViewLoadRequestPrivate;

class Q_WEBVIEW_EXPORT QQuickWebViewLoadRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url)
    Q_PROPERTY(QQuickWebView::LoadStatus status READ status)
    Q_PROPERTY(QString errorString READ errorString)

public:
    ~QQuickWebViewLoadRequest();

    QUrl url() const;
    QQuickWebView::LoadStatus status() const;
    QString errorString() const;

private:
    friend class QQuickWebView;
    explicit QQuickWebViewLoadRequest(const QWebViewLoadRequestPrivate &d);
    Q_DECLARE_PRIVATE(QWebViewLoadRequest)
    QScopedPointer<QWebViewLoadRequestPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // QQUICKWEBVIEWREQUEST_H
