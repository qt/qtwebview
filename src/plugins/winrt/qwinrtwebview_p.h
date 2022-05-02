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

#ifndef QWINRTWEBVIEW_P_H
#define QWINRTWEBVIEW_P_H

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

#include <private/qabstractwebview_p.h>

namespace ABI {
    namespace Windows {
        namespace UI {
            namespace Xaml {
                namespace Controls {
                    struct IWebView;
                    struct IWebViewNavigationStartingEventArgs;
                    struct IWebViewNavigationCompletedEventArgs;
                    struct IWebViewUnviewableContentIdentifiedEventArgs;
                }
            }
        }
    }
}

QT_BEGIN_NAMESPACE

struct WinRTWebView;
class QWinRTWebViewPrivate : public QAbstractWebView
{
    Q_OBJECT
public:
    explicit QWinRTWebViewPrivate(QObject *parent = nullptr);
    ~QWinRTWebViewPrivate() override;

    QString httpUserAgent() const override;
    void setHttpUserAgent(const QString &userAgent) override;
    QUrl url() const override;
    void setUrl(const QUrl &url) override;
    bool canGoBack() const override;
    bool canGoForward() const override;
    QString title() const override;
    int loadProgress() const override;
    bool isLoading() const override;

    void setParentView(QObject *view) override;
    QObject *parentView() const override;
    void setGeometry(const QRect &geometry) override;
    void setVisibility(QWindow::Visibility visibility) override;
    void setVisible(bool visible) override;

public Q_SLOTS:
    void goBack() override;
    void goForward() override;
    void reload() override;
    void stop() override;
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl()) override;

protected:
    void runJavaScriptPrivate(const QString &script, int callbackId) override;

private:
    HRESULT onNavigationStarted(ABI::Windows::UI::Xaml::Controls::IWebView *, ABI::Windows::UI::Xaml::Controls::IWebViewNavigationStartingEventArgs *);
    HRESULT onNavigationCompleted(ABI::Windows::UI::Xaml::Controls::IWebView *, ABI::Windows::UI::Xaml::Controls::IWebViewNavigationCompletedEventArgs *);
    HRESULT onUnviewableContent(ABI::Windows::UI::Xaml::Controls::IWebView *,
                             ABI::Windows::UI::Xaml::Controls::IWebViewUnviewableContentIdentifiedEventArgs *);
    QScopedPointer<WinRTWebView> d;
};

QT_END_NAMESPACE

#endif // QWINRTWEBVIEW_P_H
