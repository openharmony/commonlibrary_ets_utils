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

export function registerArkUI() {
    registerStaticCB("ArkUI.NavDestinationInfo", "@ohos.arkui.observer", "uiObserver.NavDestinationInfoImpl", "transferStatic");
    registerDynamicCB("ArkUI.NavDestinationInfo", "@ohos.arkui.observer", "uiObserver.NavDestinationInfoImpl", "transferDynamic");

    registerStaticCB("ArkUI.NavigationInfo", "@ohos.arkui.observer", "uiObserver.NavigationInfoImpl", "transferStatic");
    registerDynamicCB("ArkUI.NavigationInfo", "@ohos.arkui.observer", "uiObserver.NavigationInfoImpl", "transferDynamic");

    registerStaticCB("ArkUI.RouterPageInfo", "@ohos.arkui.observer", "uiObserver.RouterPageInfo", "transferStatic");
    registerDynamicCB("ArkUI.RouterPageInfo", "@ohos.arkui.observer", "uiObserver.RouterPageInfo", "transferDynamic");

    registerStaticCB("ArkUI.Matrix4", "@ohos.matrix4", "matrix4.Matrix4TransitInner", "matrixTransferStatic");
    registerDynamicCB("ArkUI.Matrix4", "@ohos.matrix4", "matrix4.Matrix4TransitInner", "matrixTransferDynamic");

    registerStaticCB("ArkUI.Animator", "@ohos.animator", "AnimatorResultInner", "animatorTransferStatic");
    registerDynamicCB("ArkUI.Animator", "@ohos.animator", "AnimatorResultInner", "animatorTransferDynamic"); 
}