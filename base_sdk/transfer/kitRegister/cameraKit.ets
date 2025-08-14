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

export function registerCameraKit() {
  registerStaticCB('CameraKit.CameraInput', '@ohos.multimedia.camera', 'camera', 'cameraInputTransferStatic');
  registerDynamicCB('CameraKit.CameraInput', '@ohos.multimedia.camera', 'camera', 'cameraInputTransferDynamic');

  registerStaticCB('CameraKit.PhotoOutput', '@ohos.multimedia.camera', 'camera', 'photoOutputTransferStatic');
  registerDynamicCB('CameraKit.PhotoOutput', '@ohos.multimedia.camera', 'camera', 'photoOutputTransferDynamic');

  registerStaticCB('CameraKit.PreviewOutput', '@ohos.multimedia.camera', 'camera', 'previewOutputTransferStatic');
  registerDynamicCB('CameraKit.PreviewOutput', '@ohos.multimedia.camera', 'camera', 'previewOutputTransferDynamic');

  registerStaticCB('CameraKit.VideoOutput', '@ohos.multimedia.camera', 'camera', 'videoOutputTransferStatic');
  registerDynamicCB('CameraKit.VideoOutput', '@ohos.multimedia.camera', 'camera', 'videoOutputTransferDynamic');

  registerStaticCB('CameraKit.MetadataOutput', '@ohos.multimedia.camera', 'camera', 'metadataOutputTransferStatic');
  registerDynamicCB('CameraKit.MetadataOutput', '@ohos.multimedia.camera', 'camera', 'metadataOutputTransferDynamic');

  registerStaticCB('CameraKit.VideoSession', '@ohos.multimedia.camera', 'camera', 'videoSessionTransferStatic');
  registerDynamicCB('CameraKit.VideoSession', '@ohos.multimedia.camera', 'camera', 'videoSessionTransferDynamic');

  registerStaticCB('CameraKit.SecureSession', '@ohos.multimedia.camera', 'camera', 'secureSessionTransferStatic');
  registerDynamicCB('CameraKit.SecureSession', '@ohos.multimedia.camera', 'camera', 'secureSessionTransferDynamic');

  registerStaticCB('CameraKit.PhotoSession', '@ohos.multimedia.camera', 'camera', 'photoSessionTransferStatic');
  registerDynamicCB('CameraKit.PhotoSession', '@ohos.multimedia.camera', 'camera', 'photoSessionTransferDynamic');

  registerStaticCB('CameraKit.Photo', '@ohos.multimedia.camera', 'camera', 'photoTransferStatic');
  registerDynamicCB('CameraKit.Photo', '@ohos.multimedia.camera', 'camera', 'photoTransferDynamic');
}