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

export function registerIpc() {
  registerStaticCB('Ipc.MessageSequence', '@ohos.rpc', 'rpc.MessageSequence', 'TransferStatic');
  registerDynamicCB('Ipc.MessageSequence', '@ohos.rpc', 'rpc.MessageSequence', 'TransferDynamic');
  registerStaticCB('Ipc.Ashmem', '@ohos.rpc', 'rpc.Ashmem', 'TransferStatic');
  registerDynamicCB('Ipc.Ashmem', '@ohos.rpc', 'rpc.Ashmem', 'TransferDynamic');
  registerStaticCB('Ipc.RemoteObject', '@ohos.rpc', 'rpc.RemoteObject', 'TransferStatic');
  registerDynamicCB('Ipc.RemoteObject', '@ohos.rpc', 'rpc.RemoteObject', 'TransferDynamic');
  registerStaticCB('Ipc.RemoteProxy', '@ohos.rpc', 'rpc.RemoteProxy', 'TransferStatic');
  registerDynamicCB('Ipc.RemoteProxy', '@ohos.rpc', 'rpc.RemoteProxy', 'TransferDynamic');
  registerStaticCB('Ipc.MessageOption', '@ohos.rpc', 'rpc.MessageOption', 'TransferStatic');
  registerDynamicCB('Ipc.MessageOption', '@ohos.rpc', 'rpc.MessageOption', 'TransferDynamic');
}