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
    registerStaticCB('ArkWeb.ScreenCaptureHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferScreenCaptureHandlerStatic');
    registerDynamicCB('ArkWeb.ScreenCaptureHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferScreenCaptureHandlerDynamic');

    registerStaticCB('ArkWeb.FileSelectorParam', 'arkui.component.web', 'ArkWebTransfer', 'transferFileSelectorParamStatic');
    registerDynamicCB('ArkWeb.FileSelectorParam', 'arkui.component.web', 'ArkWebTransfer', 'transferFileSelectorParamDynamic');

    registerStaticCB('ArkWeb.JsGeolocation', 'arkui.component.web', 'ArkWebTransfer', 'transferJsGeolocationStatic');
    registerDynamicCB('ArkWeb.JsGeolocation', 'arkui.component.web', 'ArkWebTransfer', 'transferJsGeolocationDynamic');

    registerStaticCB('ArkWeb.JsResult', 'arkui.component.web', 'ArkWebTransfer', 'transferJsResultStatic');
    registerDynamicCB('ArkWeb.JsResult', 'arkui.component.web', 'ArkWebTransfer', 'transferJsResultDynamic');

    registerStaticCB('ArkWeb.EventResult', 'arkui.component.web', 'ArkWebTransfer', 'transferEventResultStatic');
    registerDynamicCB('ArkWeb.EventResult', 'arkui.component.web', 'ArkWebTransfer', 'transferEventResultDynamic');

    registerStaticCB('ArkWeb.FileSelectorResult', 'arkui.component.web', 'ArkWebTransfer', 'transferFileSelectorResultStatic');
    registerDynamicCB('ArkWeb.FileSelectorResult', 'arkui.component.web', 'ArkWebTransfer', 'transferFileSelectorResultDynamic');

    registerStaticCB('ArkWeb.WebContextMenuParam', 'arkui.component.web', 'ArkWebTransfer', 'transferWebContextMenuParamStatic');
    registerDynamicCB('ArkWeb.WebContextMenuParam', 'arkui.component.web', 'ArkWebTransfer', 'transferWebContextMenuParamDynamic');

    registerStaticCB('ArkWeb.WebContextMenuResult', 'arkui.component.web', 'ArkWebTransfer', 'transferWebContextMenuResultStatic');
    registerDynamicCB('ArkWeb.WebContextMenuResult', 'arkui.component.web', 'ArkWebTransfer', 'transferWebContextMenuResultDynamic');

    registerStaticCB('ArkWeb.HttpAuthHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferHttpAuthHandlerToStatic');
    registerDynamicCB('ArkWeb.HttpAuthHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferHttpAuthHandlerToDynamic');

    registerStaticCB('ArkWeb.WebResourceResponse', 'arkui.component.web', 'ArkWebTransfer', 'transferWebResourceReponseToStatic');
    registerDynamicCB('ArkWeb.WebResourceResponse', 'arkui.component.web', 'ArkWebTransfer', 'transferWebResourceReponseToDynamic');

    registerStaticCB('ArkWeb.WebResourceRequest', 'arkui.component.web', 'ArkWebTransfer', 'transferWebResourceRequestToStatic');
    registerDynamicCB('ArkWeb.WebResourceRequest', 'arkui.component.web', 'ArkWebTransfer', 'transferWebResourceRequestToDynamic');

    registerStaticCB('ArkWeb.ConsoleMessage', 'arkui.component.web', 'ArkWebTransfer', 'transferConsoleMessageToStatic');
    registerDynamicCB('ArkWeb.ConsoleMessage', 'arkui.component.web', 'ArkWebTransfer', 'transferConsoleMessageToDynamic');

    registerStaticCB('ArkWeb.DataResubmissionHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferDataResubmissionHandlerToStatic');
    registerDynamicCB('ArkWeb.DataResubmissionHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferDataResubmissionHandlerToDynamic');

    registerStaticCB('ArkWeb.ClientAuthenticationHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferClientAuthenticationHandlerToStatic');
    registerDynamicCB('ArkWeb.ClientAuthenticationHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferClientAuthenticationHandlerToDynamic');

    registerStaticCB('ArkWeb.SslErrorHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferSslErrorHandlerToStatic');
    registerDynamicCB('ArkWeb.SslErrorHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferSslErrorHandlerToDynamic');

    registerStaticCB('ArkWeb.PermissionRequest', 'arkui.component.web', 'ArkWebTransfer', 'transferPermissionRequestStatic');
    registerDynamicCB('ArkWeb.PermissionRequest', 'arkui.component.web', 'ArkWebTransfer', 'transferPermissionRequestDynamic');

    registerStaticCB('ArkWeb.WebKeyboardController', 'arkui.component.web', 'ArkWebTransfer', 'transferWebKeyboardControllerStatic');
    registerDynamicCB('ArkWeb.WebKeyboardController', 'arkui.component.web', 'ArkWebTransfer', 'transferWebKeyboardControllerDynamic');

    registerStaticCB('ArkWeb.ControllerHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferControllerHandlerStatic');
    registerDynamicCB('ArkWeb.ControllerHandler', 'arkui.component.web', 'ArkWebTransfer', 'transferControllerHandlerDynamic');

    registerStaticCB('ArkWeb.BackForwardList', '@ohos.web.webview', 'webview.BackForwardListinner', 'transferBackForwardListToStatic');
    registerDynamicCB('ArkWeb.BackForwardList', '@ohos.web.webview', 'webview.BackForwardListinner', 'transferBackForwardListToDynamic');

    registerStaticCB('ArkWeb.WebMessagePort', '@ohos.web.webview', 'webview.WebMessagePortInner', 'transferWebMessagePortToStatic');
    registerDynamicCB('ArkWeb.WebMessagePort', '@ohos.web.webview', 'webview.WebMessagePortInner', 'transferWebMessagePortToDynamic');

    registerStaticCB('ArkWeb.WebResourceHandler', '@ohos.web.webview', 'webview.WebResourceHandler', 'transferWebResourceHandlerToStatic');
    registerDynamicCB('ArkWeb.WebResourceHandler', '@ohos.web.webview', 'webview.WebResourceHandler', 'transferWebResourceHandlerToDynamic');

    registerStaticCB('ArkWeb.WebSchemeHandlerRequest', '@ohos.web.webview', 'webview.WebSchemeHandlerRequest', 'transferWebSchemeHandlerRequestToStatic');
    registerDynamicCB('ArkWeb.WebSchemeHandlerRequest', '@ohos.web.webview', 'webview.WebSchemeHandlerRequest', 'transferWebSchemeHandlerRequestToDynamic');

    registerStaticCB('ArkWeb.NativeMediaPlayerHandler', '@ohos.web.webview', 'webview.NativeMediaPlayerHandlerinner', 'transferNativeMediaPlayerHandlerToStatic');
    registerDynamicCB('ArkWeb.NativeMediaPlayerHandler', '@ohos.web.webview', 'webview.NativeMediaPlayerHandlerinner', 'transferNativeMediaPlayerHandlerToDynamic');
}
