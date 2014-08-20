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

#ifndef QWEBVIEW_ANDROID_P_H
#define QWEBVIEW_ANDROID_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtGui/qwindow.h>
#include <QtAndroidExtras/qandroidjniobject.h>

#include "qwebview_p.h"

QT_BEGIN_NAMESPACE

class QAndroidWebViewPrivate : public QWebViewPrivate
{
    Q_OBJECT
public:
    QAndroidWebViewPrivate(QWebView *q);
    virtual ~QAndroidWebViewPrivate();

    QString getUrl() const;
    bool canGoBack() const;
    bool canGoForward() const;
    QString getTitle() const;

    void *nativeWebView() const;

public Q_SLOTS:
    void loadUrl(const QString &url);
    void goBack() const;
    void goForward() const;
    void stopLoading() const;

private:
    quintptr m_id;
    QAndroidJniObject m_viewController;
    QAndroidJniObject m_webView;
};

QT_END_NAMESPACE

#endif // QWEBVIEW_ANDROID_P_H
