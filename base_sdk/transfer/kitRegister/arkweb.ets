/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { registerStaticCB, registerDynamicCB } from '../register.ets';

export function registerArkWeb() {
    registerStaticCB('ArkWeb.ScreenCaptureHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferScreenCaptureHandlerStatic');
    registerDynamicCB('ArkWeb.ScreenCaptureHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferScreenCaptureHandlerDynamic');

    registerStaticCB('ArkWeb.FileSelectorParam', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferFileSelectorParamStatic');
    registerDynamicCB('ArkWeb.FileSelectorParam', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferFileSelectorParamDynamic');

    registerStaticCB('ArkWeb.JsGeolocation', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferJsGeolocationStatic');
    registerDynamicCB('ArkWeb.JsGeolocation', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferJsGeolocationDynamic');

    registerStaticCB('ArkWeb.JsResult', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferJsResultStatic');
    registerDynamicCB('ArkWeb.JsResult', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferJsResultDynamic');

    registerStaticCB('ArkWeb.EventResult', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferEventResultStatic');
    registerDynamicCB('ArkWeb.EventResult', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferEventResultDynamic');

    registerStaticCB('ArkWeb.FileSelectorResult', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferFileSelectorResultStatic');
    registerDynamicCB('ArkWeb.FileSelectorResult', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferFileSelectorResultDynamic');

    registerStaticCB('ArkWeb.WebContextMenuParam', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebContextMenuParamStatic');
    registerDynamicCB('ArkWeb.WebContextMenuParam', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebContextMenuParamDynamic');

    registerStaticCB('ArkWeb.WebContextMenuResult', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebContextMenuResultStatic');
    registerDynamicCB('ArkWeb.WebContextMenuResult', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebContextMenuResultDynamic');

    registerStaticCB('ArkWeb.HttpAuthHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferHttpAuthHandlerToStatic');
    registerDynamicCB('ArkWeb.HttpAuthHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferHttpAuthHandlerToDynamic');

    registerStaticCB('ArkWeb.WebResourceResponse', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebResourceReponseToStatic');
    registerDynamicCB('ArkWeb.WebResourceResponse', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebResourceReponseToDynamic');

    registerStaticCB('ArkWeb.WebResourceRequest', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebResourceRequestToStatic');
    registerDynamicCB('ArkWeb.WebResourceRequest', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebResourceRequestToDynamic');

    registerStaticCB('ArkWeb.ConsoleMessage', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferConsoleMessageToStatic');
    registerDynamicCB('ArkWeb.ConsoleMessage', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferConsoleMessageToDynamic');

    registerStaticCB('ArkWeb.DataResubmissionHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferDataResubmissionHandlerToStatic');
    registerDynamicCB('ArkWeb.DataResubmissionHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferDataResubmissionHandlerToDynamic');

    registerStaticCB('ArkWeb.ClientAuthenticationHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferClientAuthenticationHandlerToStatic');
    registerDynamicCB('ArkWeb.ClientAuthenticationHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferClientAuthenticationHandlerToDynamic');

    registerStaticCB('ArkWeb.SslErrorHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferSslErrorHandlerToStatic');
    registerDynamicCB('ArkWeb.SslErrorHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferSslErrorHandlerToDynamic');

    registerStaticCB('ArkWeb.BackForwardList', '@ohos.web.webview', 'webview.BackForwardListinner', 'transferBackForwardListToStatic');
    registerDynamicCB('ArkWeb.BackForwardList', '@ohos.web.webview', 'webview.BackForwardListinner', 'transferBackForwardListToDynamic');

    registerStaticCB('ArkWeb.WebMessagePort', '@ohos.web.webview', 'webview.WebMessagePortInner', 'transferWebMessagePortToStatic');
    registerDynamicCB('ArkWeb.WebMessagePort', '@ohos.web.webview', 'webview.WebMessagePortInner', 'transferWebMessagePortToDynamic');

    registerStaticCB('ArkWeb.PermissionRequest', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferPermissionRequestStatic');
    registerDynamicCB('ArkWeb.PermissionRequest', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferPermissionRequestDynamic');

    registerStaticCB('ArkWeb.WebKeyboardController', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebKeyboardControllerStatic');
    registerDynamicCB('ArkWeb.WebKeyboardController', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferWebKeyboardControllerDynamic');

    registerStaticCB('ArkWeb.ControllerHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferControllerHandlerStatic');
    registerDynamicCB('ArkWeb.ControllerHandler', 'arkui.handwritten.transfer.WebTransfer', 'ArkWebTransfer', 'transferControllerHandlerDynamic');

    registerStaticCB('ArkWeb.NativeMediaPlayerHandler', '@ohos.web.webview', 'webview.NativeMediaPlayerHandlerinner', 'transferNativeMediaPlayerHandlerToStatic');
    registerDynamicCB('ArkWeb.NativeMediaPlayerHandler', '@ohos.web.webview', 'webview.NativeMediaPlayerHandlerinner', 'transferNativeMediaPlayerHandlerToDynamic');

    registerStaticCB('ArkWeb.WebResourceHandler', '@ohos.web.webview', 'webview.WebResourceHandler', 'transferWebResourceHandlerToStatic');
    registerDynamicCB('ArkWeb.WebResourceHandler', '@ohos.web.webview', 'webview.WebResourceHandler', 'transferWebResourceHandlerToDynamic');

    registerStaticCB('ArkWeb.WebSchemeHandlerRequest', '@ohos.web.webview', 'webview.WebSchemeHandlerRequest', 'transferWebSchemeHandlerRequestToStatic');
    registerDynamicCB('ArkWeb.WebSchemeHandlerRequest', '@ohos.web.webview', 'webview.WebSchemeHandlerRequest', 'transferWebSchemeHandlerRequestToDynamic');
}
