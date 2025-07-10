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
    registerStaticCB('ArkWeb.FileSelectorParam', 'arkui.component.web', 'FileSelectorParam', 'transferStatic');
    registerDynamicCB('ArkWeb.FileSelectorParam', 'arkui.component.web', 'FileSelectorParam', 'transferDynamic');

    registerStaticCB('ArkWeb.JsResult', 'arkui.component.web', 'JsResult', 'transferStatic');
    registerDynamicCB('ArkWeb.JsResult', 'arkui.component.web', 'JsResult', 'transferDynamic');

    registerStaticCB('ArkWeb.FileSelectorResult', 'arkui.component.web', 'FileSelectorResult', 'transferStatic');
    registerDynamicCB('ArkWeb.FileSelectorResult', 'arkui.component.web', 'FileSelectorResult', 'transferDynamic');

    registerStaticCB('ArkWeb.WebContextMenuParam', 'arkui.component.web', 'WebContextMenuParam', 'transferStatic');
    registerDynamicCB('ArkWeb.WebContextMenuParam', 'arkui.component.web', 'WebContextMenuParam', 'transferDynamic');

    registerStaticCB('ArkWeb.WebContextMenuResult', 'arkui.component.web', 'WebContextMenuResult', 'transferStatic');
    registerDynamicCB('ArkWeb.WebContextMenuResult', 'arkui.component.web', 'WebContextMenuResult', 'transferDynamic');
}
