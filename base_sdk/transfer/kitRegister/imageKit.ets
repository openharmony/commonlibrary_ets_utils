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

export function registerImageKit() {
  registerStaticCB('ImageKit.ImageSource', '@ohos.multimedia.image', 'image', 'imageSourceTransferStatic');
  registerDynamicCB('ImageKit.ImageSource', '@ohos.multimedia.image', 'image', 'imageSourceTransferDynamic');

  registerStaticCB('ImageKit.ImagePacker', '@ohos.multimedia.image', 'image', 'imagePackerTransferStatic');
  registerDynamicCB('ImageKit.ImagePacker', '@ohos.multimedia.image', 'image', 'imagePackerTransferDynamic');

  registerStaticCB('ImageKit.Picture', '@ohos.multimedia.image', 'image', 'pictureTransferStatic');
  registerDynamicCB('ImageKit.Picture', '@ohos.multimedia.image', 'image', 'pictureTransferDynamic');

  registerStaticCB('ImageKit.AuxiliaryPicture', '@ohos.multimedia.image', 'image', 'auxiliaryPictureTransferStatic');
  registerDynamicCB('ImageKit.AuxiliaryPicture', '@ohos.multimedia.image', 'image', 'auxiliaryPictureTransferDynamic');

  registerStaticCB('ImageKit.ImageReceiver', '@ohos.multimedia.image', 'image', 'imageReceiverTransferStatic');
  registerDynamicCB('ImageKit.ImageReceiver', '@ohos.multimedia.image', 'image', 'imageReceiverTransferDynamic');

  registerStaticCB('ImageKit.ImageCreator', '@ohos.multimedia.image', 'image', 'imageCreatorTransferStatic');
  registerDynamicCB('ImageKit.ImageCreator', '@ohos.multimedia.image', 'image', 'imageCreatorTransferDynamic');
}
