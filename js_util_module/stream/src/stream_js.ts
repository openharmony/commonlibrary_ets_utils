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

declare function requireNapi(napiModuleName: string): any;
const emitter = requireNapi('events.emitter');
// @ts-ignore
const { TextEncoder, TextDecoder } = requireNapi('util');

const DEFAULT_HIGH_WATER_MARK = 16 * 1024;
const DEFAULT_ENCODING = 'utf-8';
const TypeErrorCodeId = 401;
class BusinessError extends Error {
  code: number | string;

  constructor(msg: string, code?: number) {
    super(msg);
    this.name = 'BusinessError';
    this.code = code ? code : TypeErrorCodeId;
  }
}
const ERR_DOWRITE_NOT_IMPLEMENTED:BusinessError = new BusinessError('The doWrite() method is not implemented', 10200035);
const ERR_DOWRITEV_NOT_IMPLEMENTED:BusinessError = new BusinessError('The doWritev() method is not implemented', 10200035);
const ERR_MULTIPLE_CALLBACK:BusinessError = new BusinessError('Callback called multiple times', 10200037);
const ERR_STREAM_ALREADY_FINISHED:BusinessError = new BusinessError('stream already finished', 10200036);
const ERR_WRITE_AFTER_END:BusinessError = new BusinessError('write after end', 10200036);
const ENCODING_SET: Array<string> = ['ascii', 'utf-8', 'UTF-8', 'gbk', 'GBK', 'GB2312', 'gb2312',
  'GB18030', 'gb18030', 'ibm866', 'iso-8859-2', 'iso-8859-3',
  'iso-8859-4', 'iso-8859-5', 'iso-8859-6', 'iso-8859-7',
  'iso-8859-8', 'iso-8859-8-i', 'iso-8859-10', 'iso-8859-13',
  'iso-8859-14', 'iso-8859-15', 'koi8-r', 'koi8-u', 'macintosh',
  'windows-874', 'windows-1250', 'windows-1251', 'windows-1252',
  'windows-1253', 'windows-1254', 'windows-1255', 'windows-1256',
  'windows-1257', 'windows-1258', 'big5', 'euc-jp', 'iso-2022-jp',
  'shift_jis', 'euc-kr', 'x-mac-cyrillic', 'utf-16be',
  'utf-16le'];
class EventEmitter {
  handlers: { [key: string]: Function[] };

  constructor() {
    this.handlers = {};
  }

  on(event: string, callback: Function): void {
    if (!this.handlers[event]) {
      this.handlers[event] = [];
    }
    this.handlers[event].push(callback);
  }

  off(event: string, callback: Function): void {
    if (this.handlers[event]) {
      const idx = this.handlers[event].findIndex((value: Function): boolean => value === callback);
      if (idx !== -1) {
        this.handlers[event].splice(idx, 1);
      }
    }
  }

  emit(event: string, ...args: any[]): void {
    if (this.handlers[event]) {
      this.handlers[event].forEach((item: any) => {
        if (args.length > 0) {
          item({ data: args[0] });
        } else {
          item({});
        }
      });
    }
  }

  isOn(event: string): boolean {
    const handler:Function[] = this.handlers[event];
    return handler && handler.length > 0;
  }

  listenerCount(event: string): number {
    return this.handlers[event]?.length || 0;
  }
}
function runOnce(runFn: Function, callback?: (multipleTimes: boolean, error: Error) => void): Function {
  let executed = false;
  return function (...args: Function[]) {
    if (!executed) {
      executed = true;
      return runFn(...args);
    } else {
      if (callback) {
        Promise.resolve().then(():void => {
          // @ts-ignore
          callback();
        });
      }
    }
  };
}
function asyncFn(asyncFn: Function) {
  return function (...args: Function[]): void {
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

interface ReadablePipeStream {
  write: Writable;
  dataCallback: Function;
  drainCallback: Function;
  endCallback: Function;
}
class Readable {
  private buf: Array<number>;
  private listener: EventEmitter | undefined;
  private callbacks: { [key: string]: Function[] } = {};
  protected encoder = new TextEncoder();
  protected decoder = new TextDecoder();
  private isInitialized: boolean = false;
  private pauseInner: boolean;
  private pipeWritableArrayInner: ReadablePipeStream[] = [];
  private readableObjectModeInner: boolean | undefined;
  private readableInner: boolean;
  private readableHighWatermarkInner: number;
  private readableFlowingInner: boolean;
  private readableLengthInner: number;
  private readableEncodingInner: string;
  private readableEndedInner: boolean;
  private erroredInner: Error | undefined;
  private closedInner: boolean | undefined;

  /**
   * The Readable constructor.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  constructor(options?: { encoding?: string | null; highWatermark?: number; doRead?: (size: number) => void }) {
    this.readableEncodingInner = options?.encoding || DEFAULT_ENCODING;
    this.readableHighWatermarkInner = options?.highWatermark || DEFAULT_HIGH_WATER_MARK;
    this.readableObjectModeInner = false;
    this.readableLengthInner = 0;
    this.pauseInner = false;
    this.readableFlowingInner = true;
    this.readableInner = true;
    this.readableEndedInner = false;
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
    return this.readableObjectModeInner;
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
    if (this.readableInner) {
      return true;
    } else if (!this.readableInner && this.readableEndedInner) {
      return false;
    }
    return true;
  }

  /**
   * Returns the value of highWatermark passed when creating this Readable.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableHighWatermark(): number {
    return this.readableHighWatermarkInner;
  }

  /**
   * This property reflects the current state of the readable stream null/true/false.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableFlowing(): boolean {
    return this.readableFlowingInner;
  }

  /**
   * Size of the data that can be read, in bytes or objects.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableLength(): number {
    return this.readableLengthInner;
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
    return this.readableEncodingInner;
  }

  /**
   * Whether all data has been generated.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get readableEnded(): boolean {
    return this.readableEndedInner;
  }

  /**
   * Returns error if the stream has been destroyed with an error.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get errored(): Error | undefined {
    return this.erroredInner;
  }

  /**
   * Readable completes destroyfile and returns true, otherwise returns false.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get closed(): boolean {
    return this.closedInner || false;
  }

  private computeNewReadableHighWatermark(readSize: number): number {
    readSize--;
    readSize |= readSize >>> 1;
    readSize |= readSize >>> 2;
    readSize |= readSize >>> 4;
    readSize |= readSize >>> 8;
    readSize |= readSize >>> 16;
    readSize++;
    return readSize;
  }

  setEndType(): void {
    Promise.resolve().then((): void => {
      this.readableInner = false;
      this.readableEndedInner = true;
      this.listener?.emit(ReadableEvent.END);
    });
  }

  /**
   * Reads a buffer of a specified size from the buffer. If the available buffer is sufficient, the result
   * of the specified size is returned. Otherwise, if Readable has ended, all remaining buffers are returned.
   *
   * @param { number } size - Expected length of the data to be read.
   * @returns { string | null } If no data is available to read, null is returned.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @throws { BusinessError } 10200038 - if the doRead method has not been implemented, an error will be thrown.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  read(size?: number): string | null {
    if (size && typeof size !== 'number') {
      this.throwError(new BusinessError('The size parameter is invalid', 401));
      return null;
    }
    if (this.doRead === null && this.readableInner) {
      this.readableInner = false;
      Promise.resolve().then(() => {
        this.closedInner = true;
        this.erroredInner = new BusinessError('The doRead() method is not implemented', 10200038);
        this.listener?.emit(ReadableEvent.ERROR, this.erroredInner);
        this.listener?.emit(ReadableEvent.CLOSE);
      });
      return null;
    }
    size = size ?? this.readableLengthInner;
    if (size > this.readableHighWatermarkInner) {
      this.readableHighWatermarkInner = this.computeNewReadableHighWatermark(size);
    }
    if (size > this.readableLengthInner) {
      if (!this.readableFlowingInner) {
        return null;
      } else {
        size = this.readableLengthInner;
      }
    }
    let buffer = null;
    if (size > 0 && size <= this.readableLengthInner) {
      this.readableLengthInner -= size;
      buffer = this.decoder.decodeWithStream(new Uint8Array(this.buf.splice(0, size)));
      this.doRead !== null && this.listener?.emit(ReadableEvent.DATA, buffer);
    }
    if ((!this.readableInner || size <= -1) && this.readableFlowingInner) {
      return null;
    }
    if (this.readableFlowingInner) {
      try {
        this.doRead(this.readableHighWatermarkInner);
      } catch (error) {
        this.readableInner = false;
        this.readableEndedInner = true;
        this.listener?.emit(ReadableEvent.ERROR, error);
        this.listener?.emit(ReadableEvent.CLOSE);
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
  resume(): Readable {
    if (this.readableLengthInner === 0) {
      Promise.resolve().then((): void => {
        this.read(this.readableHighWatermarkInner < this.readableLengthInner ? -1 : this.readableLengthInner);
      });
    }
    this.pauseInner = false;
    this.readableFlowingInner = true;
    this.listener?.emit(ReadableEvent.RESUME);
    return this;
  }

  /**
   * Toggle Readable to Suspend Mode.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  pause(): Readable {
    this.pauseInner = true;
    Promise.resolve().then((): void => {
      this.readableFlowingInner = false;
    });
    this.listener?.emit(ReadableEvent.PAUSE);
    return this;
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
    if (!encoding) {
      this.readableEncodingInner = 'utf8';
      return false;
    }
    if (encoding.toLowerCase() === 'utf8') {
      encoding = 'utf-8';
    }
    if (ENCODING_SET.indexOf(encoding.toLowerCase()) !== -1) {
      try {
        this.encoder = new TextEncoder(encoding);
        this.decoder = new TextDecoder(encoding);
        this.readableEncodingInner = encoding.toLowerCase();
      } catch (e) {
        this.throwError(e as Error);
        return false;
      }
      return true;
    } else {
      const err: BusinessError = new BusinessError(`Unknown encoding: ${encoding}`);
      this.throwError(err);
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
    return this.pauseInner;
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
    this.pauseInner = false;
    const obj: ReadablePipeStream = {
      write: destination,
      dataCallback: (data: { data: string | Uint8Array }): void => {
        destination.write(data.data);
        if ((destination.writableLength || 0) > (destination.writableHighWatermark || DEFAULT_HIGH_WATER_MARK)) {
          this.pauseInner = true;
          this.readableFlowingInner = false;
        }
      },
      drainCallback: (): void => {
        this.pauseInner = false;
        this.readableFlowingInner = true;
        this.read(this.readableLengthInner);
      },
      endCallback: (): void => {
        destination.end();
      }
    };
    this.pipeWritableArrayInner.push(obj);
    this.on(ReadableEvent.DATA, (data: { data: Function }) => {
      obj.dataCallback(data);
    });
    destination.on('drain', (): void => {
      obj.drainCallback();
    });
    this.on(ReadableEvent.END, (): void => {
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
    const objIdx: number = this.pipeWritableArrayInner.findIndex((value: ReadablePipeStream) => value.write === destination);
    if (objIdx !== -1) {
      this.readableInner = false;
      const obj: ReadablePipeStream = this.pipeWritableArrayInner[objIdx];
      this.listener?.off(ReadableEvent.DATA, obj.dataCallback);
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
    const that = this;
    if (!this.isInitialized) {
      this.isInitialized = true;
      this.doInitialize?.(() => {
      });
    }
    this.callbacks[event] = this.callbacks[event] ?? [];
    const callbackFn = callback;
    this.callbacks[event].push(callbackFn);
    this.listener?.on(event, callbackFn);
    Promise.resolve().then((): void => {
      if (event === ReadableEvent.READABLE) {
        this.readableFlowingInner = false;
        if (this.readableInner) {
          this.doRead?.(this.readableHighWatermarkInner);
        }
      } else if (event === ReadableEvent.DATA) {
        this.readableFlowingInner = true;
        if (!this.pauseInner) {
          this.read();
        }
      }
    });
  }

  /**
   * Cancel event message.
   *
   * @param { string } event - Registering Events.
   * @param { Callback<emitter.EventData> } callback - Event callback.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  off(event: string, callback?: Function): void {
    if (!event) {
      this.throwError(new BusinessError('event is null', 401));
      return;
    }
    if (event && typeof event !== 'string') {
      this.throwError(new BusinessError('event is not string', 401));
      return;
    }
    if (callback) {
      this.callbacks[event]?.forEach((it: Function): void => {
        if (callback === it) {
          this.listener?.off(event, it);
        }
      });
    } else {
      this.callbacks[event]?.forEach((it : Function) => this.listener?.off(event, it));
    }
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
        bufferArr = this.encoder.encodeInto(chunk);
        this.buf.push(...bufferArr);
        this.readableLengthInner += bufferArr.length;
      } else if (chunk instanceof Uint8Array) {
        this.buf.push(...chunk);
        this.readableLengthInner += chunk.length;
      }
      const highWaterMark = this.readableLengthInner <= this.readableHighWatermarkInner;
      Promise.resolve().then((): void => {
        try {
          if (this.readableFlowingInner) {
            !this.pauseInner && this.read(highWaterMark ? this.readableLengthInner : -1);
          } else {
            if (highWaterMark) {
              this.doRead?.(this.readableHighWatermarkInner);
            }
          }
        } catch (error) {
          this.listener?.emit(ReadableEvent.ERROR, error);
          this.listener?.emit(ReadableEvent.CLOSE);
        }
        this.listener?.emit(ReadableEvent.READABLE);
      });
      return this.readableLengthInner < this.readableHighWatermarkInner;
    } else if (chunk === null) {
      if (!this.readableEndedInner && this.readableInner) {
        !this.readableFlowingInner && this.listener?.emit(ReadableEvent.READABLE);
        this.readableInner = false;
        Promise.resolve().then((): void => {
          this.readableEndedInner = true;
          this.pauseInner = true;
          this.closedInner = true;
          this.listener?.emit(ReadableEvent.END);
          this.listener?.emit(ReadableEvent.CLOSE);
        });
      }
      return false;
    } else {
      this.readableInner = false;
      this.erroredInner = new BusinessError('ERR_INVALID_ARG_TYPE');
      this.listener?.emit(ReadableEvent.ERROR, this.erroredInner);
      return false;
    }
  };

  throwError(error: Error): void {
    this.erroredInner = error;
    if (this.listener && this.listener.listenerCount(WritableEvent.ERROR) > 0) {
      setTimeout((): void => {
        this.listener?.emit(WritableEvent.ERROR, error);
      });
    } else {
      throw error;
    }
  }
}

// @ts-ignore
Readable.prototype.doRead = null;

class Writable {
  public listener: EventEmitter | undefined;
  private callbacks: { [key: string]: Function[] } = {};
  private buffer: ({ encoding?: string, chunk: string | Uint8Array, callback: Function })[] = [];
  private writing: boolean = false;
  private encoding: string | undefined;
  protected encoder = new TextEncoder();
  private ending: boolean = false;
  private writableObjectModeInner: boolean | undefined;
  private writableHighWatermarkInner: number;
  private writableInner: boolean | undefined;
  private writableLengthInner: number | undefined;
  private writableNeedDrainInner: boolean | undefined;
  private writableCorkedInner: number = 0;
  private writableEndedInner: boolean | undefined;
  private writableFinishedInner: boolean | undefined;
  private erroredInner: Error | undefined | null;
  private closedInner: boolean | undefined;

  /**
   * The Writable constructor.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
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
    this.writableHighWatermarkInner = options.highWaterMark ?? DEFAULT_HIGH_WATER_MARK;
    this.writableObjectModeInner = options.objectMode || false;
    this.writableLengthInner = 0;
    this.writableEndedInner = false;
    this.writableNeedDrainInner = false;
    this.writableInner = true;
    this.writableCorkedInner = 0;
    this.writableFinishedInner = false;
    this.erroredInner = null;
    this.encoding = 'utf8';
    this.closedInner = false;
    // @ts-ignore
    this.doWritev = null;
    this.doInitialize((error: Error): void => {
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
    return this.writableObjectModeInner;
  }

  /**
   * Value of highWaterMark.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableHighWatermark(): number | undefined {
    return this.writableHighWatermarkInner;
  }

  /**
   * Is true if it is safe to call writable.write(), which means the stream has not been destroyed, errored, or ended.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writable(): boolean | undefined {
    return this.writableInner;
  }

  /**
   * Size of data this can be flushed, in bytes or objects.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableLength(): number | undefined {
    return this.writableLengthInner;
  }

  /**
   * If the buffer of the stream is full and true, otherwise it is false.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableNeedDrain(): boolean | undefined {
    return this.writableNeedDrainInner;
  }

  /**
   * Number of times writable.uncork() needs to be called in order to fully uncork the stream.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableCorked(): number | undefined {
    return this.writableCorkedInner;
  };

  /**
   * Whether Writable.end has been called.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableEnded(): boolean | undefined {
    return this.writableEndedInner;
  }

  /**
   * Whether Writable.end has been called and all buffers have been flushed.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get writableFinished(): boolean | undefined {
    return this.writableFinishedInner;
  }

  /**
   * Returns error if the stream has been destroyed with an error.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get errored(): Error | undefined | null {
    return this.erroredInner;
  }

  /**
   * Writable completes destroyfile and returns true, otherwise returns false.
   *
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  get closed(): boolean | undefined {
    return this.closedInner;
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
   * @throws { BusinessError } 10200035 - if doWrite not implemented, an exception will be thrown.
   * @throws { BusinessError } 10200036 - if stream has been ended, writing data to it will throw an error.
   * @throws { BusinessError } 10200037 - if the callback is called multiple times consecutively, an error will be thrown.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  write(chunk?: string | Uint8Array, encoding?: string, callback?: Function): boolean {
    if (encoding) {
      this.setDefaultEncoding(encoding);
    }
    if (chunk === null) {
      throw new BusinessError('The "chunk" argument must be of type string or UintArray', 401);
    }
    if (typeof chunk !== 'string' && !(chunk instanceof Uint8Array)) {
      throw new BusinessError('The "chunk" argument must be of type string or UintArray', 401);
    }
    if (this.ending && !this.writing) {
      setTimeout((): void => {
        this.erroredInner = new BusinessError('write after end', 10200036);
        callback?.(this.erroredInner);
        this.throwError(this.erroredInner);
      });
      return false;
    }
    if (this.erroredInner) {
      return false;
    }
    let flag = false;
    if (chunk instanceof Uint8Array) {
      flag = this.writeUint8Array(chunk, encoding ?? this.encoding, callback);
    } else {
      flag = this.writeString(chunk!, encoding ?? this.encoding, callback);
    }
    if (!flag) {
      this.writableNeedDrainInner = true;
    }
    return flag;
  }

  private getChunkLength(chunk: string | Uint8Array): number {
    if (chunk instanceof Uint8Array) {
      return chunk.byteLength;
    } else {
      return this.encoder.encodeInto(chunk).byteLength;
    }
  }

  private writeUint8Array(chunk: Uint8Array, encoding?: string, callback?: Function): boolean {
    this.writableLengthInner! += this.getChunkLength(chunk);
    const hasRemaining = this.writableLengthInner! < this.writableHighWatermark!;
    const fnBack = runOnce((error?: Error): void => {
      if (error && error instanceof Error) {
        this.writableInner = false;
        this.throwError(error);
        return;
      }
      callback?.(error ?? null);
      this.freshCache();
    }, (multipleTimes: boolean, err: Error): void => {
      this.listener?.emit(WritableEvent.ERROR, ERR_MULTIPLE_CALLBACK);
    });
    if (this.writableCorkedInner === 0) {
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
    this.writableLengthInner! += this.getChunkLength(chunk);
    const hasRemaining = this.writableLengthInner! < this.writableHighWatermark!;
    const fb = runOnce((error?: Error): void => {
      if (error) {
        this.erroredInner = error;
      }
      callback?.(error ?? null);
      this.freshCache();
      if (error && error instanceof Error) {
        this.writableInner = false;
        this.erroredInner = error;
        Promise.resolve().then((): void => {
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

    if (this.writableCorkedInner === 0) {
      if (!this.writing) {
        this.writing = true;
        this.doWrite?.(chunk, encoding ?? 'utf8', fb);
        if (!this.doWrite && !hasRemaining) {
          Promise.resolve().then(() => {
            this.writableLengthInner = 0;
            this.listener?.emit(WritableEvent.DRAIN);
          });
        }
      } else {
        this.buffer.push({ chunk: chunk, encoding: encoding, callback: fb });
      }
    } else {
      this.buffer.push({ chunk: chunk, encoding: encoding, callback: fb });
    }
    return this.erroredInner ? false : hasRemaining;
  }

  private freshCache(): void {
    const current = this.buffer.shift();
    if (current) {
      this.doWrite?.(current.chunk, current.encoding ?? 'utf8', current.callback);
      this.writableLengthInner! -= this.getChunkLength(current.chunk);
    } else {
      this.writing = false;
      this.writableLengthInner = 0;
      if (!this.finishMayBe()) {
        this.writableNeedDrainInner = false;
        this.listener?.emit(WritableEvent.DRAIN);
      }
    }
  }

  private freshCacheV(): void {
    if (this.buffer.length > 0) {
      if (this.doWritev) {
        const funCallback = runOnce((error?: Error): void => {
          if (error && error instanceof Error) {
            this.erroredInner = error;
            this.listener?.emit(WritableEvent.ERROR, error);
            return;
          }
          this.buffer = [];
        }, () => {
          this.listener?.emit(WritableEvent.ERROR, ERR_MULTIPLE_CALLBACK);
        });
        // @ts-ignore
        this.doWritev(this.buffer.map((item: { encoding?: string; chunk: string | Uint8Array; callback: Function }) => {
          return item.chunk;
        }), funCallback);
        if (!this.finishMayBe()) {
          this.writableNeedDrainInner = true;
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
   * @throws { BusinessError } 10200035 - if doWrite not implemented, an exception will be thrown.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  end(chunk?: string | Uint8Array, encoding?: string, callback?: Function): Writable {
    if (this.writableFinishedInner) {
      this.erroredInner = ERR_STREAM_ALREADY_FINISHED;
      setTimeout(() => callback?.(this.erroredInner));
      this.emitErrorOnce(this.erroredInner);
      return this;
    } else if (this.writableEndedInner) {
      this.erroredInner = ERR_WRITE_AFTER_END;
      setTimeout(() => callback?.(this.erroredInner));
      this.emitErrorOnce(this.erroredInner);
      return this;
    }
    if (this.erroredInner) {
      setTimeout(() => callback?.(this.erroredInner));
      return this;
    }
    this.writableNeedDrainInner = false;
    this.closedInner = true;
    this.ending = true;
    this.writableInner = false;
    if (!this.writableEndedInner) {
      if (this.writableCorkedInner === 0) {
        if (chunk) {
          if (this.writing) {
            this.write(chunk, encoding, callback);
          } else {
            this.doWrite?.(chunk!, encoding ?? 'utf8', (error?: Error): void => {
              if (error && error instanceof Error) {
                this.erroredInner = error;
                this.listener?.emit(WritableEvent.ERROR, error);
              } else {
                this.writableLengthInner! -= this.getChunkLength(chunk);
                this.writing = false;
                this.finishMayBe();
              }
              this.writableEndedInner = true;
              this.writableInner = false;
              asyncFn((): void => {
                callback?.(this.erroredInner ?? error ?? null);
              })();
              if (!this.writableFinishedInner) {
                this.writableFinishedInner = true;
                asyncFn((): void => {
                  if ((!this.erroredInner || this.erroredInner.message === 'write after end') && !this.isOnError()) {
                    this.listener?.emit(WritableEvent.FINISH);
                  }
                })();
              }
            });
          }
        } else {
          if (this.writableEndedInner) {
            this.erroredInner = new BusinessError('write after end', 10200036);
            callback?.(this.erroredInner);
          } else {
            setTimeout(() => callback?.(this.erroredInner));
          }
          if (!this.writableFinishedInner && !this.writableEndedInner) {
            this.writableFinishedInner = true;
            asyncFn((): void => {
              if (!this.erroredInner || this.erroredInner.message === 'write after end') {
                this.listener?.emit(WritableEvent.FINISH);
              }
            })();
          }
        }
      } else {
        this.writableCorkedInner = 1;
        this.uncork();
      }
    }
    this.writableEndedInner = true;
    this.listener?.emit(WritableEvent.CLOSE);
    return this;
  }

  private finishMayBe(): boolean {
    return !this.writing && this.writableCorkedInner === 0 && this.ending;
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
    if (!encoding) {
      return false;
    }
    if (encoding.toLowerCase() === 'utf8') {
      encoding = 'utf-8';
    }
    if (ENCODING_SET.indexOf(encoding.toLowerCase()) !== -1) {
      this.encoding = encoding.toLowerCase();
      try {
        if (encoding.toLowerCase() !== 'ascii') {
          this.encoder = new TextEncoder(encoding);
        }
      } catch (e) {
        this.throwError(e as Error);
        return false;
      }
      return true;
    } else {
      const err: BusinessError = new BusinessError(`Unknown encoding: ${encoding}`);
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
    this.writableCorkedInner += 1;
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
    if (this.writableCorkedInner > 0) {
      this.writableCorkedInner -= 1;
    }
    if (this.writableCorkedInner === 0) {
      this.freshCacheV();
    }
    return true;
  }

  /**
   * Registering Event Messages.
   *
   * @param { string } event - Register Event.
   * @param { Callback<emitter.EventData> } callback - event callbacks.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  on(event: string, callback: Function): void {
    this.callbacks[event] = this.callbacks[event] ?? [];
    const callbackFn = callback.bind(this);
    this.callbacks[event].push(callbackFn);
    this.listener?.on(event, callbackFn);
  }

  /**
   * Cancel event message.
   *
   * @param { string } event - Register Event.
   * @param { Callback<emitter.EventData> } callback - event callbacks.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  off(event: string, callback?: Function): void {
    if (!event) {
      this.throwError(new BusinessError('event is null', 401));
      return;
    }
    if (callback) {
      this.callbacks[event]?.forEach((it: Function): void => {
        if (callback && callback === it) {
          this.listener?.off(event, it);
        }
      });
    } else {
      this.callbacks[event]?.forEach((it: Function) => this.listener?.off(event, it));
    }
  }

  noWriteOpes(chunk: string | Uint8Array, encoding: string, callback: Function): void {
    if (this.doWritev === null) {
      this.throwError(ERR_DOWRITE_NOT_IMPLEMENTED);
    } else {
      // @ts-ignore
      this.doWritev([chunk], callback);
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
    this.erroredInner = error;
    if (this.listener && this.listener.listenerCount(WritableEvent.ERROR) > 0) {
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

  private emitErrorExecutedInner = false;
  // @ts-ignore
  private emitErrorIdInner: number;

  private emitErrorOnce(error: Error, reset?: boolean): void {
    if (reset) {
      this.emitErrorExecutedInner = false;
      clearTimeout(this.emitErrorIdInner);
    }
    if (!this.emitErrorExecutedInner) {
      this.emitErrorExecutedInner = true;
      // @ts-ignore
      this.emitErrorIdInner = setTimeout((): void => {
        this.listener?.emit(WritableEvent.ERROR, this.erroredInner ?? error);
      });
    }
  }
}

Writable.prototype.doWrite = Writable.prototype.noWriteOpes;

class Duplex extends Readable {
  private _writable: Writable;

  constructor() {
    super();
    this._writable = new Writable();
    const that = this;
    if (this.doWrite) {
      this._writable.doWrite = this.doWrite?.bind(that);
    }
    this._writable.doWritev = this.doWritev?.bind(that);
    Object.defineProperties(that, {
      doWrite: {
        get(): Function {
          return that._writable.doWrite?.bind(that);
        },
        set(value: Function):void {
          that._writable.doWrite = value.bind(that);
        }
      },
      doWritev: {
        get(): Function {
          return that._writable.doWritev?.bind(that);
        },
        set(value: Function):void {
          // @ts-ignore
          that._writable.doWritev = value?.bind(that);
        }
      }
    });
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
   * @throws { BusinessError } 10200036 - if stream has been ended, writing data to it will throw an error.
   * @throws { BusinessError } 10200037 - if the callback is called multiple times consecutively, an error will be thrown.
   * @throws { BusinessError } 10200039 - if a class inherits from Transform, it must implement doTransform; otherwise, an error will be raised.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  write(chunk?: string | Uint8Array, encoding?: string, callback?: Function): boolean {
    return this._writable.write(chunk, encoding, callback);
  }

  /**
   * Write the last chunk to Writable.
   *
   * @param { string | Uint8Array } [chunk] - Data to be written.
   * @param { string } [encoding] - Encoding type.
   * @param { Function } [callback] - Callback after writing.
   * @returns { Writable } Returns the Writable object.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @throws { BusinessError } 10200039 - if a class inherits from Transform, it must implement doTransform; otherwise, an error will be raised.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  end(chunk?: string | Uint8Array, encoding?: string, callback?: Function): Writable {
    super.setEndType();
    return this._writable.end(chunk, encoding, callback);
  }

  on(event: string, callback: Function): void {
    super.on(event, callback);
    this._writable.on(event, callback);
  }

  off(event: string, callback?: Function): void {
    super.off(event);
    this._writable.off(event, callback);
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

  get writableHighWatermark(): number {
    return this._writable.writableHighWatermark || 0;
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

  /**
   * Write the last chunk to Writable.
   *
   * @param { string | Uint8Array } [chunk] - Data to be written.
   * @param { string } [encoding] - Encoding type.
   * @param { Function } [callback] - Callback after writing.
   * @returns { Writable } Returns the Writable object.
   * @throws { BusinessError } 401 - if the input parameters are invalid.
   * @throws { BusinessError } 10200039 - if a class inherits from Transform, it must implement doTransform; otherwise, an error will be raised.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  end(chunk?: string | Uint8Array, encoding?: string, callback?: Function): Writable {
    if (!this.doTransform) {
      throw new BusinessError('The doTransform() method is not implemented', 10200039);
    }
    if (chunk instanceof Uint8Array) {
      const chunkString = this.decoder.decodeWithStream(chunk);
      this.doTransform(chunkString, encoding || 'utf8', callback || ((): void => {
      }));
    } else if (typeof chunk === 'string') {
      this.doTransform(chunk, encoding || 'utf8', callback || ((): void => {
      }));
    }
    this.doFlush?.((...args: (string | Uint8Array)[]) => {
      args.forEach((it: string | Uint8Array) => {
        if (it) {
          this.push(it ?? '', encoding);
        }
      });
    });
    const write:Writable = super.end(chunk, encoding, callback);
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
    throw new BusinessError('The doTransform() method is not implemented');
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
   * @throws { BusinessError } 10200036 - if stream has been ended, writing data to it will throw an error.
   * @throws { BusinessError } 10200037 - if the callback is called multiple times consecutively, an error will be thrown.
   * @throws { BusinessError } 10200039 - if a class inherits from Transform, it must implement doTransform; otherwise, an error will be raised.
   * @syscap SystemCapability.Utils.Lang
   * @crossplatform
   * @since 12
   */
  write(chunk?: string | Uint8Array, encoding?: string, callback?: Function): boolean {
    if (typeof chunk === 'string') {
      const callBackFunction = runOnce((error: Error) => {
        if (error) {
          this.getListener()?.emit(WritableEvent.ERROR, error);
        }
      }, () => {
        const err:BusinessError = new BusinessError('Callback called multiple times', 10200037);
        this.getListener()?.emit(WritableEvent.ERROR, err);
      });
      this.doTransform?.(chunk ?? '', encoding ?? 'utf8', callBackFunction);
    }
    return super.write(chunk, encoding, callback);
  }

  doRead(size: number): void {
  }

  doWrite(chunk: string | Uint8Array, encoding: string, callback: Function):void {
    super.doWrite?.(chunk, encoding, callback);
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
