// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohoswebresourceerror.h"
#include "qohoswebviewcontroller.h"
#include <QtCore/private/qnapi_p.h>
#include <QtCore/private/qohoslogger_p.h>
#include <arkui/native_node_napi.h>
#include <arkui/native_type.h>
#include <cstdlib>

QT_BEGIN_NAMESPACE

namespace {

class QOhosWebViewControllerImpl : public QOhosWebViewController
{
public:
    QOhosWebViewControllerImpl();

    ::ArkUI_NodeHandle createEmbeddedWebComponentNodeOrFail(
        std::shared_ptr<QOhosWebComponentListener> webComponentListener,
        QObject *webComponentListenerContext) override;

    bool tryLoadUrl(const std::string &url) override;
    std::string getUrl() override;

    bool tryLoadHtml(const std::string &data, const std::string &mimeType,
                     const std::string &encoding, const std::string &baseUrl,
                     const std::string &historyUrl) override;

    bool canGoBack() override;
    bool canGoForward() override;

    void goBack() override;
    void goForward() override;

    void refresh() override;
    void stop() override;

    std::string getTitle() override;

    std::optional<std::string> tryRunJavaScript(const std::string &script) override;

    void setAttribute(QWebViewSettings::WebAttribute attribute, bool enabled) override;
    bool testAttribute(QWebViewSettings::WebAttribute attribute) const override;

private:
    using WebAttribute = QWebViewSettings::WebAttribute;

    QNapi::Object makeWebComponentAttributes(
        QOhosJsState &jsState,
        const std::shared_ptr<QOhosWebComponentListener> &webComponentListener) const;
    void applyUniversalAccessPath(QOhosJsState &jsState);
    void setPathAllowingUniversalAccess(bool enabled);
    void updateWebAttribute(WebAttribute attribute, bool enabled);

    struct JsScopeData
    {
        QNapi::Reference<QNapi::Object> jsWebViewController;
        QNapi::Reference<QNapi::Object> jsComponentContent;

        QHash<WebAttribute, bool> attributes = {
            {WebAttribute::LocalStorageEnabled, false},
            {WebAttribute::JavaScriptEnabled, true},
            {WebAttribute::AllowFileAccess, false},
            {WebAttribute::LocalContentCanAccessFileUrls, false},
        };
        std::string universalAccessPath;
    };

    std::shared_ptr<JsScopeData> m_jsScopeData;
    std::shared_ptr<QOhosWebComponentListener> m_webComponentListener;
};

QNapi::Object createEmbeddedWebComponent(
    QOhosJsState &jsState, QNapi::Object parentAbilityWindowStage,
    QNapi::Object webComponentAttributes)
{
    return jsState.eval<QNapi::Object>(
        "QEmbeddedComponentCreator.makeNewQEmbeddedComponentCreator().createEmbeddedWebComponent(*)",
        {
            parentAbilityWindowStage.eval<QNapi::Object>("getMainWindowSync().getUIContext()"),
            webComponentAttributes
        });
}

std::string localDirectoryForTarget(const std::string &target)
{
    if (target.empty())
        return {};

    std::string path = target;

    const std::string filePrefix = "file://";
    if (path.compare(0, filePrefix.size(), filePrefix) == 0)
        path.erase(0, filePrefix.size());

    if (path.empty() || path.front() != '/')
        return {};

    if (path.back() == '/')
        path.pop_back();
    else
        path.erase(path.find_last_of('/'));

    if (path.empty()) // the document sat at the filesystem root, e.g. "/index.html"
        path = "/";

    return path;
}

QOhosWebViewControllerImpl::QOhosWebViewControllerImpl()
    : QOhosWebViewController()
{
    m_jsScopeData = QOhosJsThreadGateway::eval(
        [](QOhosJsState &jsState) {
            return QtOhos::makeProxyWithJsThreadDeleter(
                QtOhos::moveToSharedPtr(
                    JsScopeData{
                        .jsWebViewController = QNapi::Reference<>::makePersistentFrom(
                            jsState.eval<QNapi::Object>("@ohos.web.webview.WebviewController<new>()")),
                    }));
        });
}

::ArkUI_NodeHandle QOhosWebViewControllerImpl::createEmbeddedWebComponentNodeOrFail(
    std::shared_ptr<QOhosWebComponentListener> webComponentListener,
    QObject *webComponentListenerContext)
{
    m_webComponentListener =
        makeOhosWebComponentListenerExecutingInContextThread(
            webComponentListener, webComponentListenerContext);

    return QOhosJsThreadGateway::eval([&](QOhosJsState &jsState) {
        auto optWindowStage = jsState.defaultWindowStageOrEmpty();
        if (optWindowStage.IsEmpty()) {
            qFatal("Attempting to create Web component without WindowStage object available, which is not supported. Aborting...");
            std::abort();
        }

        auto embeddedWebComponent = createEmbeddedWebComponent(
            jsState, optWindowStage,
            makeWebComponentAttributes(jsState, m_webComponentListener));

        m_jsScopeData->jsComponentContent =
            QNapi::Reference<>::makePersistentFrom(embeddedWebComponent);

        ::ArkUI_NodeHandle handle = nullptr;
        auto getNodeFromNapiValueResult
            = ::OH_ArkUI_GetNodeHandleFromNapiValue(jsState.env(), embeddedWebComponent, &handle);
        if (getNodeFromNapiValueResult != ::ARKUI_ERROR_CODE_NO_ERROR || handle == nullptr) {
            qFatal(
                "Failed to get node handle from webview component, reason: %i, Aborting...",
                getNodeFromNapiValueResult);
            std::abort();
        }

        return handle;
    });
}

bool QOhosWebViewControllerImpl::tryLoadUrl(const std::string &url)
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &jsState) {
        m_jsScopeData->universalAccessPath = localDirectoryForTarget(url);
        applyUniversalAccessPath(jsState);
        try {
            m_jsScopeData->jsWebViewController.call("loadUrl", {url});
            return true;
        } catch (const Napi::Error &error) {
            qOhosPrintfError(
                "%s received error from WebController.loadUrl('%s'): %s",
                Q_FUNC_INFO, url.c_str(), error.what());
            return false;
        }
    });
}

std::string QOhosWebViewControllerImpl::getUrl()
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &) {
        return m_jsScopeData->jsWebViewController.call<QNapi::String>("getUrl").Utf8Value();
    });
}

bool QOhosWebViewControllerImpl::tryLoadHtml(const std::string &data, const std::string &mimeType,
                                             const std::string &encoding, const std::string &baseUrl,
                                             const std::string &historyUrl)
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &jsState) {
        m_jsScopeData->universalAccessPath = localDirectoryForTarget(baseUrl);
        applyUniversalAccessPath(jsState);
        try {
            if (baseUrl.empty())
                m_jsScopeData->jsWebViewController.call("loadData", {data, mimeType, encoding});
            else
                m_jsScopeData->jsWebViewController.call("loadData", {data, mimeType, encoding, baseUrl, historyUrl});
            return true;
        } catch (const Napi::Error &error) {
            qOhosPrintfError(
                "%s received error from WebController.loadData: %s", Q_FUNC_INFO, error.what());
            return false;
        }
    });
}

bool QOhosWebViewControllerImpl::canGoBack()
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &) {
        return m_jsScopeData->jsWebViewController.call<QNapi::Boolean>("accessBackward").Value();
    });
}

bool QOhosWebViewControllerImpl::canGoForward()
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &) {
        return m_jsScopeData->jsWebViewController.call<QNapi::Boolean>("accessForward").Value();
    });
}

void QOhosWebViewControllerImpl::goBack()
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &) {
        m_jsScopeData->jsWebViewController.call("backward");
    });
}

void QOhosWebViewControllerImpl::goForward()
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &) {
        m_jsScopeData->jsWebViewController.call("forward");
    });
}

void QOhosWebViewControllerImpl::refresh()
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &) {
        m_jsScopeData->jsWebViewController.call("refresh");
    });
}

void QOhosWebViewControllerImpl::stop()
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &) {
        m_jsScopeData->jsWebViewController.call("stop");
    });
}

std::string QOhosWebViewControllerImpl::getTitle()
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &) {
        return m_jsScopeData->jsWebViewController.call<QNapi::String>("getTitle").Utf8Value();
    });
}

std::optional<std::string> QOhosWebViewControllerImpl::tryRunJavaScript(const std::string &script)
{
    return QOhosJsThreadGateway::evalWithConsumer<std::optional<std::string>>(
        [&](QOhosJsState &, auto javaScriptResultConsumer) {
            m_jsScopeData->jsWebViewController.evalToPromiseOrRejectOnThrow("runJavaScript(*)", {script})
            .withContext(std::move(javaScriptResultConsumer))
            .onThenWithContext(
                [](const QOhosCallbackInfo &cbInfo, auto &javaScriptResultConsumer) {
                    std::string javaScriptResult = cbInfo.getFirstArg<QNapi::String>(Q_FUNC_INFO);
                    javaScriptResultConsumer(javaScriptResult);
                })
            .onCatchWithContext(
                [](const QOhosCallbackInfo &cbInfo, auto &javaScriptResultConsumer) {
                    QtOhos::logJsCallbackError(cbInfo, "@ohos.web.webview.WebviewController.runJavaScript() failed");
                    javaScriptResultConsumer({});
                });
        });
}

void QOhosWebViewControllerImpl::setAttribute(WebAttribute attribute, bool enabled)
{
    if (attribute == WebAttribute::LocalContentCanAccessFileUrls) {
        setPathAllowingUniversalAccess(enabled);
        return;
    }
    updateWebAttribute(attribute, enabled);
}

bool QOhosWebViewControllerImpl::testAttribute(WebAttribute attribute) const
{
    return QOhosJsThreadGateway::eval(
        [&](QOhosJsState &) {
            return m_jsScopeData->attributes.value(attribute, false);
        });
}

QNapi::Object QOhosWebViewControllerImpl::makeWebComponentAttributes(
    QOhosJsState &jsState,
    const std::shared_ptr<QOhosWebComponentListener> &webComponentListener) const
{
    return QNapi::makeObject(
        jsState.env(),
        {
            {"webviewController", m_jsScopeData->jsWebViewController.Value()},
            {"domStorageAccess", m_jsScopeData->attributes.value(WebAttribute::LocalStorageEnabled, false)},
            {"javaScriptAccess", m_jsScopeData->attributes.value(WebAttribute::JavaScriptEnabled, false)},
            {"fileAccess", m_jsScopeData->attributes.value(WebAttribute::AllowFileAccess, false)},
            {
                "onErrorReceive",
                [webComponentListener](const QOhosCallbackInfo &callbackInfo) {
                    auto onErrorReceiveEvent = callbackInfo.getFirstArg<QNapi::Object>("onErrorReceive");
                    std::string url = onErrorReceiveEvent.eval<QNapi::String>("request.getRequestUrl()");
                    auto error = onErrorReceiveEvent.get<QNapi::Object>("error");
                    webComponentListener->onErrorReceived(
                        url, WebResourceError::makeFromJsObject(error));
                }
            },
            {
                "onPageBegin",
                [webComponentListener](const QNapi::CallbackInfo &callbackInfo) {
                    auto onPageBeginEvent = callbackInfo.getFirstArg<QNapi::Object>("onPageBegin");
                    webComponentListener->onPageBegan(
                        onPageBeginEvent.get<QNapi::String>("url"));
                }
            },
            {
                "onPageEnd",
                [webComponentListener](const QNapi::CallbackInfo &callbackInfo) {
                    auto onPageEndEvent = callbackInfo.getFirstArg<QNapi::Object>("onPageEnd");
                    webComponentListener->onPageEnded(
                        onPageEndEvent.get<QNapi::String>("url"));
                }
            },
            {
                "onProgressChange",
                [webComponentListener](const QNapi::CallbackInfo &callbackInfo) {
                    auto onProgressChangeEvent = callbackInfo.getFirstArg<QNapi::Object>("onProgressChange");
                    webComponentListener->onProgressChanged(
                        onProgressChangeEvent.get<QNapi::Number>("newProgress"));
                }
            },
            {
                "onTitleReceive",
                [webComponentListener](const QNapi::CallbackInfo &callbackInfo) {
                    auto onTitleReceivedEvent = callbackInfo.getFirstArg<QNapi::Object>("onTitleReceive");
                    webComponentListener->onTitleReceived(
                        onTitleReceivedEvent.get<QNapi::String>("title"));
                }
            },
        });
}

void QOhosWebViewControllerImpl::applyUniversalAccessPath(QOhosJsState &jsState)
{
    if (!m_jsScopeData->jsComponentContent)
        return;

    std::vector<std::string> paths;
    if (m_jsScopeData->attributes[WebAttribute::LocalContentCanAccessFileUrls] && !m_jsScopeData->universalAccessPath.empty())
        paths.push_back(m_jsScopeData->universalAccessPath);

    try {
        m_jsScopeData->jsWebViewController.eval(
            "setPathAllowingUniversalAccess(*)",
            {QNapi::makeArray(jsState.env(), paths)});
    } catch (const Napi::Error &error) {
        qOhosPrintfError(
            "%s: setPathAllowingUniversalAccess failed (paths must live under "
            "filesDir or resourceDir, otherwise error 401): %s",
            Q_FUNC_INFO, error.what());
    }
}

void QOhosWebViewControllerImpl::setPathAllowingUniversalAccess(bool enabled)
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &jsState) {
        m_jsScopeData->attributes[WebAttribute::LocalContentCanAccessFileUrls] = enabled;
        applyUniversalAccessPath(jsState);
    });
}

void QOhosWebViewControllerImpl::updateWebAttribute(WebAttribute attribute, bool enabled)
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &jsState) {
        m_jsScopeData->attributes[attribute] = enabled;

        if (!m_jsScopeData->jsComponentContent)
            return;

        m_jsScopeData->jsComponentContent.eval(
            "update(*)", {makeWebComponentAttributes(jsState, m_webComponentListener)});
    });
}

}

QOhosWebViewController::QOhosWebViewController() = default;

QOhosWebViewController::~QOhosWebViewController() = default;

std::shared_ptr<QOhosWebViewController> makeOhosWebViewController()
{
    return std::make_shared<QOhosWebViewControllerImpl>();
}

QT_END_NAMESPACE
