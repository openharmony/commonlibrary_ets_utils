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

export function registerCommonEventManager() {
    registerStaticCB('CommonEventManager.CommonEventSubscriber', 'commonEvent.commonEventSubscriber', 'CommonEventSubscriberInner', 'transferStatic');
    registerDynamicCB('CommonEventManager.CommonEventSubscriber', 'commonEvent.commonEventSubscriber', 'CommonEventSubscriberInner', 'transferDynamic');

    registerStaticCB('StaticSubscriberExtensionContext', '@ohos.application.StaticSubscriberExtensionContext', 'StaticSubscriberExtensionContext', 'transferStatic');
    registerDynamicCB('StaticSubscriberExtensionContext', '@ohos.application.StaticSubscriberExtensionContext', 'StaticSubscriberExtensionContext', 'transferDynamic');

    registerStaticCB('Emitter', '@ohos.events.emitter.emitter', 'Emitter', 'transferStatic');
    registerDynamicCB('Emitter', '@ohos.events.emitter.emitter', 'Emitter', 'transferDynamic');
}
