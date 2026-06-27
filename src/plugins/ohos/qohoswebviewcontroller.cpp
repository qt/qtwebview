// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohoswebresourceerror.h"
#include "qohoswebviewcontroller.h"
#include <QtCore/private/qnapi_p.h>
#include <QtCore/private/qohoslogger_p.h>
#include <QtCore/QByteArray>
#include <QtCore/QMimeDatabase>
#include <QtCore/QResource>
#include <QtCore/QUrl>
#include <arkui/native_node_napi.h>
#include <arkui/native_type.h>
#include <web/arkweb_net_error_list.h>
#include <web/arkweb_scheme_handler.h>
#include <cstdlib>

QT_BEGIN_NAMESPACE

#define QT_OHOS_ARKWEB_CHECK(call) \
    do { \
        const int32_t arkwebRc_ = (call); \
        if (arkwebRc_ != ARKWEB_NET_OK) \
            qOhosPrintfError("%s: %s failed with error: %d", \
                             Q_FUNC_INFO, #call, arkwebRc_); \
    } while (false)

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

    std::optional<std::string> tryRunJavaScript(const std::string &script) override;

    void setAttribute(QWebViewSettings::WebAttribute attribute, bool enabled) override;
    bool testAttribute(QWebViewSettings::WebAttribute attribute) const override;

    bool trySetCookie(const std::string &url, const std::string &cookie) override;
    std::optional<std::string> tryFetchCookie(const std::string &url) override;
    bool tryClearAllCookies() override;
    std::vector<std::pair<std::string, std::string>> fetchAllCookies() override;

    void bindQrcSchemeHandler() override;

    bool trySetCustomUserAgent(const std::string &userAgent) override;

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
    std::shared_ptr<void> m_qrcSchemeHandlerBinding;
    std::string m_webTag;
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

std::optional<std::string> tryGetResourceRequestUrl(const ArkWeb_ResourceRequest *resourceRequest)
{
    char *rawUrl = nullptr;
    ::OH_ArkWebResourceRequest_GetUrl(resourceRequest, &rawUrl);
    if (!rawUrl) {
        qOhosPrintfError("%s: OH_ArkWebResourceRequest_GetUrl returned no URL", Q_FUNC_INFO);
        return std::nullopt;
    }
    std::string url = rawUrl;
    ::OH_ArkWeb_ReleaseString(rawUrl);
    return url;
}

std::shared_ptr<::ArkWeb_Response> createArkWebResponseOrNull()
{
    ArkWeb_Response *response = nullptr;
    ::OH_ArkWeb_CreateResponse(&response);
    if (!response) {
        qOhosPrintfError("%s: OH_ArkWeb_CreateResponse failed", Q_FUNC_INFO);
        return nullptr;
    }
    return std::shared_ptr<::ArkWeb_Response>(response, ::OH_ArkWeb_DestroyResponse);
}

void onQrcRequestStart(const ArkWeb_SchemeHandler *,
                       ArkWeb_ResourceRequest *resourceRequest,
                       const ArkWeb_ResourceHandler *resourceHandler,
                       bool *intercept)
{
    *intercept = true;
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceRequest_SetUserData(
        resourceRequest, const_cast<ArkWeb_ResourceHandler *>(resourceHandler)));

    auto optRequestUrl = tryGetResourceRequestUrl(resourceRequest);
    if (!optRequestUrl) {
        QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceHandler_DidFailWithErrorV2(
            resourceHandler, ARKWEB_ERR_FAILED, true));
        return;
    }
    const QUrl url(QString::fromUtf8(*optRequestUrl));

    // qrc:/path/file  ->  ":/path/file"
    QResource resource(QStringLiteral(":") + url.path());
    if (!resource.isValid()) {
        QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceHandler_DidFailWithErrorV2(
            resourceHandler, ARKWEB_ERR_FILE_NOT_FOUND, true));
        return;
    }

    const QByteArray body = resource.uncompressedData();
    const QByteArray mimeType =
        QMimeDatabase().mimeTypeForFileNameAndData(url.fileName(), body).name().toUtf8();

    const std::shared_ptr<::ArkWeb_Response> response = createArkWebResponseOrNull();
    if (!response) {
        QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceHandler_DidFailWithErrorV2(
            resourceHandler, ARKWEB_ERR_FAILED, true));
        return;
    }
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResponse_SetStatus(response.get(), 200));
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResponse_SetMimeType(response.get(), mimeType.constData()));
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResponse_SetCharset(response.get(), "UTF-8"));
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResponse_SetHeaderByName(
        response.get(), "content-length", QByteArray::number(body.size()).constData(), false));

    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceHandler_DidReceiveResponse(resourceHandler, response.get()));
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceHandler_DidReceiveData(
        resourceHandler, reinterpret_cast<const uint8_t *>(body.constData()), body.size()));
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceHandler_DidFinish(resourceHandler));
}

void onQrcRequestStop(const ArkWeb_SchemeHandler *,
                      const ArkWeb_ResourceRequest *resourceRequest)
{
    auto *resourceHandler = static_cast<ArkWeb_ResourceHandler *>(
        ::OH_ArkWebResourceRequest_GetUserData(resourceRequest));
    if (resourceHandler)
        QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceHandler_Destroy(resourceHandler));
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebResourceRequest_Destroy(resourceRequest));
}

std::shared_ptr<::ArkWeb_SchemeHandler> createQrcSchemeHandlerOrNull()
{
    ArkWeb_SchemeHandler *schemeHandler = nullptr;
    ::OH_ArkWeb_CreateSchemeHandler(&schemeHandler);
    if (!schemeHandler) {
        qOhosPrintfError("%s: OH_ArkWeb_CreateSchemeHandler failed", Q_FUNC_INFO);
        return nullptr;
    }
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebSchemeHandler_SetOnRequestStart(schemeHandler, onQrcRequestStart));
    QT_OHOS_ARKWEB_CHECK(::OH_ArkWebSchemeHandler_SetOnRequestStop(schemeHandler, onQrcRequestStop));
    return std::shared_ptr<::ArkWeb_SchemeHandler>(schemeHandler, ::OH_ArkWeb_DestroySchemeHandler);
}

std::shared_ptr<::ArkWeb_SchemeHandler> sharedQrcSchemeHandlerOrNull()
{
    static std::weak_ptr<::ArkWeb_SchemeHandler> weakHandler;
    auto handler = weakHandler.lock();
    if (!handler) {
        handler = createQrcSchemeHandlerOrNull();
        weakHandler = handler;
    }
    return handler;
}

std::string makeUniqueWebViewTag()
{
    static unsigned s_webTagCounter = 0;
    return "qtwebview_" + std::to_string(s_webTagCounter++);
}

QOhosWebViewControllerImpl::QOhosWebViewControllerImpl()
    : QOhosWebViewController()
    , m_webTag(makeUniqueWebViewTag())
{
    m_jsScopeData = QOhosJsThreadGateway::eval(
        [&](QOhosJsState &jsState) {
            return QtOhos::makeProxyWithJsThreadDeleter(
                QtOhos::moveToSharedPtr(
                    JsScopeData{
                        .jsWebViewController = QNapi::Reference<>::makePersistentFrom(
                            jsState.eval<QNapi::Object>("@ohos.web.webview.WebviewController<new>(*)", {m_webTag})),
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
            m_jsScopeData->jsWebViewController.eval("loadUrl(*)", {url});
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
        return m_jsScopeData->jsWebViewController.eval<QNapi::String>("getUrl()").Utf8Value();
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
                m_jsScopeData->jsWebViewController.eval("loadData(*)", {data, mimeType, encoding});
            else
                m_jsScopeData->jsWebViewController.eval("loadData(*)", {data, mimeType, encoding, baseUrl, historyUrl});
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
        return m_jsScopeData->jsWebViewController.eval<QNapi::Boolean>("accessBackward()").Value();
    });
}

bool QOhosWebViewControllerImpl::canGoForward()
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &) {
        return m_jsScopeData->jsWebViewController.eval<QNapi::Boolean>("accessForward()").Value();
    });
}

void QOhosWebViewControllerImpl::goBack()
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &) {
        m_jsScopeData->jsWebViewController.eval("backward()");
    });
}

void QOhosWebViewControllerImpl::goForward()
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &) {
        m_jsScopeData->jsWebViewController.eval("forward()");
    });
}

void QOhosWebViewControllerImpl::refresh()
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &) {
        m_jsScopeData->jsWebViewController.eval("refresh()");
    });
}

void QOhosWebViewControllerImpl::stop()
{
    QOhosJsThreadGateway::runAndWait([&](QOhosJsState &) {
        m_jsScopeData->jsWebViewController.eval("stop()");
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

bool QOhosWebViewControllerImpl::trySetCookie(const std::string &url, const std::string &cookie)
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &jsState) {
        try {
            jsState.eval("@ohos.web.webview.WebCookieManager.configCookieSync(*)", {url, cookie});
            return true;
        } catch (const Napi::Error &error) {
            qOhosPrintfError(
                "%s: WebCookieManager.configCookieSync() failed for '%s': %s",
                Q_FUNC_INFO, url.c_str(), error.what());
            return false;
        }
    });
}

std::optional<std::string> QOhosWebViewControllerImpl::tryFetchCookie(const std::string &url)
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &jsState) -> std::optional<std::string> {
        try {
            const std::string cookie = jsState.eval<QNapi::String>(
                "@ohos.web.webview.WebCookieManager.fetchCookieSync(*)", {url});
            return cookie;
        } catch (const Napi::Error &error) {
            qOhosPrintfError(
                "%s: WebCookieManager.fetchCookieSync('%s') failed: %s",
                Q_FUNC_INFO, url.c_str(), error.what());
            return {};
        }
    });
}

bool QOhosWebViewControllerImpl::tryClearAllCookies()
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &jsState) {
        try {
            jsState.eval("@ohos.web.webview.WebCookieManager.clearAllCookiesSync()");
            return true;
        } catch (const Napi::Error &error) {
            qOhosPrintfError(
                "%s: WebCookieManager.clearAllCookiesSync() failed: %s", Q_FUNC_INFO, error.what());
            return false;
        }
    });
}

std::vector<std::pair<std::string, std::string>> QOhosWebViewControllerImpl::fetchAllCookies()
{
    using CookieList = std::vector<std::pair<std::string, std::string>>;
    return QOhosJsThreadGateway::evalWithConsumer<CookieList>(
        [&](QOhosJsState &jsState, auto cookieListConsumer) {
            constexpr bool incognitoMode = false;
            jsState.evalToPromiseOrRejectOnThrow(
                "@ohos.web.webview.WebCookieManager.fetchAllCookies(*)", {incognitoMode})
                .withContext(std::move(cookieListConsumer))
                .onThenWithContext(
                    [](const QOhosCallbackInfo &cbInfo, auto &cookieListConsumer) {
                        auto cookies = cbInfo.getFirstArg<QNapi::Array>(Q_FUNC_INFO);
                        cookieListConsumer(
                            QNapi::getArrayElements<CookieList, QNapi::Object>(
                                cookies,
                                [](const QNapi::Object &cookie) -> std::pair<std::string, std::string> {
                                    return {
                                        cookie.get<QNapi::String>("domain"),
                                        cookie.get<QNapi::String>("name")
                                    };
                                }));
                    })
                .onCatchWithContext(
                    [](const QOhosCallbackInfo &cbInfo, auto &cookieListConsumer) {
                        QtOhos::logJsCallbackError(
                            cbInfo, "@ohos.web.webview.WebCookieManager.fetchAllCookies() failed");
                        cookieListConsumer({});
                    });
        });
}

void QOhosWebViewControllerImpl::bindQrcSchemeHandler()
{
    m_qrcSchemeHandlerBinding = QOhosJsThreadGateway::eval(
        [&](QOhosJsState &) -> std::shared_ptr<void> {
            auto schemeHandler = sharedQrcSchemeHandlerOrNull();
            if (!schemeHandler)
                return nullptr;

            if (!::OH_ArkWeb_SetSchemeHandler("qrc", m_webTag.c_str(), schemeHandler.get())) {
                qOhosPrintfError(
                    "%s: OH_ArkWeb_SetSchemeHandler(\"qrc\", \"%s\") failed.",
                    Q_FUNC_INFO, m_webTag.c_str());
                return nullptr;
            }

            return QtOhos::makeProxyWithJsThreadDeleter(
                QtOhos::makeDestroyNotifier(
                    [webTag = m_webTag, schemeHandler] {
                        if (::OH_ArkWeb_ClearSchemeHandlers(webTag.c_str()) != 0)
                            qOhosPrintfError(
                                "%s: OH_ArkWeb_ClearSchemeHandlers(\"%s\") failed.",
                                Q_FUNC_INFO, webTag.c_str());
                    }));
        });
}

bool QOhosWebViewControllerImpl::trySetCustomUserAgent(const std::string &userAgent)
{
    return QOhosJsThreadGateway::eval([&](QOhosJsState &) {
        try {
            m_jsScopeData->jsWebViewController.eval("setCustomUserAgent(*)", {userAgent});
            return true;
        } catch (const Napi::Error &error) {
            qOhosPrintfError("%s: setCustomUserAgent() failed: %s", Q_FUNC_INFO, error.what());
            return false;
        }
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
                    auto isRealTitleValue =
                        QNapi::getOptionalPropOrEmpty<QNapi::Boolean>(onTitleReceivedEvent, "isRealTitle");
                    webComponentListener->onTitleReceived(
                        onTitleReceivedEvent.get<QNapi::String>("title"),
                        !isRealTitleValue.IsEmpty() && isRealTitleValue.Value());
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

void initializeOhosWebEngine()
{
    QOhosJsThreadGateway::runAndWait([](QOhosJsState &jsState) {
        const int32_t opt = ARKWEB_SCHEME_OPTION_STANDARD | ARKWEB_SCHEME_OPTION_SECURE
                            | ARKWEB_SCHEME_OPTION_CORS_ENABLED | ARKWEB_SCHEME_OPTION_FETCH_ENABLED;
        if (::OH_ArkWeb_RegisterCustomSchemes("qrc", opt) != 0) {
            qOhosPrintfError("%s: RegisterCustomSchemes(\"qrc\") failed", Q_FUNC_INFO);
            return;
        }
        jsState.eval("@ohos.web.webview.WebviewController.initializeWebEngine()");
    });
}

QT_END_NAMESPACE
