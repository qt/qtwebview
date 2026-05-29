// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohoswebcomponentlistener.h"
#include <QtCore/private/qcore_ohos_p.h>

namespace {

class QOhosWebComponentListenerExecutingInContextThread : public QOhosWebComponentListener
{
public:
    QOhosWebComponentListenerExecutingInContextThread(
        std::weak_ptr<QOhosWebComponentListener> weakBaseListener, QObject *baseListenerContext);
    ~QOhosWebComponentListenerExecutingInContextThread() override;

    void onErrorReceived(const std::string &url, const WebResourceError &error) override;
    void onPageBegan(const std::string &url) override;
    void onPageEnded(const std::string &url) override;
    void onProgressChanged(int progress) override;
    void onTitleReceived(const std::string &title) override;

private:
    void visitBaseListenerInItsThreadIfAlive(
        std::function<void(QOhosWebComponentListener &)> baseListenerVisitor);

    std::weak_ptr<QOhosWebComponentListener> m_weakBaseListener;
    QtOhos::QThreadSafeRef<QObject> m_baseListenerContextRef;
};

QOhosWebComponentListenerExecutingInContextThread::QOhosWebComponentListenerExecutingInContextThread(
    std::weak_ptr<QOhosWebComponentListener> weakBaseListener, QObject *baseListenerContext)
    : QOhosWebComponentListener()
    , m_weakBaseListener(weakBaseListener)
    , m_baseListenerContextRef(QtOhos::makeQThreadSafeRef(baseListenerContext))
{
}

QOhosWebComponentListenerExecutingInContextThread::~QOhosWebComponentListenerExecutingInContextThread() = default;

void QOhosWebComponentListenerExecutingInContextThread::onErrorReceived(
    const std::string &url, const WebResourceError &error)
{
    visitBaseListenerInItsThreadIfAlive([url, error](auto &baseListener) {
        baseListener.onErrorReceived(url, error);
    });
}

void QOhosWebComponentListenerExecutingInContextThread::onPageBegan(const std::string &url)
{
    visitBaseListenerInItsThreadIfAlive([url](auto &baseListener) {
        baseListener.onPageBegan(url);
    });
}

void QOhosWebComponentListenerExecutingInContextThread::onPageEnded(const std::string &url)
{
    visitBaseListenerInItsThreadIfAlive([url](auto &baseListener) {
        baseListener.onPageEnded(url);
    });
}

void QOhosWebComponentListenerExecutingInContextThread::onProgressChanged(int progress)
{
    visitBaseListenerInItsThreadIfAlive([progress](auto &baseListener) {
        baseListener.onProgressChanged(progress);
    });
}

void QOhosWebComponentListenerExecutingInContextThread::onTitleReceived(const std::string &title)
{
    visitBaseListenerInItsThreadIfAlive([title](auto &baseListener) {
        baseListener.onTitleReceived(title);
    });
}

void QOhosWebComponentListenerExecutingInContextThread::visitBaseListenerInItsThreadIfAlive(
    std::function<void(QOhosWebComponentListener &)> baseListenerVisitor)
{
    m_baseListenerContextRef.visitInQtThreadIfAlive(
        [weakBaseListener = m_weakBaseListener, baseListenerVisitor = std::move(baseListenerVisitor)](auto &) {
            auto sharedBaseListener = weakBaseListener.lock();
            if (sharedBaseListener) {
                baseListenerVisitor(*sharedBaseListener);
            }
        });
}

}

QOhosWebComponentListener::~QOhosWebComponentListener() = default;

QOhosWebComponentListener::QOhosWebComponentListener() = default;

std::shared_ptr<QOhosWebComponentListener> makeOhosWebComponentListenerExecutingInContextThread(
    std::weak_ptr<QOhosWebComponentListener> weakBaseListener, QObject *baseListenerContext)
{
    return std::make_shared<QOhosWebComponentListenerExecutingInContextThread>(
        weakBaseListener, baseListenerContext);
}
