// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include "qwebviewsettings.h"
#include "qwebview_p.h"

QT_BEGIN_NAMESPACE

// FIXME add c++ docs

QWebViewSettings::QWebViewSettings(QAbstractWebViewSettings *settings) : d(settings)
{
    Q_ASSERT(settings != nullptr);
}

QWebViewSettings::~QWebViewSettings() { }

bool QWebViewSettings::localStorageEnabled() const
{
    return d->localStorageEnabled();
}

void QWebViewSettings::setLocalStorageEnabled(bool enabled)
{
    if (d->localStorageEnabled() == enabled)
        return;

    d->setLocalStorageEnabled(enabled);
    emit localStorageEnabledChanged();
}

bool QWebViewSettings::javaScriptEnabled() const
{
    return d->javaScriptEnabled();
}

void QWebViewSettings::setJavaScriptEnabled(bool enabled)
{
    if (d->javaScriptEnabled() == enabled)
        return;

    d->setJavaScriptEnabled(enabled);
    emit javaScriptEnabledChanged();
}

void QWebViewSettings::setAllowFileAccess(bool enabled)
{
    if (d->allowFileAccess() == enabled)
        return;

    d->setAllowFileAccess(enabled);
    emit allowFileAccessChanged();
}

bool QWebViewSettings::allowFileAccess() const
{
    return d->allowFileAccess();
}

bool QWebViewSettings::localContentCanAccessFileUrls() const
{
    return d->localContentCanAccessFileUrls();
}

void QWebViewSettings::setLocalContentCanAccessFileUrls(bool enabled)
{
    if (d->localContentCanAccessFileUrls() == enabled)
        return;

    d->setLocalContentCanAccessFileUrls(enabled);
    emit localContentCanAccessFileUrlsChanged();
}

QT_END_NAMESPACE
