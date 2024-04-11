/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

declare function requireNapi(requireNapi: string): any;
// @ts-ignore
const {TextEncoder,TextDecoder} = requireNapi('util');

const DEFAULT_HIGH_WATER_MARK = 16 * 1024;
const DEFAULT_ENCODING = 'utf8';
const TypeErrorCodeId = 401;

class BusinessError extends Error {
  code: number | string;

  constructor(msg: string) {
    super(msg);
    this.name = 'BusinessError';
    this.code = TypeErrorCodeId;
  }
}

const ERR_DOWRITE_NOT_IMPLEMENTED = new BusinessError('The doWrite() method is not implemented');
const ERR_DOWRITEV_NOT_IMPLEMENTED = new BusinessError( 'The doWritev() method is not implemented');
const ERR_MULTIPLE_CALLBACK = new BusinessError('Callback called multiple times');

class EventEmitter {
  handlers: any;

  constructor() {
    this.handlers = {};
  }

  on(event: string, callback: (...args: any[]) => void): void {
    if (!this.handlers[event]) {
      this.handlers[event] = [];
    }
    this.handlers[event].push(callback);
  }

  off(event: string, callback: (...args: any[]) => void): void {
    if (this.handlers[event]) {
      let idx = this.handlers[event].findIndex((value: any): boolean => value === callback);
      if (idx !== -1) {
        this.handlers[event].splice(idx, 1);
      }
    }
  }

  emit(event: string, ...args: any[]): void {
    if (this.handlers[event]) {
      this.handlers[event].forEach((item: any) => {
        item(...args);
      });
    }
  }

  isOn(event: string): boolean {
    let handler = this.handlers[event];
    return handler && handler.length > 0;
  }
}

function runOnce(runFn: Function, callback?: (multipleTimes: boolean, error: Error) => void): Function {
  let executed = false;
  return function (...args: any[]) {
    if (!executed) {
      executed = true;
      return runFn(...args);
    } else {
      if (callback) {
        Promise.resolve().then(() => {
          // @ts-ignore
          callback();
        });
      }
    }
  };
}

function asyncFn(asyncFn: Function) {
  return function (...args: any[]): void {
    setTimeout(() => asyncFn(...args));
  };
}

enum ReadableEvent {
  CLOSE = 'close',
  DATA = 'data',
  END = 'end',
  ERROR = 'error',
  READABLE = 'readable',
  PAUSE = 'pause',
  RESUME = 'resume',
}

enum WritableEvent {
  CLOSE = 'close',
  DRAIN = 'drain',
  ERROR = 'error',
  FINISH = 'finish',
  PIPE = 'pipe',
  UNPIPE = 'unpipe',
}

class Readable {
  private buf: Array<number>;
  private listener: EventEmitter | undefined;
  private callbacks: any = {};
  private encoder = new TextEncoder();
  private decoder = new TextDecoder();
  private isInitialized: boolean = false;
  private _pause: boolean;
  private _pipeWritableArray: any[] = [];
  private _readableObjectMode: boolean | undefined;
  private _readable: boolean;
  private _readableHighWatermark: number;
  private _readableFlowing: boolean;
  private _readableLength: number;
  private _readableEncoding: string;
  private _readableEnded: boolean;
  private _errored: Error | undefined;
  private _closed: boolean | undefined;

  /**
   * The Readable constructor.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  constructor(options?: { encoding?: string | null; highWaterMark?: number; doRead?: (size: number) => void }) {
    this._readableEncoding = options?.encoding || DEFAULT_ENCODING;
    this._readableHighWatermark = options?.highWaterMark || DEFAULT_HIGH_WATER_MARK;
    this._readableObjectMode = false;
    this._readableLength = 0;
    this._pause = false;
    this._readableFlowing = true;
    this._readable = true;
    this._readableEnded = false;
    this.listener = new EventEmitter();
    this.buf = [];
    if (typeof options?.doRead === 'function') {
      this.doRead = options.doRead;
    }
  }

  /**
   * Returns boolean indicating whether it is in ObjectMode.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableObjectMode(): boolean | undefined {
    return this._readableObjectMode;
  }

  /**
   * Is true if it is safe to call readable.read(), which means
   * the stream has not been destroyed or emitted 'error' or 'end'.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readable(): boolean {
    if (this._readable) {
      return true;
    } else if (!this._readable && this._readableEnded) {
      return false;
    }
    return true;
  }

  /**
   * Returns the value of highWaterMark passed when creating this Readable.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableHighWatermark(): number {
    return this._readableHighWatermark;
  }

  /**
   * This property reflects the current state of the readable stream null/true/false.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableFlowing(): boolean {
    return this._readableFlowing;
  }

  /**
   * Size of the data that can be read, in bytes or objects.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableLength(): number {
    return this._readableLength;
  }

  /**
   * Getter for the property encoding of a given Readable stream. The encoding property can be set using the
   * readable.setEncoding() method.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableEncoding(): string | null {
    return this._readableEncoding;
  }

  /**
   * Whether all data has been generated.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableEnded(): boolean {
    return this._readableEnded;
  }

  /**
   * Returns error if the stream has been destroyed with an error.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get errored(): Error | undefined {
    return this._errored;
  }

  /**
   * Readable completes destroyfile and returns true, otherwise returns false.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get closed(): boolean {
    return this._closed || false;
  }

  private computeNewReadableHighWaterMark(readSize: number): number {
    readSize--;
    readSize |= readSize >>> 1;
    readSize |= readSize >>> 2;
    readSize |= readSize >>> 4;
    readSize |= readSize >>> 8;
    readSize |= readSize >>> 16;
    readSize++;
    return readSize;
  }

  setEndType() {
    this._readable = false;
    this._readableEnded = false;
    Promise.resolve().then((): void => this.listener?.emit(ReadableEvent.END));
  }


  /**
   * Reads a buffer of a specified size from the buffer. If the available buffer is sufficient, the result
   * of the specified size is returned. Otherwise, if Readable has ended, all remaining buffers are returned.
   *
   * @param { number } size - Expected length of the data to be read.
   * @returns { string | null } Read data from internal buffer and return. If there is no available data to
   * read, return null.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  read(size?: number): string | null {
    if (this.doRead === null && this._readable) {
      this._readable = false;
      Promise.resolve().then(() => {
        this._errored = new BusinessError('The doRead() method is not implemented');
        this.listener?.emit(ReadableEvent.ERROR, this._errored);
        this.listener?.emit(ReadableEvent.CLOSE);
      });
      return null;
    }
    size = size ?? this._readableLength;
    if (size > this._readableHighWatermark) {
      this._readableHighWatermark = this.computeNewReadableHighWaterMark(size);
    }
    if (size > this._readableLength) {
      if (!this._readableFlowing) {
        return null;
      } else {
        size = this._readableLength;
      }
    }
    let buffer = null;
    if (size > 0 && size <= this._readableLength) {
      let current = this.buf.slice(0, size);
      buffer = new Uint8Array(current);
      this.buf = this.buf.slice(size, this._readableLength);
      this._readableLength -= size;
      buffer = this.decoder.decode(buffer);
      this.doRead !== null && this.listener?.emit(ReadableEvent.DATA, buffer);
    }
    if ((!this._readable || size <= -1) && this._readableFlowing) {
      return null;
    }
    if (this._readableFlowing) {
      try {
        this.doRead(this._readableHighWatermark);
      } catch (error) {
        this._readable = false;
        this._readableEnded = true;
        this.listener?.emit(ReadableEvent.ERROR, error);
      }
    }
    return buffer;
  };

  /**
   * Switch Readable to Streaming Mode.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  resume(): boolean {
    if (this._readableLength === 0) {
      Promise.resolve().then(() => {
        this.read(this._readableHighWatermark < this._readableLength ? -1 : this._readableLength);
      });
    }
    this._pause = false;
    this._readableFlowing = true;
    this.listener?.emit(ReadableEvent.RESUME);
    return this._pause;
  }

  /**
   * Toggle Readable to Suspend Mode.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  pause(): boolean {
    this._pause = true;
    Promise.resolve().then(() => {
      this._readableFlowing = false;
    });
    this.listener?.emit(ReadableEvent.PAUSE);
    return this._pause;
  }

  /**
   * Sets the encoding format of the input binary data.Default: utf8.
   *
   * @param { string } [encoding] - Original Data Encoding Type.
   * @returns { boolean } Setting successful returns true, setting failed returns false.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  setEncoding(encoding?: string): boolean {
    if (encoding && encoding.toUpperCase() !== 'UTF8' && encoding.toUpperCase() !== 'UTF-8') {
      this._readable = false;
      let err = new BusinessError(`Unknown encoding: ${ encoding }`);
      this._errored = err;
      throw err;
    } else if (encoding && (encoding.toUpperCase() === 'UTF8' || encoding.toUpperCase() === 'UTF-8')) {
      this._readableEncoding = encoding;
      return true;
    } else {
      this._readableEncoding = 'utf8';
      return false;
    }
  }

  /**
   * Query whether it is in pause state.
   *
   * @returns { boolean } Pause state returns true, otherwise returns false.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  isPaused(): boolean {
    return this._pause;
  }

  /**
   * Concatenates a Writable to a Readable and switches the Readable to stream mode.
   *
   * @param { Writable } destination - Output writable stream.
   * @param { Object } [option] - Pipeline Options.
   * @returns { Writable } Returns the Writable object.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  pipe(destination: Writable, option?: Object): Writable {
    let obj = {
      write: destination,
      dataCallback: (data: any): void => {
        let flg = destination.write(data);
        if (!flg) {
          this.pause();
        }
      },
      drainCallback: (): void => {
      },
      endCallback: (): void => {
        destination.end();
      }
    };
    this._pipeWritableArray.push(obj);
    this.on(ReadableEvent.DATA, (data:any)=>{
      obj.dataCallback(data);
    });
    destination.on('drain', ()=>{
      obj.drainCallback();
    });
    destination.doWrite = (chunk:any, encoding, callback:any)=>{
      callback();
    }
    this.on(ReadableEvent.END, ()=>{
      obj.endCallback();
    });
    destination?.listener?.emit('pipe', this);
    return destination;
  }

  /**
   * Disconnect Writable from Readable.
   *
   * @param { Writable } [destination] - Writable Streams Needing to Be Disconnected.
   * @returns { Readable } Returns the Readable object.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  unpipe(destination?: Writable): Readable {
    let objIdx = this._pipeWritableArray.findIndex(value => value.write === destination);
    if (objIdx !== -1) {
      this._readable = false;
      let obj = this._pipeWritableArray[objIdx];
      this.listener?.off(ReadableEvent.DATA, obj.dataCallback!);
      destination?.listener?.off('drain', obj.drainCallback);
      this.listener?.off(ReadableEvent.END, obj.endCallback);
      destination?.listener?.emit('unpipe', this);
    }
    return this;
  }

  /**
   * Registering Event Messages.
   *
   * @param { string } event - Registering Events.
   * @param { Callback } callback - Event callback.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  on(event: string, callback: Function): void {
    let that = this;
    if (!this.isInitialized) {
      this.isInitialized = true;
      this.doInitialize?.(() => {
      });
    }
    this.callbacks[event] = this.callbacks[event] ?? [];
    let callbackFn = callback.bind(this);
    this.callbacks[event].push(callbackFn);
    this.listener?.on(event, callbackFn);
    Promise.resolve().then(() => {
      if (event === ReadableEvent.READABLE) {
        that._readableFlowing = false;
        if (this._readable) {
          that.doRead?.(this._readableHighWatermark);
        }
      } else if (event === ReadableEvent.DATA) {
        that._readableFlowing = true;
        if (!this._pause) {
          that.read();
        }
      }
    });
  }

  /**
   * Cancel event message.
   *
   * @param { string } event - Registering Events.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  off(event: string): void {
    this.callbacks[event]?.forEach((it: any) => {
      this.listener?.off(event, it);
    });
  }

  /**
   * Called by the Readable during initialization. It should not be called actively. Call callback () after the
   * resource has been initialized within the doInitialize, or call callback (err) when an error occurs.
   *
   * @param { Function } callback - Callback when the stream has completed the initial.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  doInitialize(callback: Function): void {
  }

  /**
   * The specific implementation of data production should not be actively called. Readable.read controls the
   * calling. After data production, Readable.push should be called to push the produced data into the buffer.
   * If push is not called, doRead will not be called again.
   *
   * @param { number } size -Expected length of the data to be read.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  doRead(size: number): void {
  };

  /**
   * Adds the generated data to the buffer. The return value indicates whether the data in the buffer has not
   * reached the highWaterMark (similar to Writable.write). If the chunk is null, all data has been generated.
   *
   * @param {  Uint8Array | string | null } chunk - Binary data to be stored in the buffer.
   * @param { string } [encoding] - Binary data encoding type.
   * @returns { boolean } If true is returned, the data in the buffer reaches the highWaterMark. Otherwise, the
   * data in the buffer does not reach the highWaterMark.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  push(chunk: Uint8Array | string | null, encoding?: string): boolean {
    let bufferArr: Uint8Array;
    if (encoding) {
      this.setEncoding(encoding);
    }
    if (typeof chunk === 'string' || chunk instanceof Uint8Array) {
      if (typeof chunk === 'string') {
        bufferArr = this.encoder.encode(chunk);
        this.buf.push(...bufferArr);
        this._readableLength += bufferArr.length;
      } else if (chunk instanceof Uint8Array) {
        this.buf.push(...chunk);
        this._readableLength += chunk.length;
      }
      let highWaterMark = this._readableLength < this._readableHighWatermark;
      Promise.resolve().then(() => {
        try {
          if (this._readableFlowing) {
            !this._pause && this.read(highWaterMark ? this._readableLength : -1);
          } else {
            this.doRead?.(this._readableHighWatermark);
          }
        } catch (error: any) {
          this.listener?.emit(ReadableEvent.ERROR, error);
        }
        this.listener?.emit(ReadableEvent.READABLE);
      });
      return highWaterMark;
    } else if (chunk === null) {
      if (!this._readableEnded && this._readable) {
        !this._readableFlowing && this.listener?.emit(ReadableEvent.READABLE);
        this._readable = false;
        Promise.resolve().then(() => {
          this._readableEnded = true;
          this._pause = true;
          this.listener?.emit(ReadableEvent.END);
          this.listener?.emit(ReadableEvent.CLOSE);
        });
      }
      return false;
    } else {
      this._readable = false;
      this._errored = new BusinessError("ERR_INVALID_ARG_TYPE");
      this.listener?.emit(ReadableEvent.ERROR, this._errored);
      return false;
    }
  };
}

// @ts-ignore
Readable.prototype.doRead = null;

class Writable {
  public listener: EventEmitter | undefined;
  private callbacks: any = {};
  private buffer: any[] = [];
  private writing: boolean = false;
  private encoding: string | undefined;
  private encoder = new TextEncoder();
  private ending: boolean = false;
  private _writableObjectMode: boolean | undefined;
  private _writableHighWaterMark: number;
  private _writable: boolean | undefined;
  private _writableLength: number | undefined;
  private _writableNeedDrain: boolean | undefined;
  private _writableCorked: number = 0;
  private _writableEnded: boolean | undefined;
  private _writableFinished: boolean | undefined;
  private _errored: Error | undefined | null;
  private _closed: boolean | undefined;

  constructor(options?: {
    highWaterMark?: number | undefined;
    objectMode?: boolean | undefined;
  }) {
    this.listener = new EventEmitter();
    if (!options) {
      options = {
        highWaterMark: DEFAULT_HIGH_WATER_MARK,
        objectMode: false,
      };
    }
    this._writableHighWaterMark = options.highWaterMark ?? DEFAULT_HIGH_WATER_MARK;
    this._writableObjectMode = options.objectMode || false;
    this._writableLength = 0;
    this._writableEnded = false;
    this._writableNeedDrain = false;
    this._writable = true;
    this._writableCorked = 0;
    this._writableFinished = false;
    this._errored = null;
    this.encoding = 'utf8';
    this._closed = false;
    this.doInitialize((error: any) => {
      if (error) {
        this.listener?.emit(WritableEvent.ERROR, error);
      }
    });
  }

  /**
   * Returns boolean indicating whether it is in ObjectMode.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableObjectMode(): boolean | undefined {
    return this._writableObjectMode;
  }

  /**
   * Value of highWaterMark.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableHighWaterMark(): number | undefined {
    return this._writableHighWaterMark;
  }

  /**
   * Is true if it is safe to call writable.write(), which means the stream has not been destroyed, errored, or ended.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writable(): boolean | undefined {
    return this._writable;
  }

  /**
   * Size of data that can be flushed, in bytes or objects.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableLength(): number | undefined {
    return this._writableLength;
  }

  /**
   * If the buffer of the stream is full and true, otherwise it is false.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableNeedDrain(): boolean | undefined {
    return this._writableNeedDrain;
  }

  /**
   * Number of times writable.uncork() needs to be called in order to fully uncork the stream.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableCorked(): number | undefined {
    return this._writableCorked;
  };

  /**
   * Whether Writable.end has been called.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableEnded(): boolean | undefined {
    return this._writableEnded;
  }

  /**
   * Whether Writable.end has been called and all buffers have been flushed.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableFinished(): boolean | undefined {
    return this._writableFinished;
  }

  /**
   * Returns error if the stream has been destroyed with an error.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get errored(): Error | undefined | null {
    return this._errored;
  }

  /**
   * Writable completes destroyfile and returns true, otherwise returns false.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get closed(): boolean | undefined {
    return this._closed;
  }

  /**
   * writes a chunk to Writable and invokes callback when the chunk is flushed. The return value indicates
   * whether the internal buffer of the Writable reaches the hightWaterMark. If true is returned, the buffer
   * does not reach the hightWaterMark. If false is returned, the buffer has been reached. The write function
   * should be called after the drain event is triggered. If the write function is called continuously,
   * the chunk is still added to the buffer until the memory overflows
   *
   * @param { string | Uint8Array } [chunk] - Data to be written.
   * @param { string } [encoding] - Encoding type.
   * @param { Function } [callback] - Callback after writing.
   * @returns { boolean } Write success returns true, write failure returns false.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  write(chunk?: string | Uint8Array, encoding?: string, callback?: Function): boolean {
    if (encoding) {
      this.setDefaultEncoding(encoding);
    }
    if (chunk === null) {
      throw new BusinessError(`The "chunk" argument must be of type string or UintArray`);
    }
    if (typeof chunk !== 'string' && !(chunk instanceof Uint8Array)) {
      throw new BusinessError(`The "chunk" argument must be of type string or UintArray`);
    }
    if (this.ending && !this.writing) {
      setTimeout(() => {
        let error = new Error('write after end');
        callback?.(error);
        this.throwError(error);
      });
      return false;
    }
    if (this._errored) {
      return false;
    }
    let flag = false;
    if (chunk instanceof Uint8Array) {
      flag = this.writeUint8Array(chunk, encoding ?? this.encoding, callback);
    } else {
      flag = this.writeString(chunk!, encoding ?? this.encoding, callback);
    }
    if (!flag) {
      this._writableNeedDrain = true;
    }
    return flag;
  }

  private getChunkLength(chunk: string | Uint8Array): number {
    if (chunk instanceof Uint8Array) {
      return chunk.byteLength;
    } else {
      return this.encoder.encode(chunk).byteLength;
    }
  }

  private writeUint8Array(chunk: Uint8Array, encoding?: string, callback?: Function): boolean {
    this._writableLength! += this.getChunkLength(chunk);
    let hasRemaining = this._writableLength! < this.writableHighWaterMark!;
    const fnBack = runOnce((error: any) => {
      if (error && error instanceof Error) {
        this._writable = false;
        this.throwError(error);
        return;
      }
      callback?.(error ?? null);
      this.freshCache();
    }, (multipleTimes: boolean, err: Error) => {
      this.listener?.emit(WritableEvent.ERROR, ERR_MULTIPLE_CALLBACK);
    });
    if (this._writableCorked === 0) {
      if (!this.writing) {
        this.writing = true;
        this.doWrite(chunk, encoding ?? 'utf8', fnBack);
      } else {
        this.buffer.push({ chunk: chunk, encoding: encoding, callback: fnBack });
      }
    } else {
      this.buffer.push({ chunk: chunk, encoding: encoding, callback: fnBack });
    }
    return hasRemaining;
  }

  private writeString(chunk: string, encoding?: string, callback?: Function): boolean {
    let that = this;
    this._writableLength! += this.getChunkLength(chunk);
    let hasRemaining = this._writableLength! < this.writableHighWaterMark!;
    const fb = runOnce((error: any) => {
      if (error) {
        this._errored = error;
      }
      callback?.(error ?? null);
      that.freshCache();
      if (error && error instanceof Error) {
        this._writable = false;
        this._errored = error;
        Promise.resolve().then(() => {
          if (this.isOnError()) {
            this.emitErrorOnce(error);
          } else {
            this.emitErrorOnce(error);
            throw error;
          }
        });
        return;
      }
    }, () => {
      this.emitErrorOnce(ERR_MULTIPLE_CALLBACK, true);
    });

    if (this._writableCorked === 0) {
      if (!this.writing) {
        this.writing = true;
        this.doWrite?.(chunk, encoding ?? 'utf8', fb);
        if (!this.doWrite && !hasRemaining) {
          Promise.resolve().then(() => {
            this._writableLength = 0;
            this.listener?.emit(WritableEvent.DRAIN);
          });
        }
      } else {
        this.buffer.push({ chunk: chunk, encoding: encoding, callback: fb });
      }
    } else {
      this.buffer.push({ chunk: chunk, encoding: encoding, callback: fb });
    }
    return this._errored ? false : hasRemaining;
  }

  private freshCache(): void {
    let current = this.buffer.shift();
    if (current) {
      this.doWrite?.(current.chunk, current.encoding ?? 'utf8', current.callback);
      this._writableLength! -= this.getChunkLength(current.chunk);
    } else {
      this.writing = false;
      this._writableLength = 0;
      if (!this.finishMayBe()) {
        this._writableNeedDrain = false;
        this.listener?.emit(WritableEvent.DRAIN);
      }
    }
  }

  private freshCacheV(): void {
    if (this.buffer.length > 0) {
      if (this.doWritev) {
        const that = this;
        const funCallback = runOnce((error: any) => {
          if (error && error instanceof Error) {
            that._errored = error;
            that.listener?.emit(WritableEvent.ERROR, error);
            return;
          }
          this.buffer = [];
        }, () => {
          this.listener?.emit(WritableEvent.ERROR, ERR_MULTIPLE_CALLBACK);
        });
        this.doWritev(this.buffer.map((item) => {
          return item.chunk;
        }), funCallback);
        if (!this.finishMayBe()) {
          this._writableNeedDrain = true;
          this.listener?.emit(WritableEvent.DRAIN);
        }
      } else {
        this.freshCache();
      }
    }
  }

  /**
   * Write the last chunk to Writable.
   *
   * @param { string | Uint8Array } [chunk] - Data to be written.
   * @param { string } [encoding] - Encoding type.
   * @param { Function } [callback] - Callback after writing.
   * @returns { Writable } Returns the Writable object.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  end(chunk?: string | Uint8Array, encoding?: string, callback?: Function): Writable {
    if (this._writableFinished) {
      this._errored = new BusinessError("stream already finished");
      setTimeout(() => callback?.(this._errored))
      this.emitErrorOnce(this._errored);
      return this;
    } else if (this._writableEnded) {
      this._errored = new BusinessError("write after end");
      setTimeout(() => callback?.(this._errored));
      this.emitErrorOnce(this._errored);
      return this;
    }
    let that = this;
    if (that._errored) {
      setTimeout(() => callback?.(that._errored));
      return this;
    }
    this._writableNeedDrain = false;
    this._closed = true;
    that.ending = true;
    this._writable = false;
    if (!that._writableEnded) {
      if (that._writableCorked === 0) {
        if (chunk) {
          if (that.writing) {
            that.write(chunk, encoding, callback);
          } else {
            that.doWrite?.(chunk!, encoding ?? 'utf8', (error: any) => {
              if (error && error instanceof Error) {
                this._errored = error;
                this.listener?.emit(WritableEvent.ERROR, error);
              } else {
                that._writableLength! -= this.getChunkLength(chunk);
                that.writing = false;
                that.finishMayBe();
              }
              that._writableEnded = true;
              that._writable = false;
              asyncFn(() => {
                callback?.(that._errored ?? error ?? null);
              })();
              if (!that._writableFinished) {
                that._writableFinished = true;
                asyncFn(() => {
                  if ((!that._errored || that._errored.message === 'write after end') && !this.isOnError()) {
                    that.listener?.emit(WritableEvent.FINISH);
                  }
                })();
              }
            });
          }
        } else {
          if (that._writableEnded) {
            that._errored = new BusinessError('write after end');
            callback?.(that._errored);
          } else {
            setTimeout(() => callback?.(this._errored));
          }
          if (!that._writableFinished && !that._writableEnded) {
            that._writableFinished = true;
            asyncFn(() => {
              if (!that._errored || that._errored.message === 'write after end') {
                that.listener?.emit(WritableEvent.FINISH);
              }
            })();
          }
        }
      } else {
        that._writableCorked = 1;
        that.uncork();
      }
    }
    this._writableEnded = true;
    that.listener?.emit(WritableEvent.CLOSE);
    return that;
  }

  private finishMayBe(): boolean {
    return !this.writing && this._writableCorked === 0 && this.ending;
  }

  /**
   * Set the default encoding mode.
   *
   * @param { string } [encoding] - Encoding type.Default: utf8.
   * @returns { boolean } Setting successful returns true, setting failed returns false.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  setDefaultEncoding(encoding?: string): boolean {
    if (encoding && encoding.toUpperCase() === 'UTF8') {
      this.encoding = encoding.toLowerCase();
      return true;
    } else if (encoding && encoding.toUpperCase() === 'ASCII') {
      this.encoding = encoding.toLowerCase();
      return true;
    } else {
      let err = new BusinessError(`Unknown encoding: ${ encoding }`);
      this.listener?.emit(WritableEvent.ERROR, err);
      this.throwError(err);
      return false;
    }
  }

  /**
   * After the call, all Write operations will be forced to write to the buffer instead of being flushed.
   *
   * @returns { boolean } Setting successful returns true, setting failed returns false.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  cork(): boolean {
    this._writableCorked += 1;
    return true;
  }

  /**
   * After calling, flush all buffers.
   *
   * @returns { boolean } Setting successful returns true, setting failed returns false.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  uncork(): boolean {
    if (this._writableCorked > 0) {
      this._writableCorked -= 1;
    }
    if (this._writableCorked === 0) {
      this.freshCacheV();
    }
    return true;
  }

  /**
   * Registering Event Messages.
   *
   * @param { string } event - Register Event.
   * @param { Callback } callback - event callbacks.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  on(event: string, callback: Function): void {
    this.callbacks[event] = this.callbacks[event] ?? [];
    let callbackFn = callback.bind(this);
    this.callbacks[event].push(callbackFn);
    this.listener?.on(event, callbackFn);
  }

  /**
   * Cancel event message.
   *
   * @param { string } event - Register Event.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  off(event: string): void {
    this.callbacks[event]?.forEach((it: any) => {
      this.listener?.off(event, it);
    });
  }

  noWriteOpes(chunk: string | Uint8Array, encoding: string, callback: Function): void {
    if (this.doWritev === null) {
      this.throwError(ERR_DOWRITE_NOT_IMPLEMENTED);
    } else {
      this.doWritev([chunk] as any, callback);
    }
  }

  /**
   * This method is invoked by the Writable method during initialization and should not be invoked actively.
   * After the resource is initialized in the doInitialize method, the callback () method is invoked.
   *
   * @param { Function } callback - Callback when the stream has completed the initial.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  doInitialize(callback: Function): void {
  }

  /**
   * Implemented by subclass inheritance. The implementation logic of flushing chunks in the buffer should not be
   * actively called. The call is controlled by Writable.write.
   *
   * @param { string | Uint8Array } chunk - Data to be written.
   * @param { string } encoding - Encoding type.
   * @param { Function } callback - Callback after writing.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  doWrite(chunk: string | Uint8Array, encoding: string, callback: Function): void {
    throw ERR_DOWRITE_NOT_IMPLEMENTED;
  }

  /**
   * The implementation logic of flushing chunks in the buffer in batches should not be actively called.
   * The call is controlled by Writable.write.
   *
   * @param { string[] | Uint8Array[] } chunk - Data to be written.
   * @param { Function } callback - Callback after writing.
   * @returns { Writable } Returns the Writable object.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  doWritev(chunk: string[] | Uint8Array[], callback: Function): void {
    throw ERR_DOWRITEV_NOT_IMPLEMENTED;
  }

  throwError(error: Error): void {
    this._errored = error;
    if (this.listener?.isOn(WritableEvent.ERROR)) {
      setTimeout(() => {
        this.listener?.emit(WritableEvent.ERROR, error);
      });
    } else {
      throw error;
    }
  }

  private isOnError(): boolean {
    return this.listener?.isOn(WritableEvent.ERROR) || false;
  }

  private _emitErrorExecuted = false;
  private _emitErrorId: any;

  private emitErrorOnce(error: Error, reset?: boolean): void {
    if (reset) {
      this._emitErrorExecuted = false;
      clearTimeout(this._emitErrorId);
    }
    if (!this._emitErrorExecuted) {
      this._emitErrorExecuted = true;
      this._emitErrorId = setTimeout(() => {
        this.listener?.emit(WritableEvent.ERROR, this._errored ?? error);
      });
    }
  }
}

Writable.prototype.doWrite = Writable.prototype.noWriteOpes;
// @ts-ignore
Writable.prototype.doWritev = null;


class Duplex extends Readable {
  private _writable: Writable;

  constructor() {
    super();
    this._writable = new Writable();
    let that = this;
    this._writable.doWrite = this.doWrite;
    this._writable.doWritev = this.doWritev;
    Object.defineProperties(that, {
      doWrite: {
        get(): any {
          return that._writable.doWrite;
        },
        set(value: any) {
          that._writable.doWrite = value;
        }
      },
      doWritev: {
        get(): any {
          return that._writable.doWritev;
        },
        set(value: any) {
          that._writable.doWritev = value;
        }
      }
    });
  }

  write(chunk?: string | Uint8Array, encoding?: string, callback?: Function): boolean {
    return this._writable.write(chunk, encoding, callback);
  }

  end(chunk?: string | Uint8Array, encoding?: string, callback?: Function): Writable {
    super.setEndType();
    return this._writable.end(chunk, encoding, callback);
  }

  on(event: string, callback: Function): void {
    super.on(event, callback);
    this._writable.on(event, callback);
  }

  off(event: string): void {
    super.off(event);
    this._writable.off(event);
  }

  getListener(): EventEmitter | undefined {
    return this._writable.listener;
  }

  setDefaultEncoding(encoding?: string): boolean {
    return this._writable.setDefaultEncoding(encoding);
  }

  cork(): boolean {
    return this._writable.cork();
  }

  uncork(): boolean {
    return this._writable.uncork();
  }

  doInitialize(callback: Function): void {
    super.doInitialize(callback);
    this._writable.doInitialize(callback);
  }

  doWrite(chunk: string | Uint8Array, encoding: string, callback: Function): void {
  }

  doWritev(chunk: string[] | Uint8Array[], callback: Function): void {
    this._writable.doWritev?.(chunk, callback);
  }

  get writableObjectMode(): boolean {
    return this._writable.writableObjectMode || false;
  }

  get writableHighWaterMark(): number {
    return this._writable.writableHighWaterMark || 0;
  }

  get writable(): boolean {
    return this._writable.writable || false;
  }

  get writableLength(): number {
    return this._writable.writableLength || 0;
  }

  get writableNeedDrain(): boolean {
    return this._writable.writableNeedDrain || false;
  }

  get writableCorked(): number {
    return this._writable.writableCorked || 0;
  }

  get writableEnded(): boolean {
    return this._writable.writableEnded || false;
  }

  get writableFinished(): boolean {
    return this._writable.writableFinished || false;
  }
}

// @ts-ignore
Duplex.prototype.doWrite = null;
// @ts-ignore
Duplex.prototype.doWritev = null;


class Transform extends Duplex {
  /**
   * The Transform constructor.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  constructor() {
    super();
  }

  on(event: string, callback: Function): void {
    super.on(event, callback);
  }

  end(chunk?: string | Uint8Array, encoding?: string, callback?: Function): Writable {
    if (!this.doTransform) {
      throw new BusinessError('The doTransform() method is not implemented');
    }
    if (chunk instanceof Uint8Array) {
      let chunkString = new TextDecoder().decode(chunk);
      this.doTransform(chunkString, encoding || 'utf8', callback || ((): void => {
      }));
    } else if (typeof chunk === 'string') {
      this.doTransform(chunk, encoding || 'utf8', callback || ((): void => {
      }));
    }
    let write = super.end(chunk, encoding, callback);
    this.doFlush?.((arg1: any, arg2: any) => {
      this.push(arg2 ?? '', encoding);
    });
    return write;
  }

  push(chunk: Uint8Array | string | null, encoding?: string): boolean {
    return super.push(chunk, encoding);
  }

  /**
   * Convert the input data. After the conversion, Transform.push can be called to send the input to the read stream.
   * Transform.push should not be called Transform.write to call.
   *
   * @param { string } chunk - Input data to be converted.
   * @param { string } encoding - If the chunk is a string, then this is the encoding type. If chunk is a buffer,
   * then this is the special value 'buffer'. Ignore it in that case.
   * @param { Function } callback - Callback after conversion.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  doTransform(chunk: string, encoding: string, callback: Function): void {
    throw new BusinessError("The doTransform() method is not implemented");
  }

  /**
   * After all data is flushed to the write stream, you can use the Transform.doFlush writes some extra data, should
   * not be called actively, only called by Writable after flushing all data.
   *
   * @param { Function } callback - Callback after flush completion.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  doFlush(callback: Function): void {
  }

  write(chunk?: string | Uint8Array, encoding?: string, callback?: Function): boolean {
    if (typeof chunk === 'string') {
      let callBackFunction = runOnce((error:Error) => {
        if(error){
          this.getListener()?.emit(WritableEvent.ERROR, error);
        }
      }, () => {
        let err = new BusinessError('Callback called multiple times');
        this.getListener()?.emit(WritableEvent.ERROR, err);
      });
      this.doTransform(chunk ?? '', encoding ?? 'utf8', callBackFunction);
    }
    return super.write(chunk, encoding, callback);
  }

  doRead(size: number): void {
  }
}

// @ts-ignore
Transform.prototype.doTransform = null;
// @ts-ignore
Transform.prototype.doFlush = null;

export default {
  Readable: Readable,
  Writable: Writable,
  Duplex: Duplex,
  Transform: Transform,
};
