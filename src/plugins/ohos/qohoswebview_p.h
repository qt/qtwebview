// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSWEBVIEW_P_H
#define QOHOSWEBVIEW_P_H

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

#include <private/qwebview_p.h>

QT_BEGIN_NAMESPACE

QWebViewPrivate *makeOhosWebView(QWebView *view);
void prepareOhosWebView();

QT_END_NAMESPACE

#endif // QOHOSWEBVIEW_P_H
