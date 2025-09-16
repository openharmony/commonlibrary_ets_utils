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
import { staticRegisterMap, dynamicRegisterMap, CallbackUrl } from './register.ets';
import { BusinessError } from '@ohos.base';
import { registerMain } from './kitRegister/registerMain.ets';

registerMain();

const transferErrorCodeId = 10200067;

const staticCBCache = new Map<string, Method>();
const dynamicCBCache = new Map<string, Method>();
const staticRegisterMapRef = staticRegisterMap;
const dynamicRegisterMapRef = dynamicRegisterMap;

export namespace transfer {

    export function transferStatic(input: Any, inputName: string): Object {
        if (!staticRegisterMapRef.has(inputName)) {
            throw new BusinessError(transferErrorCodeId,
                new Error(`Transfer Error. The input name is not supported!`));
        }
        let cb: CallbackUrl = staticRegisterMapRef.get(inputName)!;
        if (cb.filePath === '') {
            return cb.staticMethod!(input);
        }

        let method: Method | undefined;
        if (staticCBCache.has(inputName)) {
            method = staticCBCache.get(inputName);
        } else {
            method = getMethod(cb.filePath, cb.className, cb.methodName);
            staticCBCache.set(inputName, method);
        }
        return method!.invoke(null, [input]) as Object;
    }

    export function transferDynamic(input: Object, inputName: string): Any {
        if (!dynamicRegisterMapRef.has(inputName)) {
            throw new BusinessError(transferErrorCodeId,
                new Error(`Transfer Error. The input name is not supported!`));
        }
        let cb: CallbackUrl = dynamicRegisterMapRef.get(inputName)!;
        if (cb.filePath === '') {
            return cb.dynamicMethod!(input) ;
        }

        let method: Method | undefined;
        if (dynamicCBCache.has(inputName)) {
            method = dynamicCBCache.get(inputName);
        } else {
            method = getMethod(cb.filePath, cb.className, cb.methodName);
            dynamicCBCache.set(inputName, method);
        }
        return method!.invoke(null, [input]) as Any;
    }

    function getMethod(filePath: string, className: string, methodName: string): Method {
        className = filePath + '.' + className;
        let linker = Class.ofCaller()!.getLinker();
        let classType: ClassType | undefined = linker.getType(className) as ClassType;
        if (!classType) {
            throw new BusinessError(transferErrorCodeId,
                new Error(`Transfer Error. The class ${className} is not found!`));
        }
        for (let i = 0; i < classType!.getMethodsNum(); i++) {
            if (methodName === classType!.getMethod(i).getName()) {
                return classType!.getMethod(i);
            }
        }
        throw new BusinessError(transferErrorCodeId,
            new Error(`Transfer Error. The method ${methodName} is not found!`));
    }
}
