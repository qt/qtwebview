// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qohoswebresourceerror.h"

WebResourceError WebResourceError::makeFromJsObject(QNapi::Object jsWebResourceErrorObject)
{
    return WebResourceError(
        jsWebResourceErrorObject.call<QNapi::Number>("getErrorCode"),
        jsWebResourceErrorObject.call<QNapi::String>("getErrorInfo"));
}

WebResourceError::WebResourceError(int errorCode, const std::string &errorInfo)
    : m_errorCode(errorCode)
    , m_errorInfo(errorInfo)
{
}

int WebResourceError::errorCode() const
{
    return m_errorCode;
}

std::string WebResourceError::errorInfo() const
{
    return m_errorInfo;
}
