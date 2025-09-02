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

export function registerDrawing() {
    registerStaticCB('ArkGraphics2D.SamplingOptions', '@ohos.graphics.drawing', 'drawing.SamplingOptions', 'samplingOptionsTransferStatic');
    registerDynamicCB('ArkGraphics2D.SamplingOptions', '@ohos.graphics.drawing', 'drawing.SamplingOptions', 'samplingOptionsTransferDynamic');

    registerStaticCB('ArkGraphics2D.Canvas', '@ohos.graphics.drawing', 'drawing.Canvas', 'canvasTransferStatic');
    registerDynamicCB('ArkGraphics2D.Canvas', '@ohos.graphics.drawing', 'drawing.Canvas', 'canvasTransferDynamic');

    registerStaticCB('ArkGraphics2D.ColorFilter', '@ohos.graphics.drawing', 'drawing.ColorFilter', 'colorFilterTransferStatic');
    registerDynamicCB('ArkGraphics2D.ColorFilter', '@ohos.graphics.drawing', 'drawing.ColorFilter', 'colorFilterTransferDynamic');

    registerStaticCB('ArkGraphics2D.Pen', '@ohos.graphics.drawing', 'drawing.Pen', 'penTransferStatic');
    registerDynamicCB('ArkGraphics2D.Pen', '@ohos.graphics.drawing', 'drawing.Pen', 'penTransferDynamic');

    registerStaticCB('ArkGraphics2D.Brush', '@ohos.graphics.drawing', 'drawing.Brush', 'brushTransferStatic');
    registerDynamicCB('ArkGraphics2D.Brush', '@ohos.graphics.drawing', 'drawing.Brush', 'brushTransferDynamic');

    registerStaticCB('ArkGraphics2D.Typeface', '@ohos.graphics.drawing', 'drawing.Typeface', 'typefaceTransferStatic');
    registerDynamicCB('ArkGraphics2D.Typeface', '@ohos.graphics.drawing', 'drawing.Typeface', 'typefaceTransferDynamic');

    registerStaticCB('ArkGraphics2D.Font', '@ohos.graphics.drawing', 'drawing.Font', 'fontTransferStatic');
    registerDynamicCB('ArkGraphics2D.Font', '@ohos.graphics.drawing', 'drawing.Font', 'fontTransferDynamic');

    registerStaticCB('ArkGraphics2D.Lattice', '@ohos.graphics.drawing', 'drawing.Lattice', 'latticeTransferStatic');
    registerDynamicCB('ArkGraphics2D.Lattice', '@ohos.graphics.drawing', 'drawing.Lattice', 'latticeTransferDynamic');

    registerStaticCB('ArkGraphics2D.Matrix', '@ohos.graphics.drawing', 'drawing.Matrix', 'matrixTransferStatic');
    registerDynamicCB('ArkGraphics2D.Matrix', '@ohos.graphics.drawing', 'drawing.Matrix', 'matrixTransferDynamic');

    registerStaticCB('ArkGraphics2D.Path', '@ohos.graphics.drawing', 'drawing.Path', 'pathTransferStatic');
    registerDynamicCB('ArkGraphics2D.Path', '@ohos.graphics.drawing', 'drawing.Path', 'pathTransferDynamic');

    registerStaticCB('ArkGraphics2D.Region', '@ohos.graphics.drawing', 'drawing.Region', 'regionTransferStatic');
    registerDynamicCB('ArkGraphics2D.Region', '@ohos.graphics.drawing', 'drawing.Region', 'regionTransferDynamic');

    registerStaticCB('ArkGraphics2D.RoundRect', '@ohos.graphics.drawing', 'drawing.RoundRect', 'roundRectTransferStatic');
    registerDynamicCB('ArkGraphics2D.RoundRect', '@ohos.graphics.drawing', 'drawing.RoundRect', 'roundRectTransferDynamic');
}