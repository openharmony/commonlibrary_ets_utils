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

export function registerEffectKit() {
    registerStaticCB('ArkGraphics2D.Filter', '@ohos.effectKit','effectKit.FilterInternal','kitTransferStatic');// 1.1 => 1.2对象
    registerDynamicCB('ArkGraphics2D.Filter', '@ohos.effectKit','effectKit.FilterInternal','kitTransferDynamic');// 1.2 => 1.1对象
    registerStaticCB('ArkGraphics2D.ColorPicker', '@ohos.effectKit','effectKit.ColorPickerInternal','kitTransferStatic');// 1.1 => 1.2对象
    registerDynamicCB('ArkGraphics2D.ColorPicker', '@ohos.effectKit','effectKit.ColorPickerInternal','kitTransferDynamic');// 1.2 => 1.1对象
}