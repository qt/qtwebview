// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOHOSJSWEBRESOURCEERROR_H
#define QOHOSJSWEBRESOURCEERROR_H

#include <QtCore/private/qnapi_p.h>
#include <string>

class WebResourceError
{
public:
    static WebResourceError makeFromJsObject(QNapi::Object errorObject);

    int errorCode() const;
    std::string errorInfo() const;

protected:
    WebResourceError(int errorCode, const std::string &errorInfo);

private:
    int m_errorCode = 0;
    std::string m_errorInfo;
};

#endif // QOHOSJSWEBRESOURCEERROR_H
