// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include "qwebenginewebview_p.h"
#include <QtWebView/qwebview.h>
#include <QtWebView/qwebviewloadinginfo.h>
#include <QtWebViewQuick/private/qquickwebview_p.h>

#include <QtCore/qmap.h>
#include <QtGui/qguiapplication.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qurl.h>

#include <QtQml/qqml.h>

#include <QtQuick/qquickwindow.h>
#include <QtQuick/qquickview.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/private/qquickitem_p.h>

#include <QtWebEngineQuick/private/qquickwebengineview_p.h>
#include <QtWebEngineCore/qwebengineloadinginfo.h>

#include <QWebEngineCookieStore>
#include <QNetworkCookie>

QT_BEGIN_NAMESPACE

using namespace Qt::StringLiterals;

QWebEngineWebViewPrivate::QWebEngineWebViewPrivate(QWebView *p)
    : QWebViewPrivate(p),
      m_profile(nullptr),
      m_parentItem(nullptr),
      m_window(qobject_cast<QQuickWindow *>(p->parent())),
      m_ownsWindow(!m_window)
{
    m_settings = new QWebEngineWebViewSettingsPrivate(this);
    m_webEngineView.m_parent = this;
    m_cookieStore.m_webEngineViewPtr = &m_webEngineView;
    if (m_ownsWindow) {
        m_window = new QQuickView(p);
        connect(p, &QWindow::visibleChanged, m_window, &QWindow::setVisible);
        connect(p, &QWindow::widthChanged, m_window, &QWindow::setWidth);
        connect(p, &QWindow::heightChanged, m_window, &QWindow::setHeight);
    }
}

QWebEngineWebViewPrivate::~QWebEngineWebViewPrivate()
{
    if (m_ownsWindow) {
        // quickview destructor deletes also webengieview item
        m_webEngineView.m_webEngineView.release();
        delete m_window;
    }
}

void QWebEngineWebViewPrivate::initialize(QObject *context)
{
    m_parentItem = qobject_cast<QQuickItem *>(context);
}

QString QWebEngineWebViewPrivate::httpUserAgent() const
{
    return m_httpUserAgent;
}

void QWebEngineWebViewPrivate::setHttpUserAgent(const QString &userAgent)
{
    m_httpUserAgent = userAgent;
    if (m_profile) {
        m_profile->setHttpUserAgent(userAgent);
        emit q_ptr->httpUserAgentChanged(userAgent);
    }
}

QUrl QWebEngineWebViewPrivate::url() const
{
    return m_webEngineView->url();
}

void QWebEngineWebViewPrivate::setUrl(const QUrl &url)
{
    m_webEngineView->setUrl(url);
}

void QWebEngineWebViewPrivate::loadHtml(const QString &html, const QUrl &baseUrl)
{
    m_webEngineView->loadHtml(html, baseUrl);
}

bool QWebEngineWebViewPrivate::canGoBack() const
{
    return m_webEngineView->canGoBack();
}

void QWebEngineWebViewPrivate::goBack()
{
    m_webEngineView->goBack();
}

bool QWebEngineWebViewPrivate::canGoForward() const
{
    return m_webEngineView->canGoForward();
}

void QWebEngineWebViewPrivate::goForward()
{
    m_webEngineView->goForward();
}

void QWebEngineWebViewPrivate::reload()
{
    m_webEngineView->reload();
}

QString QWebEngineWebViewPrivate::title() const
{
    return m_webEngineView->title();
}

void QWebEngineWebViewPrivate::runJavaScript(
        const QString &script, const std::function<void(const QVariant &)> &resultCallback)
{
    m_webEngineView->runJavaScript(script, resultCallback);
}

void QWebEngineWebViewPrivate::setCookie(const QString &domain, const QString &name, const QString &value)
{
    QNetworkCookie cookie;
    cookie.setDomain(domain);
    cookie.setName(QByteArray(name.toUtf8()));
    cookie.setValue(QByteArray(value.toUtf8()));
    cookie.setPath("/");

    m_cookieStore->setCookie(cookie);
}

void QWebEngineWebViewPrivate::deleteCookie(const QString &domain, const QString &name)
{
    QNetworkCookie cookie;
    cookie.setDomain(domain);
    cookie.setName(QByteArray(name.toUtf8()));
    cookie.setPath("/");

    m_cookieStore->deleteCookie(cookie);
}

void QWebEngineWebViewPrivate::deleteAllCookies()
{
    m_cookieStore->deleteAllCookies();
}

QWebViewSettingsPrivate *QWebEngineWebViewPrivate::settings() const
{
    return m_settings;
}

int QWebEngineWebViewPrivate::loadProgress() const
{
    return m_webEngineView->loadProgress();
}

bool QWebEngineWebViewPrivate::isLoading() const
{
    return m_webEngineView->isLoading();
}

void QWebEngineWebViewPrivate::stop()
{
    m_webEngineView->stop();
}

void QWebEngineWebViewPrivate::q_urlChanged()
{
    emit q_ptr->urlChanged(m_webEngineView->url());
}

void QWebEngineWebViewPrivate::q_loadProgressChanged()
{
    emit q_ptr->loadProgressChanged(m_webEngineView->loadProgress());
}

void QWebEngineWebViewPrivate::q_titleChanged()
{
    emit q_ptr->titleChanged(m_webEngineView->title());
}

void QWebEngineWebViewPrivate::q_loadingChanged(const QWebEngineLoadingInfo &loadRequest)
{
    QWebViewLoadingInfo lr(loadRequest.url(),
                           static_cast<QWebViewLoadingInfo::LoadStatus>(
                                   loadRequest.status()), // These "should" match...
                           loadRequest.errorString());
    emit q_ptr->loadingChanged(lr);
}

void QWebEngineWebViewPrivate::q_profileChanged()
{
    auto profile = m_webEngineView->profile();
    if (profile == m_profile)
        return;

    m_profile = profile;
    auto userAgent = m_profile->httpUserAgent();
    if (m_httpUserAgent == userAgent)
        return;
    m_httpUserAgent = userAgent;
    QObject::connect(m_profile, &QQuickWebEngineProfile::httpUserAgentChanged, this, &QWebEngineWebViewPrivate::q_httpUserAgentChanged);
    emit q_ptr->httpUserAgentChanged(userAgent);
}

void QWebEngineWebViewPrivate::q_httpUserAgentChanged()
{
    QString httpUserAgent = m_profile->httpUserAgent();
    if (m_httpUserAgent == httpUserAgent)
        return;
     m_httpUserAgent = httpUserAgent;
     emit q_ptr->httpUserAgentChanged(m_httpUserAgent);
}

void QWebEngineWebViewPrivate::q_cookieAdded(const QNetworkCookie &cookie)
{
    emit q_ptr->cookieAdded(cookie.domain(), cookie.name());
}

void QWebEngineWebViewPrivate::q_cookieRemoved(const QNetworkCookie &cookie)
{
    emit q_ptr->cookieRemoved(cookie.domain(), cookie.name());
}

void QWebEngineWebViewPrivate::QQuickWebEngineViewPtr::init() const
{
    Q_ASSERT(!m_webEngineView);
    Q_ASSERT(m_parent->m_window);

    QQuickWebEngineView *webEngineView = nullptr;
    QQuickItem *parentItem = m_parent->m_parentItem;

    if (!parentItem) {
        // this is non qquickwebview initialization
        // set the content for qquickview
        QQuickView *view = qobject_cast<QQuickView *>(m_parent->m_window);
        Q_ASSERT(view);
        view->setResizeMode(QQuickView::SizeRootObjectToView);
        view->loadFromModule("QtWebEngine"_L1, "WebEngineView"_L1);
        webEngineView = qobject_cast<QQuickWebEngineView *>(view->rootObject());
        Q_ASSERT(webEngineView);
    } else {
        QQmlEngine *engine = qmlEngine(parentItem);
        Q_ASSERT(engine);
        QQmlComponent component(engine);
        component.loadFromModule("QtWebEngine"_L1, "WebEngineView"_L1);
        webEngineView = qobject_cast<QQuickWebEngineView *>(component.create());
        Q_ASSERT(webEngineView);
        webEngineView->setParentItem(parentItem);
        QQuickItemPrivate::get(webEngineView)->anchors()->setFill(parentItem);
    }

    QQuickWebEngineProfile *profile = webEngineView->profile();
    Q_ASSERT(profile);
    QQuickWebEngineSettings *settings = webEngineView->settings();
    Q_ASSERT(settings);
    m_parent->m_profile = profile;
    if (!m_parent->m_settings)
        m_parent->m_settings = new QWebEngineWebViewSettingsPrivate(m_parent);
    m_parent->m_settings->init(settings);
    webEngineView->settings()->setErrorPageEnabled(false);
    webEngineView->settings()->setPluginsEnabled(true);
    // When the httpUserAgent is set as a property then it will be set before
    // init() is called
    if (m_parent->m_httpUserAgent.isEmpty())
        m_parent->m_httpUserAgent = profile->httpUserAgent();
    else
        profile->setHttpUserAgent(m_parent->m_httpUserAgent);
    QObject::connect(webEngineView, &QQuickWebEngineView::urlChanged, m_parent, &QWebEngineWebViewPrivate::q_urlChanged);
    QObject::connect(webEngineView, &QQuickWebEngineView::loadProgressChanged, m_parent, &QWebEngineWebViewPrivate::q_loadProgressChanged);
    QObject::connect(webEngineView, &QQuickWebEngineView::loadingChanged, m_parent, &QWebEngineWebViewPrivate::q_loadingChanged);
    QObject::connect(webEngineView, &QQuickWebEngineView::titleChanged, m_parent, &QWebEngineWebViewPrivate::q_titleChanged);
    QObject::connect(webEngineView, &QQuickWebEngineView::profileChanged,m_parent, &QWebEngineWebViewPrivate::q_profileChanged);
    QObject::connect(profile, &QQuickWebEngineProfile::httpUserAgentChanged, m_parent, &QWebEngineWebViewPrivate::q_httpUserAgentChanged);

    m_webEngineView.reset(webEngineView);

    if (!m_parent->m_cookieStore.m_cookieStore)
        m_parent->m_cookieStore.init();
}

void QWebEngineWebViewPrivate::QWebEngineCookieStorePtr::init() const
{
    if (!m_webEngineViewPtr->m_webEngineView)
        m_webEngineViewPtr->init();
    else {
        QWebEngineWebViewPrivate * parent = m_webEngineViewPtr->m_parent;
        QWebEngineCookieStore *cookieStore = parent->m_profile->cookieStore();
        m_cookieStore = cookieStore;

        QObject::connect(cookieStore, &QWebEngineCookieStore::cookieAdded, parent, &QWebEngineWebViewPrivate::q_cookieAdded);
        QObject::connect(cookieStore, &QWebEngineCookieStore::cookieRemoved, parent, &QWebEngineWebViewPrivate::q_cookieRemoved);
    }
}

QWebEngineWebViewSettingsPrivate::QWebEngineWebViewSettingsPrivate(QWebEngineWebViewPrivate *p)
    : QWebViewSettingsPrivate(p)
{

}

bool QWebEngineWebViewSettingsPrivate::testAttribute(QWebViewSettings::WebAttribute attribute) const
{
    switch (attribute) {
    case QWebViewSettings::WebAttribute::localStorageEnabled:
        return localStorageEnabled();
    case QWebViewSettings::WebAttribute::javaScriptEnabled:
        return javaScriptEnabled();
    case QWebViewSettings::WebAttribute::allowFileAccess:
        return allowFileAccess();
    case QWebViewSettings::WebAttribute::localContentCanAccessFileUrls:
        return localContentCanAccessFileUrls();
    }
    return false;
}

void QWebEngineWebViewSettingsPrivate::setAttribute(QWebViewSettings::WebAttribute attribute, bool value)
{
    switch (attribute) {
    case QWebViewSettings::WebAttribute::localStorageEnabled:
        setLocalStorageEnabled(value);
        break;
    case QWebViewSettings::WebAttribute::javaScriptEnabled:
        setJavaScriptEnabled(value);
        break;
    case QWebViewSettings::WebAttribute::allowFileAccess:
        setAllowFileAccess(value);
        break;
    case QWebViewSettings::WebAttribute::localContentCanAccessFileUrls:
        setLocalContentCanAccessFileUrls(value);
        break;
    }
}

bool QWebEngineWebViewSettingsPrivate::localStorageEnabled() const
{
    return m_settings ? m_settings->localStorageEnabled() : m_localStorageEnabled;
}
bool QWebEngineWebViewSettingsPrivate::javaScriptEnabled() const
{
    return m_settings ? m_settings->javascriptEnabled() : m_javaScriptEnabled;
}
bool QWebEngineWebViewSettingsPrivate::localContentCanAccessFileUrls() const
{
    return m_settings ? m_settings->localContentCanAccessFileUrls() : m_localContentCanAccessFileUrlsEnabled;
}
bool QWebEngineWebViewSettingsPrivate::allowFileAccess() const
{
    return m_allowFileAccessEnabled;
}
void QWebEngineWebViewSettingsPrivate::setLocalContentCanAccessFileUrls(bool enabled)
{
    if (m_settings)
        m_settings->setLocalContentCanAccessFileUrls(enabled);

    m_localContentCanAccessFileUrlsEnabled  = enabled;
}
void QWebEngineWebViewSettingsPrivate::setJavaScriptEnabled(bool enabled)
{
    if (m_settings)
        m_settings->setJavascriptEnabled(enabled);

    m_javaScriptEnabled = enabled;
}
void QWebEngineWebViewSettingsPrivate::setLocalStorageEnabled(bool enabled)
{
    // This separation is a bit different on the mobile platforms, so for now
    // we'll interpret this property to also affect the "off the record" profile setting.
    if (auto webview = qobject_cast<QWebEngineWebViewPrivate *>(parent())) {
        if (webview->m_profile)
            webview->m_profile->setOffTheRecord(enabled);
    }

    if (m_settings)
        m_settings->setLocalStorageEnabled(enabled);

    m_localStorageEnabled = enabled;
}
void QWebEngineWebViewSettingsPrivate::setAllowFileAccess(bool enabled)
{
    Q_UNUSED(enabled);
}

void QWebEngineWebViewSettingsPrivate::init(QQuickWebEngineSettings *settings)
{
    m_settings = settings;

    if (m_settings) {
        // Sync any values already set.
        setLocalContentCanAccessFileUrls(m_localContentCanAccessFileUrlsEnabled);
        setJavaScriptEnabled(m_javaScriptEnabled);
        setLocalStorageEnabled(m_localStorageEnabled);
    }
}

QT_END_NAMESPACE
