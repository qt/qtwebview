// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include "qwebviewsettings.h"
#include "qwebview_p.h"

QT_BEGIN_NAMESPACE

// FIXME add c++ docs

QWebViewSettings::QWebViewSettings(QWebViewSettingsPrivate *settings) : d(settings)
{
    Q_ASSERT(settings != nullptr);
}

QWebViewSettings::~QWebViewSettings() { }

bool QWebViewSettings::testAttribute(QWebViewSettings::WebAttribute attribute) const
{
    return d->testAttribute(attribute);
}

void QWebViewSettings::setAttribute(QWebViewSettings::WebAttribute attribute, bool value)
{
    d->setAttribute(attribute, value);
}

QT_END_NAMESPACE
