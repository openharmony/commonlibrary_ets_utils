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

export function registerUDMF() {
  registerStaticCB('ArkData.SystemDefinedRecord', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'systemDefinedRecordTransferStatic');
  registerDynamicCB('ArkData.SystemDefinedRecord', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'systemDefinedRecordTransferDynamic');

  registerStaticCB('ArkData.SystemDefinedForm', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'systemDefinedFormTransferStatic');
  registerDynamicCB('ArkData.SystemDefinedForm', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'systemDefinedFormTransferDynamic');

  registerStaticCB('ArkData.SystemDefinedAppItem', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'systemDefinedAppItemTransferStatic');
  registerDynamicCB('ArkData.SystemDefinedAppItem', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'systemDefinedAppItemTransferDynamic');

  registerStaticCB('ArkData.SystemDefinedPixelMap', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'systemDefinedPixelMapTransferStatic');
  registerDynamicCB('ArkData.SystemDefinedPixelMap', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'systemDefinedPixelMapTransferDynamic');

  registerStaticCB('ArkData.ApplicationDefinedRecord', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'applicationDefinedRecordTransferStatic');
  registerDynamicCB('ArkData.ApplicationDefinedRecord', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'applicationDefinedRecordTransferDynamic');

  registerStaticCB('ArkData.Summary', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'summaryTransferStatic');
  registerDynamicCB('ArkData.Summary', '@ohos.data.unifiedDataChannel', 'unifiedDataChannel', 'summaryTransferDynamic');
}