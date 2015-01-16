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

#ifndef QQUICKVIEWCONTROLLER_H
#define QQUICKVIEWCONTROLLER_H

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

#include <QtQuick/QQuickItem>
#include <QtGui/qwindow.h>

QT_BEGIN_NAMESPACE

class QNativeViewController;

class Q_WEBVIEW_EXPORT QQuickViewController : public QQuickItem
{
    Q_OBJECT
public:
    explicit QQuickViewController(QQuickItem *parent = 0);
    ~QQuickViewController();

    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) Q_DECL_OVERRIDE;

public slots:
    void onWindowChanged(QQuickWindow* window);
    void onVisibleChanged();

protected:
    void componentComplete() Q_DECL_OVERRIDE;
    void setView(QNativeViewController *view);

private:
    friend class QQuickWebView;
    QNativeViewController *m_view;
};

QT_END_NAMESPACE

#endif // QTWINDOWCONTROLLERITEM_H
