/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "util_helper.h"

#include "util_plugin.h"
#include <unicode/ustring.h>
#include <stdio.h>

namespace Commonlibrary::Platform {
using namespace OHOS::Util;
bool isChineseEncoding = false;
std::string targetEncStr;

UConverter* CreateConverter(const std::string& encStr_, UErrorCode& codeflag) {
    const std::string convertFormat("gbk,GBK,GB2312,gb2312,GB18030,gb18030");
    targetEncStr = encStr_;
    std::string encodeStr = "";
    if (convertFormat.find(targetEncStr.c_str()) != convertFormat.npos) {
        isChineseEncoding = true;
        encodeStr = "ISO-8859-1";
    } else {
        isChineseEncoding = false;
        encodeStr = targetEncStr;
    }
    UConverter *conv = ucnv_open(encodeStr.c_str(), &codeflag);
    if (U_FAILURE(codeflag)) {
        HILOG_ERROR("Unable to create a UConverter object");
        return NULL;
    }

    ucnv_setFromUCallBack(conv, UCNV_FROM_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, &codeflag);
    if (U_FAILURE(codeflag)) {
        HILOG_ERROR("Unable to set the from Unicode callback function");
        ucnv_close(conv);
        return NULL;
    }

    ucnv_setToUCallBack(conv, UCNV_TO_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, &codeflag);
    if (U_FAILURE(codeflag)) {
        HILOG_ERROR("Unable to set the to Unicode callback function");
        ucnv_close(conv);
        return NULL;
    }

    return conv;
}

std::string ConvertToString(UChar * uchar, size_t length)
{
    std::string tepStr;
    if (isChineseEncoding) {
        std::string input = "";
        for (size_t i = 0; i < length; ++i) {
            input += static_cast<char>(uchar[i] & 0xFF);
        }
        tepStr = UtilPlugin::Decode(input, targetEncStr);
    } else {
        std::u16string tempStr16(uchar);
        tepStr = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(tempStr16);
    }
    return tepStr;
}

void EncodeIntoChinese(napi_env env, napi_value src, std::string encoding, std::string& buffer)
{
    std::string input = "";
    size_t inputSize = 0;
    napi_get_value_string_utf8(env, src, nullptr, 0, &inputSize); // 0:buffer size
    input.resize(inputSize);
    napi_get_value_string_utf8(env, src, input.data(), inputSize + 1, &inputSize);
    buffer = UtilPlugin::EncodeIntoChinese(input, encoding);
}

} // namespace Commonlibrary::Platform