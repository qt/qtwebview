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

#include "qwindowcontrolleritem_p.h"

#include <QtGui/QWindow>
#include <QtQuick/QQuickWindow>

#import <UIKit/UIView.h>
#import <UIKit/UIWindow.h>
#import <UIKit/UIViewController.h>

static inline CGRect toCGRect(const QRectF &rect)
{
    return CGRectMake(rect.x(), rect.y(), rect.width(), rect.height());
}

QWindowControllerItem::QWindowControllerItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_controlledUIView(0)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(onWindowChanged(QQuickWindow*)));
    connect(this, SIGNAL(visibleChanged()), this, SLOT(onVisibleChanged()));
}

QWindowControllerItem::~QWindowControllerItem()
{

}

void QWindowControllerItem::setNativeWindow(WId windowId)
{
   m_controlledUIView = reinterpret_cast<UIView *>(windowId);
}

void QWindowControllerItem::componentComplete()
{
   QQuickItem::componentComplete();
}

void QWindowControllerItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    [m_controlledUIView setFrame:toCGRect(newGeometry)];
}

void QWindowControllerItem::onWindowChanged(QQuickWindow* window)
{
    if (!m_controlledUIView)
        return;

    UIView *parentView = reinterpret_cast<UIView *>(window->winId());
    [parentView addSubview:m_controlledUIView];
}

void QWindowControllerItem::onVisibleChanged()
{
    [m_controlledUIView setHidden:isVisible()];
}
