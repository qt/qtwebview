// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohoswebcomponentlistener.h"
#include "qohoswebresourceerror.h"
#include "qohoswebview_p.h"
#include "qohoswebviewcontroller.h"
#include <QtWebView/private/qwebviewfactory_p.h>
#include <QtCore/private/qohoslogger_p.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qjsonobject.h>
#include <QtGui/qpa/qplatformwindow_p.h>

QT_BEGIN_NAMESPACE

namespace {

class QOhosWebViewSettingPrivate : public QWebViewSettingsPrivate
{
public:
    explicit QOhosWebViewSettingPrivate(std::shared_ptr<QOhosWebViewController> controller);

private:
    bool doTestAttribute(WebAttribute attribute) const final;
    void doSetAttribute(WebAttribute attribute, bool value) final;

    std::shared_ptr<QOhosWebViewController> m_webViewController;
};

class QOhosWebViewPrivate : public QWebViewPrivate
{
    Q_OBJECT
public:
    explicit QOhosWebViewPrivate(QWebView *view);
    ~QOhosWebViewPrivate() override;

    QString httpUserAgent() const override;
    void setHttpUserAgent(const QString &httpUserAgent) override;
    QUrl url() const override;
    void setUrl(const QUrl &url) override;
    bool canGoBack() const override;
    bool canGoForward() const override;
    QString title() const override;
    int loadProgress() const override;
    bool isLoading() const override;

    void initialize(QObject *context) override;
    void setCookie(const QString &domain, const QString &name, const QString &value) override;
    void deleteCookie(const QString &domain, const QString &name) override;
    void deleteAllCookies() override;
    QWindow *nativeWindow() const override;

    void onErrorReceived(const QString &url, const WebResourceError &error);
    void onPageBegan(const QString &url);
    void onPageEnded(const QString &url);
    void onProgressChanged(int progress);
    void onTitleUpdated(const QString &title);

public Q_SLOTS:
    void goBack() override;
    void goForward() override;
    void reload() override;
    void stop() override;
    void loadHtml(const QString &html, const QUrl &baseUrl) override;

protected:
    void runJavaScript(const QString &script,
                       const std::function<void(const QVariant &)> &resultCallback) override;
    QWebViewSettingsPrivate *settings() const override;

private:
    void emitLoadingChangeSignalAndUpdateLoadStatus(const QWebViewLoadingInfo &loadingInfo);

    QPointer<QWindow> m_webViewWindow;
    std::shared_ptr<QOhosWebViewController> m_webViewController;
    std::shared_ptr<QOhosWebComponentListener> m_webComponentListener;

    int m_loadProgress = 0;
    QWebViewLoadingInfo::LoadStatus m_loadStatus = QWebViewLoadingInfo::LoadStatus::Stopped;
    QString m_url;
};

class QOhosWebComponentListenerImpl : public QOhosWebComponentListener
{
public:
    QOhosWebComponentListenerImpl(QPointer<QOhosWebViewPrivate> webViewPrivate);
    ~QOhosWebComponentListenerImpl();

    void onErrorReceived(const std::string &url, const WebResourceError &error) override;
    void onPageBegan(const std::string &url) override;
    void onPageEnded(const std::string &url) override;
    void onProgressChanged(int progress) override;
    void onTitleReceived(const std::string &title) override;

private:
    QPointer<QOhosWebViewPrivate> m_webViewPrivate;
};

QOhosWebViewSettingPrivate::QOhosWebViewSettingPrivate(std::shared_ptr<QOhosWebViewController> controller)
    : m_webViewController(controller)
{
}

bool QOhosWebViewSettingPrivate::doTestAttribute(WebAttribute attribute) const
{
    return m_webViewController->testAttribute(attribute);
}

void QOhosWebViewSettingPrivate::doSetAttribute(WebAttribute attribute, bool value)
{
    m_webViewController->setAttribute(attribute, value);
}

QWindow *createEmbeddedWebViewWindow(
    std::shared_ptr<QOhosWebViewController> webViewController,
    std::shared_ptr<QOhosWebComponentListener> webComponentListener,
    QObject *webComponentListenerContext)
{
    auto webViewNode = webViewController->createEmbeddedWebComponentNodeOrFail(
        webComponentListener, webComponentListenerContext);

    return QNativeInterface::Private::QOhosWindow::fromNative(webViewNode);
}

QVariant convertJavaScriptResultToQVariantOrNull(std::optional<std::string> javaScriptResult)
{
    if (!javaScriptResult.has_value())
        return QVariant();

    const QByteArray jsonData = "{ \"data\": " + QByteArray(javaScriptResult.value().c_str()) + " }";

    QJsonParseError parseError;
    const auto jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qOhosCritical(QtForOhos)
            << Q_FUNC_INFO
            << "Failed to parse javaScript result, reason:"
            << parseError.errorString();
        return QVariant();
    }

    return jsonDoc.isObject()
        ? jsonDoc.object()[QStringLiteral("data")].toVariant()
        : QVariant();
}

QOhosWebViewPrivate::QOhosWebViewPrivate(QWebView *view)
    : QWebViewPrivate(view)
    , m_webViewController(makeOhosWebViewController())
{
    m_webComponentListener = std::make_shared<QOhosWebComponentListenerImpl>(this);
    m_webViewWindow = createEmbeddedWebViewWindow(m_webViewController, m_webComponentListener, this);
    if (m_webViewWindow) {
        m_webViewWindow->setParent(view);
        connect(view, &QWindow::widthChanged, m_webViewWindow.data(), &QWindow::setWidth);
        connect(view, &QWindow::heightChanged, m_webViewWindow.data(), &QWindow::setHeight);
        connect(view, &QWindow::visibleChanged, m_webViewWindow.data(), &QWindow::setVisible);
    }
}

QOhosWebViewPrivate::~QOhosWebViewPrivate() = default;

QString QOhosWebViewPrivate::httpUserAgent() const
{
    return QString();
}

void QOhosWebViewPrivate::setHttpUserAgent(const QString &httpUserAgent)
{
    Q_UNUSED(httpUserAgent);
}

QUrl QOhosWebViewPrivate::url() const
{
    auto controllerUrl = QString::fromStdString(m_webViewController->getUrl());

    return QFileInfo::exists(controllerUrl)
        ? QUrl::fromLocalFile(controllerUrl)
        : QUrl::fromEncoded(controllerUrl.toUtf8());
}

void QOhosWebViewPrivate::setUrl(const QUrl &url)
{
    auto controllerUrl = url.isLocalFile()
        ? QFileInfo(url.toLocalFile()).absoluteFilePath().toStdString()
        : url.toString().toStdString();

    if (!m_webViewController->tryLoadUrl(controllerUrl)) {
        qOhosWarning(QtForOhos) << Q_FUNC_INFO << "Failed to set url:" << controllerUrl.c_str();
    }
}

void QOhosWebViewPrivate::loadHtml(const QString &html, const QUrl &baseUrl)
{
    const std::string mimeType = "text/html;charset=UTF-8";
    const std::string encoding = "UTF-8";

    bool ok;
    if (baseUrl.isEmpty() || baseUrl.scheme() == QLatin1String("data")) {
        const std::string data = QUrl::toPercentEncoding(html).toStdString();
        ok = m_webViewController->tryLoadHtml(data, mimeType, encoding, {}, {});
    } else {
        const std::string base = baseUrl.toString().toStdString();
        ok = m_webViewController->tryLoadHtml(html.toStdString(), mimeType, encoding, base, base);
    }
    if (!ok)
        qOhosWarning(QtForOhos) << Q_FUNC_INFO << "Failed to load html:" << html;
}

bool QOhosWebViewPrivate::canGoBack() const
{
    return m_webViewController->canGoBack();
}

void QOhosWebViewPrivate::goBack()
{
    m_webViewController->goBack();
}

bool QOhosWebViewPrivate::canGoForward() const
{
    return m_webViewController->canGoForward();
}

void QOhosWebViewPrivate::goForward()
{
    m_webViewController->goForward();
}

void QOhosWebViewPrivate::reload()
{
    m_webViewController->refresh();
}

QString QOhosWebViewPrivate::title() const
{
    return QString::fromStdString(m_webViewController->getTitle());
}

void QOhosWebViewPrivate::runJavaScript(const QString &script,
                                        const std::function<void(const QVariant &)> &resultCallback)
{
    auto result = convertJavaScriptResultToQVariantOrNull(m_webViewController->tryRunJavaScript(script.toStdString()));
    if (resultCallback)
        resultCallback(result);
}

QWebViewSettingsPrivate *QOhosWebViewPrivate::settings() const
{
    return new QOhosWebViewSettingPrivate(m_webViewController);
}

int QOhosWebViewPrivate::loadProgress() const
{
    return m_loadProgress;
}

bool QOhosWebViewPrivate::isLoading() const
{
    return m_loadStatus == QWebViewLoadingInfo::LoadStatus::Started;
}

void QOhosWebViewPrivate::initialize(QObject *context)
{
    Q_UNUSED(context);
}

void QOhosWebViewPrivate::setCookie(const QString &domain, const QString &name, const QString &value)
{
    Q_UNUSED(domain);
    Q_UNUSED(name);
    Q_UNUSED(value);
}

void QOhosWebViewPrivate::deleteCookie(const QString &domain, const QString &name)
{
    Q_UNUSED(domain);
    Q_UNUSED(name);
}

void QOhosWebViewPrivate::deleteAllCookies()
{
}

QWindow *QOhosWebViewPrivate::nativeWindow() const
{
    return m_webViewWindow.data();
}

void QOhosWebViewPrivate::stop()
{
    m_webViewController->stop();

    QMetaObject::invokeMethod(
        this,
        [this]() {
            emitLoadingChangeSignalAndUpdateLoadStatus(
                QWebViewFactory::LoadingInfo::create(QUrl(m_url), QWebViewLoadingInfo::LoadStatus::Stopped, QString()));
        },
        Qt::QueuedConnection);
}

void QOhosWebViewPrivate::onErrorReceived(const QString &url, const WebResourceError &error)
{
    auto errorMessage =
        QString(QLatin1String("Error %1: '%3'")).arg(error.errorCode()).arg(error.errorInfo().c_str());

    if (m_url != url) {
        qOhosWarning(QtForOhos)
            << Q_FUNC_INFO << "Received onErrorReceive event on other url:" << url << errorMessage;
        return;
    }

    qOhosCritical(QtForOhos) << Q_FUNC_INFO << "Received onErrorReceive for main page:" << errorMessage;

    emitLoadingChangeSignalAndUpdateLoadStatus(
        QWebViewFactory::LoadingInfo::create(QUrl(m_url), QWebViewLoadingInfo::LoadStatus::Failed, errorMessage));
}

void QOhosWebViewPrivate::onPageBegan(const QString &url)
{
    m_url = url;

    Q_EMIT q_ptr->urlChanged(QUrl(url));
    emitLoadingChangeSignalAndUpdateLoadStatus(
        QWebViewFactory::LoadingInfo::create(QUrl(url), QWebViewLoadingInfo::LoadStatus::Started, QString()));
    onProgressChanged(0);
}

void QOhosWebViewPrivate::onPageEnded(const QString &url)
{
    if (m_url != url) {
        qOhosWarning(QtForOhos) << Q_FUNC_INFO << "Received onPageEnd event from other url:" << url;
        return;
    }

    emitLoadingChangeSignalAndUpdateLoadStatus(
        QWebViewFactory::LoadingInfo::create(QUrl(url), QWebViewLoadingInfo::LoadStatus::Succeeded, QString()));
}

void QOhosWebViewPrivate::onProgressChanged(int progress)
{
    if (m_loadProgress == progress) {
        return;
    }

    m_loadProgress = progress;
    Q_EMIT q_ptr->loadProgressChanged(progress);
}

void QOhosWebViewPrivate::onTitleUpdated(const QString &title)
{
    Q_EMIT q_ptr->titleChanged(title);
}

void QOhosWebViewPrivate::emitLoadingChangeSignalAndUpdateLoadStatus(
    const QWebViewLoadingInfo &loadingInfo)
{
    m_loadStatus = loadingInfo.status();
    Q_EMIT q_ptr->loadingChanged(loadingInfo);
}

QOhosWebComponentListenerImpl::QOhosWebComponentListenerImpl(QPointer<QOhosWebViewPrivate> webViewPrivate)
    : QOhosWebComponentListener()
    , m_webViewPrivate(webViewPrivate)
{
}

QOhosWebComponentListenerImpl::~QOhosWebComponentListenerImpl() = default;

void QOhosWebComponentListenerImpl::onErrorReceived(const std::string &url, const WebResourceError &error)
{
    if (m_webViewPrivate)
        m_webViewPrivate->onErrorReceived(QString::fromStdString(url), error);
}

void QOhosWebComponentListenerImpl::onPageBegan(const std::string &url)
{
    if (m_webViewPrivate)
        m_webViewPrivate->onPageBegan(QString::fromStdString(url));
}

void QOhosWebComponentListenerImpl::onPageEnded(const std::string &url)
{
    if (m_webViewPrivate)
        m_webViewPrivate->onPageEnded(QString::fromStdString(url));
}

void QOhosWebComponentListenerImpl::onProgressChanged(int progress)
{
    if (m_webViewPrivate)
        m_webViewPrivate->onProgressChanged(progress);
}

void QOhosWebComponentListenerImpl::onTitleReceived(const std::string &title)
{
    if (m_webViewPrivate)
        m_webViewPrivate->onTitleUpdated(QString::fromStdString(title));
}

}

QWebViewPrivate *makeOhosWebView(QWebView *view)
{
    return new QOhosWebViewPrivate(view);
}

QT_END_NAMESPACE

#include "qohoswebview.moc"
