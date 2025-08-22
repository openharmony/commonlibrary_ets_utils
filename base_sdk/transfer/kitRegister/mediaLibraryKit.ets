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

export function registerMediaLibraryKit() {
  registerStaticCB('MediaLibraryKit.PhotoAsset', '@ohos.file.photoAccessHelper.photoAccessHelper', 'PhotoAssetHandle', 'transferStatic');
  registerDynamicCB('MediaLibraryKit.PhotoAsset', '@ohos.file.photoAccessHelper.photoAccessHelper', 'PhotoAssetHandle', 'transferDynamic');

  registerStaticCB('MediaLibraryKit.Album', '@ohos.file.photoAccessHelper.photoAccessHelper', 'AlbumHandle', 'transferStatic');
  registerDynamicCB('MediaLibraryKit.Album', '@ohos.file.photoAccessHelper.photoAccessHelper', 'AlbumHandle', 'transferDynamic');

  registerStaticCB('MediaLibraryKit.AbsAlbum', '@ohos.file.photoAccessHelper.photoAccessHelper', 'AlbumHandle', 'transferStatic');
  registerDynamicCB('MediaLibraryKit.AbsAlbum', '@ohos.file.photoAccessHelper.photoAccessHelper', 'AlbumHandle', 'transferDynamic');

  registerStaticCB('MediaLibraryKit.FetchResult', '@ohos.file.photoAccessHelper.photoAccessHelper', 'FetchResultHandle', 'transferStatic');
  registerDynamicCB('MediaLibraryKit.FetchResult', '@ohos.file.photoAccessHelper.photoAccessHelper', 'FetchResultHandle', 'transferDynamic');

  registerStaticCB('MediaLibraryKit.MovingPhoto', '@ohos.file.photoAccessHelper.photoAccessHelper', 'MovingPhotoHandle', 'transferStatic');
  registerDynamicCB('MediaLibraryKit.MovingPhoto', '@ohos.file.photoAccessHelper.photoAccessHelper', 'MovingPhotoHandle', 'transferDynamic');
}