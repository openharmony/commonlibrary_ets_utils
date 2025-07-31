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

import { registerArkTS } from './arkts.ets';
import { registerArkUI } from './arkui.ets';
import { registerWindow } from './window.ets';
import { registerAbilityKit } from './abilitykit.ets';
import { registerOsAccount } from './osAccount.ets';
import { registerArkWeb } from './arkweb.ets';
import { registerCommonEventManager } from './basicServicesKit.ets'
import { registerRpc } from './rpc.ets';

export function registerMain(){
    registerArkTS();
    registerArkUI();
    registerWindow();
    registerAbilityKit();
    registerOsAccount();
    registerArkWeb();
    registerCommonEventManager();
    registerRpc();
}