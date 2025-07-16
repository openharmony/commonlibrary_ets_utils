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

    registerStaticCB('ArkWeb.JsResult', 'arkui.component.web', 'ArkWebTransfer', 'transferJsResultStatic');
    registerDynamicCB('ArkWeb.JsResult', 'arkui.component.web', 'ArkWebTransfer', 'transferJsResultDynamic');

    registerStaticCB('ArkWeb.FileSelectorResult', 'arkui.component.web', 'ArkWebTransfer', 'transferFileSelectorResultStatic');
    registerDynamicCB('ArkWeb.FileSelectorResult', 'arkui.component.web', 'ArkWebTransfer', 'transferFileSelectorResultDynamic');

    registerStaticCB('ArkWeb.WebContextMenuParam', 'arkui.component.web', 'ArkWebTransfer', 'transferWebContextMenuParamStatic');
    registerDynamicCB('ArkWeb.WebContextMenuParam', 'arkui.component.web', 'ArkWebTransfer', 'transferWebContextMenuParamDynamic');

    registerStaticCB('ArkWeb.WebContextMenuResult', 'arkui.component.web', 'ArkWebTransfer', 'transferWebContextMenuResultStatic');
    registerDynamicCB('ArkWeb.WebContextMenuResult', 'arkui.component.web', 'ArkWebTransfer', 'transferWebContextMenuResultDynamic');
}
