/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtWebEngine module of the Qt Toolkit.
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

#include "qtwebviewfunctions.h"

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#include <QtWebEngine/qtwebengineglobal.h>
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

QT_BEGIN_NAMESPACE

/*!
    \namespace QtWebView
    \inmodule QtWebView
    \brief The QtWebView namespace provides functions that makes it easier to set-up and use the WebView.
    \inheaderfile QtWebView
*/

/*!
    \fn void QtWebView::initialize()

    This function initializes resources or sets options that are required by the different back-ends.

    \note The \c initialize() function needs to be called immediately after the QGuiApplication
    instance is created.
 */

void QtWebView::initialize()
{
#if defined(QT_WEBVIEW_WEBENGINE_BACKEND)
    QtWebEngine::initialize();
#elif defined(Q_OS_OSX)
    // On OS X, correct WebView / QtQuick compositing and stacking requires running
    // Qt in layer-backed mode, which again resuires rendering on the Gui thread.
    qWarning("Setting QT_MAC_WANTS_LAYER=1 and QSG_RENDER_LOOP=basic");
    qputenv("QT_MAC_WANTS_LAYER", "1");
    qputenv("QSG_RENDER_LOOP", "basic");
#endif
}

QT_END_NAMESPACE