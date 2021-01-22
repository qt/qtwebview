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


#ifndef QWEBVIEWLOADREQUESTPRIVATE_H
#define QWEBVIEWLOADREQUESTPRIVATE_H

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

#include <QtWebView/private/qwebview_p.h>
#include <QtCore/qstring.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

class Q_WEBVIEW_EXPORT QWebViewLoadRequestPrivate
{
public:
    QWebViewLoadRequestPrivate();
    QWebViewLoadRequestPrivate(const QUrl &url,
                               QWebView::LoadStatus status,
                               const QString &errorString);
    ~QWebViewLoadRequestPrivate();

    QUrl m_url;
    QWebView::LoadStatus m_status;
    QString m_errorString;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QWebViewLoadRequestPrivate)

#endif // QWEBVIEWLOADREQUESTPRIVATE_H
