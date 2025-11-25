// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBVIEWSETTINGS_H
#define QWEBVIEWSETTINGS_H

#include <QtWebView/qwebview_global.h>

#include <QtCore/qobject.h>

#include <memory>

QT_BEGIN_NAMESPACE

class QAbstractWebViewSettings;
class QWindow;

class Q_WEBVIEW_EXPORT QWebViewSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool localStorageEnabled READ localStorageEnabled WRITE setLocalStorageEnabled NOTIFY
                       localStorageEnabledChanged)
    Q_PROPERTY(bool javaScriptEnabled READ javaScriptEnabled WRITE setJavaScriptEnabled NOTIFY
                       javaScriptEnabledChanged)
    Q_PROPERTY(bool allowFileAccess READ allowFileAccess WRITE setAllowFileAccess NOTIFY
                       allowFileAccessChanged)
    Q_PROPERTY(bool localContentCanAccessFileUrls READ localContentCanAccessFileUrls WRITE
                       setLocalContentCanAccessFileUrls NOTIFY localContentCanAccessFileUrlsChanged)

public:
    explicit QWebViewSettings(QAbstractWebViewSettings *webview);
    virtual ~QWebViewSettings() override;

    bool localStorageEnabled() const;
    bool javaScriptEnabled() const;
    bool allowFileAccess() const;
    bool localContentCanAccessFileUrls() const;

    void setLocalStorageEnabled(bool enabled);
    void setJavaScriptEnabled(bool enabled);
    void setAllowFileAccess(bool enabled);
    void setLocalContentCanAccessFileUrls(bool enabled);

Q_SIGNALS:
    void localStorageEnabledChanged();
    void javaScriptEnabledChanged();
    void allowFileAccessChanged();
    void localContentCanAccessFileUrlsChanged();

private:
    std::unique_ptr<QAbstractWebViewSettings> d;
};

QT_END_NAMESPACE

#endif // QWEBVIEWSETTINGS_H
