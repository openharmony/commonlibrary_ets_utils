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

export function registerAbilityKit() {
  registerStaticCB('AbilityKit.UIAbilityContext', 'application.UIAbilityContext', 'UIAbilityContext', 'transferStatic');
  registerDynamicCB('AbilityKit.UIAbilityContext', 'application.UIAbilityContext', 'UIAbilityContext', 'transferDynamic');

  registerStaticCB('AbilityKit.Context', 'application.Context', 'Context', 'transferStatic');
  registerDynamicCB('AbilityKit.Context', 'application.Context', 'Context', 'transferDynamic');

  registerStaticCB('AbilityKit.ApplicationContext', 'application.ApplicationContext', 'ApplicationContext', 'transferStatic');
  registerDynamicCB('AbilityKit.ApplicationContext', 'application.ApplicationContext', 'ApplicationContext', 'transferDynamic');

  registerStaticCB('AbilityKit.AbilityStageContext', 'application.AbilityStageContext', 'AbilityStageContext', 'transferStatic');
  registerDynamicCB('AbilityKit.AbilityStageContext', 'application.AbilityStageContext', 'AbilityStageContext', 'transferDynamic');

  registerStaticCB('AbilityKit.Caller', 'application.Caller', 'CallerImpl', 'transferStatic');
  registerDynamicCB('AbilityKit.Caller', 'application.Caller', 'CallerImpl', 'transferDynamic');

  registerStaticCB('AbilityKit.Callee', 'application.Callee', 'CalleeImpl', 'transferStatic');
  registerDynamicCB('AbilityKit.Callee', 'application.Callee', 'CalleeImpl', 'transferDynamic');

  registerStaticCB('AbilityKit.ExtensionContext', 'application.ExtensionContext', 'ExtensionContext', 'transferStatic');
  registerDynamicCB('AbilityKit.ExtensionContext', 'application.ExtensionContext', 'ExtensionContext', 'transferDynamic');

  registerStaticCB('AbilityKit.UIExtensionContext', 'application.UIExtensionContext', 'UIExtensionContext', 'transferStatic');
  registerDynamicCB('AbilityKit.UIExtensionContext', 'application.UIExtensionContext', 'UIExtensionContext', 'transferDynamic');

  registerStaticCB('AbilityKit.AutoFillExtensionContext', 'application.AutoFillExtensionContext', 'AutoFillExtensionContext', 'transferStatic');
  registerDynamicCB('AbilityKit.AutoFillExtensionContext', 'application.AutoFillExtensionContext', 'AutoFillExtensionContext', 'transferDynamic');

  registerStaticCB('AbilityKit.AppServiceExtensionContext', 'application.AppServiceExtensionContext', 'AppServiceExtensionContext', 'transferStatic');
  registerDynamicCB('AbilityKit.AppServiceExtensionContext', 'application.AppServiceExtensionContext', 'AppServiceExtensionContext', 'transferDynamic');

  registerStaticCB('AbilityKit.ServiceExtensionContext', 'application.ServiceExtensionContext', 'ServiceExtensionContext', 'transferStatic');
  registerDynamicCB('AbilityKit.ServiceExtensionContext', 'application.ServiceExtensionContext', 'ServiceExtensionContext', 'transferDynamic');

  registerStaticCB('AbilityKit.UIExtensionContentSession', '@ohos.app.ability.UIExtensionContentSession', 'UIExtensionContentSession', 'transferStatic');
  registerDynamicCB('AbilityKit.UIExtensionContentSession', '@ohos.app.ability.UIExtensionContentSession', 'UIExtensionContentSession', 'transferDynamic');

  registerStaticCB('AbilityKit.InsightIntentContext', '@ohos.app.ability.InsightIntentContext', 'InsightIntentContext', 'transferStatic');
  registerDynamicCB('AbilityKit.InsightIntentContext', '@ohos.app.ability.InsightIntentContext', 'InsightIntentContext', 'transferDynamic');

  registerStaticCB('AbilityKit.UIServiceExtensionContext', 'application.UIServiceExtensionContext', 'UIServiceExtensionContext', 'transferStatic');
  registerDynamicCB('AbilityKit.UIServiceExtensionContext', 'application.UIServiceExtensionContext', 'UIServiceExtensionContext', 'transferDynamic');
}
