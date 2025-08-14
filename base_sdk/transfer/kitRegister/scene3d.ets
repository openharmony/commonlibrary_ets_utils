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

export function registerScene3D() {
    registerStaticCB('ArkGraphics3D.Scene', 'graphics3d.Scene', 'Transfer', 'sceneTransferStatic');
    registerDynamicCB('ArkGraphics3D.Scene', 'graphics3d.Scene', 'Transfer', 'sceneTransferDynamic');
    registerStaticCB('ArkGraphics3D.SceneResourceFactory', 'graphics3d.Scene', 'Transfer', 'sceneResourceFactoryTransferStatic');
    registerDynamicCB('ArkGraphics3D.SceneResourceFactory', 'graphics3d.Scene', 'Transfer', 'sceneResourceFactoryTransferDynamic');
    registerStaticCB('ArkGraphics3D.Camera', 'graphics3d.SceneNodes', 'Transfer', 'cameraTransferStatic');
    registerDynamicCB('ArkGraphics3D.Camera', 'graphics3d.SceneNodes', 'Transfer', 'cameraTransferDynamic');
    registerStaticCB('ArkGraphics3D.Node', 'graphics3d.SceneNodes', 'Transfer', 'nodeTransferStatic');
    registerDynamicCB('ArkGraphics3D.Node', 'graphics3d.SceneNodes', 'Transfer', 'nodeTransferDynamic');
    registerStaticCB('ArkGraphics3D.Animation', 'graphics3d.SceneResources', 'Transfer', 'animationTransferStatic');
    registerDynamicCB('ArkGraphics3D.Animation', 'graphics3d.SceneResources', 'Transfer', 'animationTransferDynamic');
    registerStaticCB('ArkGraphics3D.Environment', 'graphics3d.SceneResources', 'Transfer', 'environmentTransferStatic');
    registerDynamicCB('ArkGraphics3D.Environment', 'graphics3d.SceneResources', 'Transfer', 'environmentTransferDynamic');
}
