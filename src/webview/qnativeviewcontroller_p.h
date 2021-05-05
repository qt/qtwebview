/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
**
**
**
******************************************************************************/

#ifndef QNATIVEVIEWCONTROLLER_P_H
#define QNATIVEVIEWCONTROLLER_P_H

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

#include "qwebview_global.h"
#include <QtCore/qrect.h>
#include <QtGui/qwindow.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QNativeViewController
{
public:
    virtual ~QNativeViewController() {}
    virtual void setParentView(QObject *view) = 0;
    virtual QObject *parentView() const = 0;
    virtual void setGeometry(const QRect &geometry) = 0;
    virtual void setVisibility(QWindow::Visibility visibility) = 0;
    virtual void setVisible(bool visible) = 0;
    virtual void init() { }
    virtual void setFocus(bool focus) { Q_UNUSED(focus); }
    virtual void updatePolish() { }
};

QT_END_NAMESPACE

#endif // QNATIVEVIEWCONTROLLER_P_H

