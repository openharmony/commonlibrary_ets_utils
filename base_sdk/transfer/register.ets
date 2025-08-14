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

export interface CallbackUrl {
    key: string;
    filePath: string;
    className: string;
    methodName: string;
    staticMethod?: (input: Any) => Object;
    dynamicMethod?: (input: Object) => Any;
}

export const staticRegisterMap = new Map<string, CallbackUrl>();
export const dynamicRegisterMap = new Map<string, CallbackUrl>();

export function registerStaticCB(key: string, filePath: string, className: string, methodName: string) {
    let callbackUrl: CallbackUrl = {
        key: key,
        filePath: filePath,
        className: className,
        methodName: methodName
    };
    staticRegisterMap.set(key, callbackUrl);
}

export function registerDynamicCB(key: string, filePath: string, className: string, methodName: string) {
    let callbackUrl: CallbackUrl = {
        key: key,
        filePath: filePath,
        className: className,
        methodName: methodName
    };
    dynamicRegisterMap.set(key, callbackUrl);
}

export function registerStaticCB(key: string, staticMethod: (input: Any) => Object) {
    let callbackUrl: CallbackUrl = {
        key: key,
        filePath: '',
        className: '',
        methodName: '',
        staticMethod: staticMethod
    };
    staticRegisterMap.set(key, callbackUrl);
}

export function registerDynamicCB(key: string, dynamicMethod: (input: Object) => Any) {
    let callbackUrl: CallbackUrl = {
        key: key,
        filePath: '',
        className: '',
        methodName: '',
        dynamicMethod: dynamicMethod
    };
    dynamicRegisterMap.set(key, callbackUrl);
}


