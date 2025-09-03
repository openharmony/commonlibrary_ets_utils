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

export function registerArkGraphics2D() {
    registerStaticCB('ArkGraphics2D.FontCollection', '@ohos.graphics.text', 'text.FontCollection', 'transferStatic');
    registerDynamicCB('ArkGraphics2D.FontCollection', '@ohos.graphics.text', 'text.FontCollection', 'transferDynamic');

    registerStaticCB('ArkGraphics2D.ParagraphBuilder', '@ohos.graphics.text', 'text.ParagraphBuilder', 'transferStatic');
    registerDynamicCB('ArkGraphics2D.ParagraphBuilder', '@ohos.graphics.text', 'text.ParagraphBuilder', 'transferDynamic');

    registerStaticCB('ArkGraphics2D.Paragraph', '@ohos.graphics.text', 'text.Paragraph', 'transferStatic');
    registerDynamicCB('ArkGraphics2D.Paragraph', '@ohos.graphics.text', 'text.Paragraph', 'transferDynamic');

    registerStaticCB('ArkGraphics2D.TextLine', '@ohos.graphics.text', 'text.TextLine', 'transferStatic');
    registerDynamicCB('ArkGraphics2D.TextLine', '@ohos.graphics.text', 'text.TextLine', 'transferDynamic');

    registerStaticCB('ArkGraphics2D.Run', '@ohos.graphics.text', 'text.Run', 'transferStatic');
    registerDynamicCB('ArkGraphics2D.Run', '@ohos.graphics.text', 'text.Run', 'transferDynamic');
}
