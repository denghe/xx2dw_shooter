var Module = typeof Module != "undefined" ? Module : {};
var moduleOverrides = Object.assign({}, Module);
var arguments_ = [];
var thisProgram = "./this.program";
var quit_ = (status, toThrow) => {
  throw toThrow;
};
var ENVIRONMENT_IS_WEB = true;
var ENVIRONMENT_IS_WORKER = false;
var scriptDirectory = "";
function locateFile(path) {
  if (Module["locateFile"]) {
    return Module["locateFile"](path, scriptDirectory);
  }
  return scriptDirectory + path;
}
var read_, readAsync, readBinary;
if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
  if (ENVIRONMENT_IS_WORKER) {
    scriptDirectory = self.location.href;
  } else if (typeof document != "undefined" && document.currentScript) {
    scriptDirectory = document.currentScript.src;
  }
  if (scriptDirectory.startsWith("blob:")) {
    scriptDirectory = "";
  } else {
    scriptDirectory = scriptDirectory.substr(0, scriptDirectory.replace(/[?#].*/, "").lastIndexOf("/") + 1);
  }
  {
    read_ = url => {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", url, false);
      xhr.send(null);
      return xhr.responseText;
    };
    if (ENVIRONMENT_IS_WORKER) {
      readBinary = url => {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", url, false);
        xhr.responseType = "arraybuffer";
        xhr.send(null);
        return new Uint8Array(xhr.response);
      };
    }
    readAsync = (url, onload, onerror) => {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", url, true);
      xhr.responseType = "arraybuffer";
      xhr.onload = () => {
        if (xhr.status == 200 || xhr.status == 0 && xhr.response) {
          onload(xhr.response);
          return;
        }
        onerror();
      };
      xhr.onerror = onerror;
      xhr.send(null);
    };
  }
} else {}
var out = Module["print"] || console.log.bind(console);
var err = Module["printErr"] || console.error.bind(console);
Object.assign(Module, moduleOverrides);
moduleOverrides = null;
if (Module["arguments"]) arguments_ = Module["arguments"];
if (Module["thisProgram"]) thisProgram = Module["thisProgram"];
if (Module["quit"]) quit_ = Module["quit"];
var wasmBinary;
if (Module["wasmBinary"]) wasmBinary = Module["wasmBinary"];
if (typeof WebAssembly != "object") {
  abort("no native wasm support detected");
}
var wasmMemory;
var ABORT = false;
var EXITSTATUS;
var HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;
function updateMemoryViews() {
  var b = wasmMemory.buffer;
  Module["HEAP8"] = HEAP8 = new Int8Array(b);
  Module["HEAP16"] = HEAP16 = new Int16Array(b);
  Module["HEAPU8"] = HEAPU8 = new Uint8Array(b);
  Module["HEAPU16"] = HEAPU16 = new Uint16Array(b);
  Module["HEAP32"] = HEAP32 = new Int32Array(b);
  Module["HEAPU32"] = HEAPU32 = new Uint32Array(b);
  Module["HEAPF32"] = HEAPF32 = new Float32Array(b);
  Module["HEAPF64"] = HEAPF64 = new Float64Array(b);
}
var __ATPRERUN__ = [];
var __ATINIT__ = [];
var __ATMAIN__ = [];
var __ATPOSTRUN__ = [];
var runtimeInitialized = false;
function preRun() {
  if (Module["preRun"]) {
    if (typeof Module["preRun"] == "function") Module["preRun"] = [Module["preRun"]];
    while (Module["preRun"].length) {
      addOnPreRun(Module["preRun"].shift());
    }
  }
  callRuntimeCallbacks(__ATPRERUN__);
}
function initRuntime() {
  runtimeInitialized = true;
  callRuntimeCallbacks(__ATINIT__);
}
function preMain() {
  callRuntimeCallbacks(__ATMAIN__);
}
function postRun() {
  if (Module["postRun"]) {
    if (typeof Module["postRun"] == "function") Module["postRun"] = [Module["postRun"]];
    while (Module["postRun"].length) {
      addOnPostRun(Module["postRun"].shift());
    }
  }
  callRuntimeCallbacks(__ATPOSTRUN__);
}
function addOnPreRun(cb) {
  __ATPRERUN__.unshift(cb);
}
function addOnInit(cb) {
  __ATINIT__.unshift(cb);
}
function addOnPostRun(cb) {
  __ATPOSTRUN__.unshift(cb);
}
var runDependencies = 0;
var runDependencyWatcher = null;
var dependenciesFulfilled = null;
function addRunDependency(id) {
  var _Module$monitorRunDep;
  runDependencies++;
  (_Module$monitorRunDep = Module["monitorRunDependencies"]) === null || _Module$monitorRunDep === void 0 || _Module$monitorRunDep.call(Module, runDependencies);
}
function removeRunDependency(id) {
  var _Module$monitorRunDep2;
  runDependencies--;
  (_Module$monitorRunDep2 = Module["monitorRunDependencies"]) === null || _Module$monitorRunDep2 === void 0 || _Module$monitorRunDep2.call(Module, runDependencies);
  if (runDependencies == 0) {
    if (runDependencyWatcher !== null) {
      clearInterval(runDependencyWatcher);
      runDependencyWatcher = null;
    }
    if (dependenciesFulfilled) {
      var callback = dependenciesFulfilled;
      dependenciesFulfilled = null;
      callback();
    }
  }
}
function abort(what) {
  var _Module$onAbort;
  (_Module$onAbort = Module["onAbort"]) === null || _Module$onAbort === void 0 || _Module$onAbort.call(Module, what);
  what = "Aborted(" + what + ")";
  err(what);
  ABORT = true;
  EXITSTATUS = 1;
  what += ". Build with -sASSERTIONS for more info.";
  var e = new WebAssembly.RuntimeError(what);
  throw e;
}
var dataURIPrefix = "data:application/octet-stream;base64,";
var isDataURI = filename => filename.startsWith(dataURIPrefix);
var wasmBinaryFile;
wasmBinaryFile = "s18.wasm";
if (!isDataURI(wasmBinaryFile)) {
  wasmBinaryFile = locateFile(wasmBinaryFile);
}
function getBinarySync(file) {
  if (file == wasmBinaryFile && wasmBinary) {
    return new Uint8Array(wasmBinary);
  }
  if (readBinary) {
    return readBinary(file);
  }
  throw "both async and sync fetching of the wasm failed";
}
function getBinaryPromise(binaryFile) {
  if (!wasmBinary && (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER)) {
    if (typeof fetch == "function") {
      return fetch(binaryFile, {
        credentials: "same-origin"
      }).then(response => {
        if (!response["ok"]) {
          throw `failed to load wasm binary file at '${binaryFile}'`;
        }
        return response["arrayBuffer"]();
      }).catch(() => getBinarySync(binaryFile));
    }
  }
  return Promise.resolve().then(() => getBinarySync(binaryFile));
}
function instantiateArrayBuffer(binaryFile, imports, receiver) {
  return getBinaryPromise(binaryFile).then(binary => WebAssembly.instantiate(binary, imports)).then(receiver, reason => {
    err(`failed to asynchronously prepare wasm: ${reason}`);
    abort(reason);
  });
}
function instantiateAsync(binary, binaryFile, imports, callback) {
  if (!binary && typeof WebAssembly.instantiateStreaming == "function" && !isDataURI(binaryFile) && typeof fetch == "function") {
    return fetch(binaryFile, {
      credentials: "same-origin"
    }).then(response => {
      var result = WebAssembly.instantiateStreaming(response, imports);
      return result.then(callback, function (reason) {
        err(`wasm streaming compile failed: ${reason}`);
        err("falling back to ArrayBuffer instantiation");
        return instantiateArrayBuffer(binaryFile, imports, callback);
      });
    });
  }
  return instantiateArrayBuffer(binaryFile, imports, callback);
}
function createWasm() {
  var info = {
    "a": wasmImports
  };
  function receiveInstance(instance, module) {
    wasmExports = instance.exports;
    wasmMemory = wasmExports["wa"];
    updateMemoryViews();
    wasmTable = wasmExports["Aa"];
    addOnInit(wasmExports["xa"]);
    removeRunDependency("wasm-instantiate");
    return wasmExports;
  }
  addRunDependency("wasm-instantiate");
  function receiveInstantiationResult(result) {
    receiveInstance(result["instance"]);
  }
  if (Module["instantiateWasm"]) {
    try {
      return Module["instantiateWasm"](info, receiveInstance);
    } catch (e) {
      err(`Module.instantiateWasm callback failed with error: ${e}`);
      return false;
    }
  }
  instantiateAsync(wasmBinary, wasmBinaryFile, info, receiveInstantiationResult);
  return {};
}
var ASM_CONSTS = {
  17576: ($0, $1, $2, $3, $4) => {
    if (typeof window === "undefined" || (window.AudioContext || window.webkitAudioContext) === undefined) {
      return 0;
    }
    if (typeof window.miniaudio === "undefined") {
      window.miniaudio = {
        referenceCount: 0
      };
      window.miniaudio.device_type = {};
      window.miniaudio.device_type.playback = $0;
      window.miniaudio.device_type.capture = $1;
      window.miniaudio.device_type.duplex = $2;
      window.miniaudio.device_state = {};
      window.miniaudio.device_state.stopped = $3;
      window.miniaudio.device_state.started = $4;
      miniaudio.devices = [];
      miniaudio.track_device = function (device) {
        for (var iDevice = 0; iDevice < miniaudio.devices.length; ++iDevice) {
          if (miniaudio.devices[iDevice] == null) {
            miniaudio.devices[iDevice] = device;
            return iDevice;
          }
        }
        miniaudio.devices.push(device);
        return miniaudio.devices.length - 1;
      };
      miniaudio.untrack_device_by_index = function (deviceIndex) {
        miniaudio.devices[deviceIndex] = null;
        while (miniaudio.devices.length > 0) {
          if (miniaudio.devices[miniaudio.devices.length - 1] == null) {
            miniaudio.devices.pop();
          } else {
            break;
          }
        }
      };
      miniaudio.untrack_device = function (device) {
        for (var iDevice = 0; iDevice < miniaudio.devices.length; ++iDevice) {
          if (miniaudio.devices[iDevice] == device) {
            return miniaudio.untrack_device_by_index(iDevice);
          }
        }
      };
      miniaudio.get_device_by_index = function (deviceIndex) {
        return miniaudio.devices[deviceIndex];
      };
      miniaudio.unlock_event_types = function () {
        return ["touchend", "click"];
      }();
      miniaudio.unlock = function () {
        for (var i = 0; i < miniaudio.devices.length; ++i) {
          var device = miniaudio.devices[i];
          if (device != null && device.webaudio != null && device.state === window.miniaudio.device_state.started) {
            device.webaudio.resume().then(() => {
              Module._ma_device__on_notification_unlocked(device.pDevice);
            }, error => {
              console.error("Failed to resume audiocontext", error);
            });
          }
        }
        miniaudio.unlock_event_types.map(function (event_type) {
          document.removeEventListener(event_type, miniaudio.unlock, true);
        });
      };
      miniaudio.unlock_event_types.map(function (event_type) {
        document.addEventListener(event_type, miniaudio.unlock, true);
      });
    }
    window.miniaudio.referenceCount += 1;
    return 1;
  },
  19734: () => {
    if (typeof window.miniaudio !== "undefined") {
      window.miniaudio.referenceCount -= 1;
      if (window.miniaudio.referenceCount === 0) {
        delete window.miniaudio;
      }
    }
  },
  19898: () => navigator.mediaDevices !== undefined && navigator.mediaDevices.getUserMedia !== undefined,
  20002: () => {
    try {
      var temp = new (window.AudioContext || window.webkitAudioContext)();
      var sampleRate = temp.sampleRate;
      temp.close();
      return sampleRate;
    } catch (e) {
      return 0;
    }
  },
  20173: ($0, $1, $2, $3, $4, $5) => {
    var deviceType = $0;
    var channels = $1;
    var sampleRate = $2;
    var bufferSize = $3;
    var pIntermediaryBuffer = $4;
    var pDevice = $5;
    if (typeof window.miniaudio === "undefined") {
      return -1;
    }
    var device = {};
    var audioContextOptions = {};
    if (deviceType == window.miniaudio.device_type.playback && sampleRate != 0) {
      audioContextOptions.sampleRate = sampleRate;
    }
    device.webaudio = new (window.AudioContext || window.webkitAudioContext)(audioContextOptions);
    device.webaudio.suspend();
    device.state = window.miniaudio.device_state.stopped;
    var channelCountIn = 0;
    var channelCountOut = channels;
    if (deviceType != window.miniaudio.device_type.playback) {
      channelCountIn = channels;
    }
    device.scriptNode = device.webaudio.createScriptProcessor(bufferSize, channelCountIn, channelCountOut);
    device.scriptNode.onaudioprocess = function (e) {
      if (device.intermediaryBufferView == null || device.intermediaryBufferView.length == 0) {
        device.intermediaryBufferView = new Float32Array(Module.HEAPF32.buffer, pIntermediaryBuffer, bufferSize * channels);
      }
      if (deviceType == miniaudio.device_type.capture || deviceType == miniaudio.device_type.duplex) {
        for (var iChannel = 0; iChannel < channels; iChannel += 1) {
          var inputBuffer = e.inputBuffer.getChannelData(iChannel);
          var intermediaryBuffer = device.intermediaryBufferView;
          for (var iFrame = 0; iFrame < bufferSize; iFrame += 1) {
            intermediaryBuffer[iFrame * channels + iChannel] = inputBuffer[iFrame];
          }
        }
        _ma_device_process_pcm_frames_capture__webaudio(pDevice, bufferSize, pIntermediaryBuffer);
      }
      if (deviceType == miniaudio.device_type.playback || deviceType == miniaudio.device_type.duplex) {
        _ma_device_process_pcm_frames_playback__webaudio(pDevice, bufferSize, pIntermediaryBuffer);
        for (var iChannel = 0; iChannel < e.outputBuffer.numberOfChannels; ++iChannel) {
          var outputBuffer = e.outputBuffer.getChannelData(iChannel);
          var intermediaryBuffer = device.intermediaryBufferView;
          for (var iFrame = 0; iFrame < bufferSize; iFrame += 1) {
            outputBuffer[iFrame] = intermediaryBuffer[iFrame * channels + iChannel];
          }
        }
      } else {
        for (var iChannel = 0; iChannel < e.outputBuffer.numberOfChannels; ++iChannel) {
          e.outputBuffer.getChannelData(iChannel).fill(0);
        }
      }
    };
    if (deviceType == miniaudio.device_type.capture || deviceType == miniaudio.device_type.duplex) {
      navigator.mediaDevices.getUserMedia({
        audio: true,
        video: false
      }).then(function (stream) {
        device.streamNode = device.webaudio.createMediaStreamSource(stream);
        device.streamNode.connect(device.scriptNode);
        device.scriptNode.connect(device.webaudio.destination);
      }).catch(function (error) {
        console.log("Failed to get user media: " + error);
      });
    }
    if (deviceType == miniaudio.device_type.playback) {
      device.scriptNode.connect(device.webaudio.destination);
    }
    device.pDevice = pDevice;
    return miniaudio.track_device(device);
  },
  23001: $0 => miniaudio.get_device_by_index($0).webaudio.sampleRate,
  23067: $0 => {
    var device = miniaudio.get_device_by_index($0);
    if (device.scriptNode !== undefined) {
      device.scriptNode.onaudioprocess = function (e) {};
      device.scriptNode.disconnect();
      device.scriptNode = undefined;
    }
    if (device.streamNode !== undefined) {
      device.streamNode.disconnect();
      device.streamNode = undefined;
    }
    device.webaudio.close();
    device.webaudio = undefined;
    device.pDevice = undefined;
  },
  23460: $0 => {
    miniaudio.untrack_device_by_index($0);
  },
  23503: $0 => {
    var device = miniaudio.get_device_by_index($0);
    device.webaudio.resume();
    device.state = miniaudio.device_state.started;
  },
  23628: $0 => {
    var device = miniaudio.get_device_by_index($0);
    device.webaudio.suspend();
    device.state = miniaudio.device_state.stopped;
  }
};
function ExitStatus(status) {
  this.name = "ExitStatus";
  this.message = `Program terminated with exit(${status})`;
  this.status = status;
}
var callRuntimeCallbacks = callbacks => {
  while (callbacks.length > 0) {
    callbacks.shift()(Module);
  }
};
var noExitRuntime = Module["noExitRuntime"] || true;
var UTF8Decoder = new TextDecoder("utf8");
var UTF8ToString = (ptr, maxBytesToRead) => {
  if (!ptr) return "";
  var maxPtr = ptr + maxBytesToRead;
  for (var end = ptr; !(end >= maxPtr) && HEAPU8[end];) ++end;
  return UTF8Decoder.decode(HEAPU8.subarray(ptr, end));
};
var ___assert_fail = (condition, filename, line, func) => {
  abort(`Assertion failed: ${UTF8ToString(condition)}, at: ` + [filename ? UTF8ToString(filename) : "unknown filename", line, func ? UTF8ToString(func) : "unknown function"]);
};
class ExceptionInfo {
  constructor(excPtr) {
    this.excPtr = excPtr;
    this.ptr = excPtr - 24;
  }
  set_type(type) {
    HEAPU32[this.ptr + 4 >> 2] = type;
  }
  get_type() {
    return HEAPU32[this.ptr + 4 >> 2];
  }
  set_destructor(destructor) {
    HEAPU32[this.ptr + 8 >> 2] = destructor;
  }
  get_destructor() {
    return HEAPU32[this.ptr + 8 >> 2];
  }
  set_caught(caught) {
    caught = caught ? 1 : 0;
    HEAP8[this.ptr + 12] = caught;
  }
  get_caught() {
    return HEAP8[this.ptr + 12] != 0;
  }
  set_rethrown(rethrown) {
    rethrown = rethrown ? 1 : 0;
    HEAP8[this.ptr + 13] = rethrown;
  }
  get_rethrown() {
    return HEAP8[this.ptr + 13] != 0;
  }
  init(type, destructor) {
    this.set_adjusted_ptr(0);
    this.set_type(type);
    this.set_destructor(destructor);
  }
  set_adjusted_ptr(adjustedPtr) {
    HEAPU32[this.ptr + 16 >> 2] = adjustedPtr;
  }
  get_adjusted_ptr() {
    return HEAPU32[this.ptr + 16 >> 2];
  }
  get_exception_ptr() {
    var isPointer = ___cxa_is_pointer_type(this.get_type());
    if (isPointer) {
      return HEAPU32[this.excPtr >> 2];
    }
    var adjusted = this.get_adjusted_ptr();
    if (adjusted !== 0) return adjusted;
    return this.excPtr;
  }
}
var exceptionLast = 0;
var uncaughtExceptionCount = 0;
var ___cxa_throw = (ptr, type, destructor) => {
  var info = new ExceptionInfo(ptr);
  info.init(type, destructor);
  exceptionLast = ptr;
  uncaughtExceptionCount++;
  throw exceptionLast;
};
var SYSCALLS = {
  varargs: undefined,
  get() {
    var ret = HEAP32[+SYSCALLS.varargs >> 2];
    SYSCALLS.varargs += 4;
    return ret;
  },
  getp() {
    return SYSCALLS.get();
  },
  getStr(ptr) {
    var ret = UTF8ToString(ptr);
    return ret;
  }
};
function ___syscall_fcntl64(fd, cmd, varargs) {
  SYSCALLS.varargs = varargs;
  return 0;
}
function ___syscall_ioctl(fd, op, varargs) {
  SYSCALLS.varargs = varargs;
  return 0;
}
function ___syscall_openat(dirfd, path, flags, varargs) {
  SYSCALLS.varargs = varargs;
}
function __emscripten_fetch_free(id) {
  if (Fetch.xhrs.has(id)) {
    var xhr = Fetch.xhrs.get(id);
    Fetch.xhrs.free(id);
    if (xhr.readyState > 0 && xhr.readyState < 4) {
      xhr.abort();
    }
  }
}
var nowIsMonotonic = 1;
var __emscripten_get_now_is_monotonic = () => nowIsMonotonic;
var _abort = () => {
  abort("");
};
var readEmAsmArgsArray = [];
var readEmAsmArgs = (sigPtr, buf) => {
  readEmAsmArgsArray.length = 0;
  var ch;
  while (ch = HEAPU8[sigPtr++]) {
    var wide = ch != 105;
    wide &= ch != 112;
    buf += wide && buf % 8 ? 4 : 0;
    readEmAsmArgsArray.push(ch == 112 ? HEAPU32[buf >> 2] : ch == 105 ? HEAP32[buf >> 2] : HEAPF64[buf >> 3]);
    buf += wide ? 8 : 4;
  }
  return readEmAsmArgsArray;
};
var runEmAsmFunction = (code, sigPtr, argbuf) => {
  var args = readEmAsmArgs(sigPtr, argbuf);
  return ASM_CONSTS[code](...args);
};
var _emscripten_asm_const_int = (code, sigPtr, argbuf) => runEmAsmFunction(code, sigPtr, argbuf);
var _emscripten_date_now = () => Date.now();
var _emscripten_get_now;
_emscripten_get_now = () => performance.now();
var _emscripten_is_main_browser_thread = () => !ENVIRONMENT_IS_WORKER;
var wasmTable;
var getWasmTableEntry = funcPtr => wasmTable.get(funcPtr);
var _emscripten_request_animation_frame_loop = (cb, userData) => {
  function tick(timeStamp) {
    if (getWasmTableEntry(cb)(timeStamp, userData)) {
      requestAnimationFrame(tick);
    }
  }
  return requestAnimationFrame(tick);
};
var getHeapMax = () => 2147483648;
var growMemory = size => {
  var b = wasmMemory.buffer;
  var pages = (size - b.byteLength + 65535) / 65536;
  try {
    wasmMemory.grow(pages);
    updateMemoryViews();
    return 1;
  } catch (e) {}
};
var _emscripten_resize_heap = requestedSize => {
  var oldSize = HEAPU8.length;
  requestedSize >>>= 0;
  var maxHeapSize = getHeapMax();
  if (requestedSize > maxHeapSize) {
    return false;
  }
  var alignUp = (x, multiple) => x + (multiple - x % multiple) % multiple;
  for (var cutDown = 1; cutDown <= 4; cutDown *= 2) {
    var overGrownHeapSize = oldSize * (1 + .2 / cutDown);
    overGrownHeapSize = Math.min(overGrownHeapSize, requestedSize + 100663296);
    var newSize = Math.min(maxHeapSize, alignUp(Math.max(requestedSize, overGrownHeapSize), 65536));
    var replacement = growMemory(newSize);
    if (replacement) {
      return true;
    }
  }
  return false;
};
var JSEvents = {
  removeAllEventListeners() {
    while (JSEvents.eventHandlers.length) {
      JSEvents._removeHandler(JSEvents.eventHandlers.length - 1);
    }
    JSEvents.deferredCalls = [];
  },
  inEventHandler: 0,
  deferredCalls: [],
  deferCall(targetFunction, precedence, argsList) {
    function arraysHaveEqualContent(arrA, arrB) {
      if (arrA.length != arrB.length) return false;
      for (var i in arrA) {
        if (arrA[i] != arrB[i]) return false;
      }
      return true;
    }
    for (var i in JSEvents.deferredCalls) {
      var call = JSEvents.deferredCalls[i];
      if (call.targetFunction == targetFunction && arraysHaveEqualContent(call.argsList, argsList)) {
        return;
      }
    }
    JSEvents.deferredCalls.push({
      targetFunction: targetFunction,
      precedence: precedence,
      argsList: argsList
    });
    JSEvents.deferredCalls.sort((x, y) => x.precedence < y.precedence);
  },
  removeDeferredCalls(targetFunction) {
    for (var i = 0; i < JSEvents.deferredCalls.length; ++i) {
      if (JSEvents.deferredCalls[i].targetFunction == targetFunction) {
        JSEvents.deferredCalls.splice(i, 1);
        --i;
      }
    }
  },
  canPerformEventHandlerRequests() {
    if (navigator.userActivation) {
      return navigator.userActivation.isActive;
    }
    return JSEvents.inEventHandler && JSEvents.currentEventHandler.allowsDeferredCalls;
  },
  runDeferredCalls() {
    if (!JSEvents.canPerformEventHandlerRequests()) {
      return;
    }
    for (var i = 0; i < JSEvents.deferredCalls.length; ++i) {
      var call = JSEvents.deferredCalls[i];
      JSEvents.deferredCalls.splice(i, 1);
      --i;
      call.targetFunction(...call.argsList);
    }
  },
  eventHandlers: [],
  removeAllHandlersOnTarget: (target, eventTypeString) => {
    for (var i = 0; i < JSEvents.eventHandlers.length; ++i) {
      if (JSEvents.eventHandlers[i].target == target && (!eventTypeString || eventTypeString == JSEvents.eventHandlers[i].eventTypeString)) {
        JSEvents._removeHandler(i--);
      }
    }
  },
  _removeHandler(i) {
    var h = JSEvents.eventHandlers[i];
    h.target.removeEventListener(h.eventTypeString, h.eventListenerFunc, h.useCapture);
    JSEvents.eventHandlers.splice(i, 1);
  },
  registerOrRemoveHandler(eventHandler) {
    if (!eventHandler.target) {
      return -4;
    }
    if (eventHandler.callbackfunc) {
      eventHandler.eventListenerFunc = function (event) {
        ++JSEvents.inEventHandler;
        JSEvents.currentEventHandler = eventHandler;
        JSEvents.runDeferredCalls();
        eventHandler.handlerFunc(event);
        JSEvents.runDeferredCalls();
        --JSEvents.inEventHandler;
      };
      eventHandler.target.addEventListener(eventHandler.eventTypeString, eventHandler.eventListenerFunc, eventHandler.useCapture);
      JSEvents.eventHandlers.push(eventHandler);
    } else {
      for (var i = 0; i < JSEvents.eventHandlers.length; ++i) {
        if (JSEvents.eventHandlers[i].target == eventHandler.target && JSEvents.eventHandlers[i].eventTypeString == eventHandler.eventTypeString) {
          JSEvents._removeHandler(i--);
        }
      }
    }
    return 0;
  },
  getNodeNameForTarget(target) {
    if (!target) return "";
    if (target == window) return "#window";
    if (target == screen) return "#screen";
    return (target === null || target === void 0 ? void 0 : target.nodeName) || "";
  },
  fullscreenEnabled() {
    return document.fullscreenEnabled || document.webkitFullscreenEnabled;
  }
};
var maybeCStringToJsString = cString => cString > 2 ? UTF8ToString(cString) : cString;
var specialHTMLTargets = [0, document, window];
var findEventTarget = target => {
  target = maybeCStringToJsString(target);
  var domElement = specialHTMLTargets[target] || document.querySelector(target);
  return domElement;
};
var findCanvasEventTarget = findEventTarget;
var _emscripten_set_canvas_element_size = (target, width, height) => {
  var canvas = findCanvasEventTarget(target);
  if (!canvas) return -4;
  canvas.width = width;
  canvas.height = height;
  return 0;
};
var stringToUTF8Array = (str, heap, outIdx, maxBytesToWrite) => {
  if (!(maxBytesToWrite > 0)) return 0;
  var startIdx = outIdx;
  var endIdx = outIdx + maxBytesToWrite - 1;
  for (var i = 0; i < str.length; ++i) {
    var u = str.charCodeAt(i);
    if (u >= 55296 && u <= 57343) {
      var u1 = str.charCodeAt(++i);
      u = 65536 + ((u & 1023) << 10) | u1 & 1023;
    }
    if (u <= 127) {
      if (outIdx >= endIdx) break;
      heap[outIdx++] = u;
    } else if (u <= 2047) {
      if (outIdx + 1 >= endIdx) break;
      heap[outIdx++] = 192 | u >> 6;
      heap[outIdx++] = 128 | u & 63;
    } else if (u <= 65535) {
      if (outIdx + 2 >= endIdx) break;
      heap[outIdx++] = 224 | u >> 12;
      heap[outIdx++] = 128 | u >> 6 & 63;
      heap[outIdx++] = 128 | u & 63;
    } else {
      if (outIdx + 3 >= endIdx) break;
      heap[outIdx++] = 240 | u >> 18;
      heap[outIdx++] = 128 | u >> 12 & 63;
      heap[outIdx++] = 128 | u >> 6 & 63;
      heap[outIdx++] = 128 | u & 63;
    }
  }
  heap[outIdx] = 0;
  return outIdx - startIdx;
};
var stringToUTF8 = (str, outPtr, maxBytesToWrite) => stringToUTF8Array(str, HEAPU8, outPtr, maxBytesToWrite);
var registerKeyEventCallback = (target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString, targetThread) => {
  if (!JSEvents.keyEvent) JSEvents.keyEvent = _malloc(176);
  var keyEventHandlerFunc = e => {
    var keyEventData = JSEvents.keyEvent;
    HEAPF64[keyEventData >> 3] = e.timeStamp;
    var idx = keyEventData >> 2;
    HEAP32[idx + 2] = e.location;
    HEAP32[idx + 3] = e.ctrlKey;
    HEAP32[idx + 4] = e.shiftKey;
    HEAP32[idx + 5] = e.altKey;
    HEAP32[idx + 6] = e.metaKey;
    HEAP32[idx + 7] = e.repeat;
    HEAP32[idx + 8] = e.charCode;
    HEAP32[idx + 9] = e.keyCode;
    HEAP32[idx + 10] = e.which;
    stringToUTF8(e.key || "", keyEventData + 44, 32);
    stringToUTF8(e.code || "", keyEventData + 76, 32);
    stringToUTF8(e.char || "", keyEventData + 108, 32);
    stringToUTF8(e.locale || "", keyEventData + 140, 32);
    if (getWasmTableEntry(callbackfunc)(eventTypeId, keyEventData, userData)) e.preventDefault();
  };
  var eventHandler = {
    target: findEventTarget(target),
    eventTypeString: eventTypeString,
    callbackfunc: callbackfunc,
    handlerFunc: keyEventHandlerFunc,
    useCapture: useCapture
  };
  return JSEvents.registerOrRemoveHandler(eventHandler);
};
var _emscripten_set_keydown_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) => registerKeyEventCallback(target, userData, useCapture, callbackfunc, 2, "keydown", targetThread);
var _emscripten_set_keyup_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) => registerKeyEventCallback(target, userData, useCapture, callbackfunc, 3, "keyup", targetThread);
var getBoundingClientRect = e => specialHTMLTargets.indexOf(e) < 0 ? e.getBoundingClientRect() : {
  "left": 0,
  "top": 0
};
var fillMouseEventData = (eventStruct, e, target) => {
  HEAPF64[eventStruct >> 3] = e.timeStamp;
  var idx = eventStruct >> 2;
  HEAP32[idx + 2] = e.screenX;
  HEAP32[idx + 3] = e.screenY;
  HEAP32[idx + 4] = e.clientX;
  HEAP32[idx + 5] = e.clientY;
  HEAP32[idx + 6] = e.ctrlKey;
  HEAP32[idx + 7] = e.shiftKey;
  HEAP32[idx + 8] = e.altKey;
  HEAP32[idx + 9] = e.metaKey;
  HEAP16[idx * 2 + 20] = e.button;
  HEAP16[idx * 2 + 21] = e.buttons;
  HEAP32[idx + 11] = e["movementX"];
  HEAP32[idx + 12] = e["movementY"];
  var rect = getBoundingClientRect(target);
  HEAP32[idx + 13] = e.clientX - rect.left;
  HEAP32[idx + 14] = e.clientY - rect.top;
};
var registerMouseEventCallback = (target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString, targetThread) => {
  if (!JSEvents.mouseEvent) JSEvents.mouseEvent = _malloc(72);
  target = findEventTarget(target);
  var mouseEventHandlerFunc = function () {
    let e = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : event;
    fillMouseEventData(JSEvents.mouseEvent, e, target);
    if (getWasmTableEntry(callbackfunc)(eventTypeId, JSEvents.mouseEvent, userData)) e.preventDefault();
  };
  var eventHandler = {
    target: target,
    allowsDeferredCalls: eventTypeString != "mousemove" && eventTypeString != "mouseenter" && eventTypeString != "mouseleave",
    eventTypeString: eventTypeString,
    callbackfunc: callbackfunc,
    handlerFunc: mouseEventHandlerFunc,
    useCapture: useCapture
  };
  return JSEvents.registerOrRemoveHandler(eventHandler);
};
var _emscripten_set_mousedown_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) => registerMouseEventCallback(target, userData, useCapture, callbackfunc, 5, "mousedown", targetThread);
var _emscripten_set_mousemove_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) => registerMouseEventCallback(target, userData, useCapture, callbackfunc, 8, "mousemove", targetThread);
var _emscripten_set_mouseup_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) => registerMouseEventCallback(target, userData, useCapture, callbackfunc, 6, "mouseup", targetThread);
class HandleAllocator {
  constructor() {
    this.allocated = [undefined];
    this.freelist = [];
  }
  get(id) {
    return this.allocated[id];
  }
  has(id) {
    return this.allocated[id] !== undefined;
  }
  allocate(handle) {
    var id = this.freelist.pop() || this.allocated.length;
    this.allocated[id] = handle;
    return id;
  }
  free(id) {
    this.allocated[id] = undefined;
    this.freelist.push(id);
  }
}
var Fetch = {
  openDatabase(dbname, dbversion, onsuccess, onerror) {
    try {
      var openRequest = indexedDB.open(dbname, dbversion);
    } catch (e) {
      return onerror(e);
    }
    openRequest.onupgradeneeded = event => {
      var db = event.target.result;
      if (db.objectStoreNames.contains("FILES")) {
        db.deleteObjectStore("FILES");
      }
      db.createObjectStore("FILES");
    };
    openRequest.onsuccess = event => onsuccess(event.target.result);
    openRequest.onerror = onerror;
  },
  init() {
    Fetch.xhrs = new HandleAllocator();
    var onsuccess = db => {
      Fetch.dbInstance = db;
      removeRunDependency("library_fetch_init");
    };
    var onerror = () => {
      Fetch.dbInstance = false;
      removeRunDependency("library_fetch_init");
    };
    addRunDependency("library_fetch_init");
    Fetch.openDatabase("emscripten_filesystem", 1, onsuccess, onerror);
  }
};
function fetchXHR(fetch, onsuccess, onerror, onprogress, onreadystatechange) {
  var url = HEAPU32[fetch + 8 >> 2];
  if (!url) {
    onerror(fetch, 0, "no url specified!");
    return;
  }
  var url_ = UTF8ToString(url);
  var fetch_attr = fetch + 112;
  var requestMethod = UTF8ToString(fetch_attr + 0);
  requestMethod || (requestMethod = "GET");
  var timeoutMsecs = HEAPU32[fetch_attr + 56 >> 2];
  var userName = HEAPU32[fetch_attr + 68 >> 2];
  var password = HEAPU32[fetch_attr + 72 >> 2];
  var requestHeaders = HEAPU32[fetch_attr + 76 >> 2];
  var overriddenMimeType = HEAPU32[fetch_attr + 80 >> 2];
  var dataPtr = HEAPU32[fetch_attr + 84 >> 2];
  var dataLength = HEAPU32[fetch_attr + 88 >> 2];
  var fetchAttributes = HEAPU32[fetch_attr + 52 >> 2];
  var fetchAttrLoadToMemory = !!(fetchAttributes & 1);
  var fetchAttrStreamData = !!(fetchAttributes & 2);
  var fetchAttrSynchronous = !!(fetchAttributes & 64);
  var userNameStr = userName ? UTF8ToString(userName) : undefined;
  var passwordStr = password ? UTF8ToString(password) : undefined;
  var xhr = new XMLHttpRequest();
  xhr.withCredentials = !!HEAPU8[fetch_attr + 60];
  xhr.open(requestMethod, url_, !fetchAttrSynchronous, userNameStr, passwordStr);
  if (!fetchAttrSynchronous) xhr.timeout = timeoutMsecs;
  xhr.url_ = url_;
  xhr.responseType = "arraybuffer";
  if (overriddenMimeType) {
    var overriddenMimeTypeStr = UTF8ToString(overriddenMimeType);
    xhr.overrideMimeType(overriddenMimeTypeStr);
  }
  if (requestHeaders) {
    for (;;) {
      var key = HEAPU32[requestHeaders >> 2];
      if (!key) break;
      var value = HEAPU32[requestHeaders + 4 >> 2];
      if (!value) break;
      requestHeaders += 8;
      var keyStr = UTF8ToString(key);
      var valueStr = UTF8ToString(value);
      xhr.setRequestHeader(keyStr, valueStr);
    }
  }
  var id = Fetch.xhrs.allocate(xhr);
  HEAPU32[fetch >> 2] = id;
  var data = dataPtr && dataLength ? HEAPU8.slice(dataPtr, dataPtr + dataLength) : null;
  function saveResponseAndStatus() {
    var ptr = 0;
    var ptrLen = 0;
    if (xhr.response && fetchAttrLoadToMemory && HEAPU32[fetch + 12 >> 2] === 0) {
      ptrLen = xhr.response.byteLength;
    }
    if (ptrLen > 0) {
      ptr = _malloc(ptrLen);
      HEAPU8.set(new Uint8Array(xhr.response), ptr);
    }
    HEAPU32[fetch + 12 >> 2] = ptr;
    writeI53ToI64(fetch + 16, ptrLen);
    writeI53ToI64(fetch + 24, 0);
    var len = xhr.response ? xhr.response.byteLength : 0;
    if (len) {
      writeI53ToI64(fetch + 32, len);
    }
    HEAP16[fetch + 40 >> 1] = xhr.readyState;
    HEAP16[fetch + 42 >> 1] = xhr.status;
    if (xhr.statusText) stringToUTF8(xhr.statusText, fetch + 44, 64);
  }
  xhr.onload = e => {
    if (!Fetch.xhrs.has(id)) {
      return;
    }
    saveResponseAndStatus();
    if (xhr.status >= 200 && xhr.status < 300) {
      onsuccess === null || onsuccess === void 0 || onsuccess(fetch, xhr, e);
    } else {
      onerror === null || onerror === void 0 || onerror(fetch, xhr, e);
    }
  };
  xhr.onerror = e => {
    if (!Fetch.xhrs.has(id)) {
      return;
    }
    saveResponseAndStatus();
    onerror === null || onerror === void 0 || onerror(fetch, xhr, e);
  };
  xhr.ontimeout = e => {
    if (!Fetch.xhrs.has(id)) {
      return;
    }
    onerror === null || onerror === void 0 || onerror(fetch, xhr, e);
  };
  xhr.onprogress = e => {
    if (!Fetch.xhrs.has(id)) {
      return;
    }
    var ptrLen = fetchAttrLoadToMemory && fetchAttrStreamData && xhr.response ? xhr.response.byteLength : 0;
    var ptr = 0;
    if (ptrLen > 0 && fetchAttrLoadToMemory && fetchAttrStreamData) {
      ptr = _malloc(ptrLen);
      HEAPU8.set(new Uint8Array(xhr.response), ptr);
    }
    HEAPU32[fetch + 12 >> 2] = ptr;
    writeI53ToI64(fetch + 16, ptrLen);
    writeI53ToI64(fetch + 24, e.loaded - ptrLen);
    writeI53ToI64(fetch + 32, e.total);
    HEAP16[fetch + 40 >> 1] = xhr.readyState;
    if (xhr.readyState >= 3 && xhr.status === 0 && e.loaded > 0) xhr.status = 200;
    HEAP16[fetch + 42 >> 1] = xhr.status;
    if (xhr.statusText) stringToUTF8(xhr.statusText, fetch + 44, 64);
    onprogress === null || onprogress === void 0 || onprogress(fetch, xhr, e);
    if (ptr) {
      _free(ptr);
    }
  };
  xhr.onreadystatechange = e => {
    if (!Fetch.xhrs.has(id)) {
      return;
    }
    HEAP16[fetch + 40 >> 1] = xhr.readyState;
    if (xhr.readyState >= 2) {
      HEAP16[fetch + 42 >> 1] = xhr.status;
    }
    onreadystatechange === null || onreadystatechange === void 0 || onreadystatechange(fetch, xhr, e);
  };
  try {
    xhr.send(data);
  } catch (e) {
    onerror === null || onerror === void 0 || onerror(fetch, xhr, e);
  }
}
var handleException = e => {
  if (e instanceof ExitStatus || e == "unwind") {
    return EXITSTATUS;
  }
  quit_(1, e);
};
var runtimeKeepaliveCounter = 0;
var keepRuntimeAlive = () => noExitRuntime || runtimeKeepaliveCounter > 0;
var _proc_exit = code => {
  EXITSTATUS = code;
  if (!keepRuntimeAlive()) {
    var _Module$onExit;
    (_Module$onExit = Module["onExit"]) === null || _Module$onExit === void 0 || _Module$onExit.call(Module, code);
    ABORT = true;
  }
  quit_(code, new ExitStatus(code));
};
var exitJS = (status, implicit) => {
  EXITSTATUS = status;
  _proc_exit(status);
};
var _exit = exitJS;
var maybeExit = () => {
  if (!keepRuntimeAlive()) {
    try {
      _exit(EXITSTATUS);
    } catch (e) {
      handleException(e);
    }
  }
};
var callUserCallback = func => {
  if (ABORT) {
    return;
  }
  try {
    func();
    maybeExit();
  } catch (e) {
    handleException(e);
  }
};
var writeI53ToI64 = (ptr, num) => {
  HEAPU32[ptr >> 2] = num;
  var lower = HEAPU32[ptr >> 2];
  HEAPU32[ptr + 4 >> 2] = (num - lower) / 4294967296;
};
function fetchCacheData(db, fetch, data, onsuccess, onerror) {
  if (!db) {
    onerror(fetch, 0, "IndexedDB not available!");
    return;
  }
  var fetch_attr = fetch + 112;
  var destinationPath = HEAPU32[fetch_attr + 64 >> 2];
  destinationPath || (destinationPath = HEAPU32[fetch + 8 >> 2]);
  var destinationPathStr = UTF8ToString(destinationPath);
  try {
    var transaction = db.transaction(["FILES"], "readwrite");
    var packages = transaction.objectStore("FILES");
    var putRequest = packages.put(data, destinationPathStr);
    putRequest.onsuccess = event => {
      HEAP16[fetch + 40 >> 1] = 4;
      HEAP16[fetch + 42 >> 1] = 200;
      stringToUTF8("OK", fetch + 44, 64);
      onsuccess(fetch, 0, destinationPathStr);
    };
    putRequest.onerror = error => {
      HEAP16[fetch + 40 >> 1] = 4;
      HEAP16[fetch + 42 >> 1] = 413;
      stringToUTF8("Payload Too Large", fetch + 44, 64);
      onerror(fetch, 0, error);
    };
  } catch (e) {
    onerror(fetch, 0, e);
  }
}
function fetchLoadCachedData(db, fetch, onsuccess, onerror) {
  if (!db) {
    onerror(fetch, 0, "IndexedDB not available!");
    return;
  }
  var fetch_attr = fetch + 112;
  var path = HEAPU32[fetch_attr + 64 >> 2];
  path || (path = HEAPU32[fetch + 8 >> 2]);
  var pathStr = UTF8ToString(path);
  try {
    var transaction = db.transaction(["FILES"], "readonly");
    var packages = transaction.objectStore("FILES");
    var getRequest = packages.get(pathStr);
    getRequest.onsuccess = event => {
      if (event.target.result) {
        var value = event.target.result;
        var len = value.byteLength || value.length;
        var ptr = _malloc(len);
        HEAPU8.set(new Uint8Array(value), ptr);
        HEAPU32[fetch + 12 >> 2] = ptr;
        writeI53ToI64(fetch + 16, len);
        writeI53ToI64(fetch + 24, 0);
        writeI53ToI64(fetch + 32, len);
        HEAP16[fetch + 40 >> 1] = 4;
        HEAP16[fetch + 42 >> 1] = 200;
        stringToUTF8("OK", fetch + 44, 64);
        onsuccess(fetch, 0, value);
      } else {
        HEAP16[fetch + 40 >> 1] = 4;
        HEAP16[fetch + 42 >> 1] = 404;
        stringToUTF8("Not Found", fetch + 44, 64);
        onerror(fetch, 0, "no data");
      }
    };
    getRequest.onerror = error => {
      HEAP16[fetch + 40 >> 1] = 4;
      HEAP16[fetch + 42 >> 1] = 404;
      stringToUTF8("Not Found", fetch + 44, 64);
      onerror(fetch, 0, error);
    };
  } catch (e) {
    onerror(fetch, 0, e);
  }
}
function fetchDeleteCachedData(db, fetch, onsuccess, onerror) {
  if (!db) {
    onerror(fetch, 0, "IndexedDB not available!");
    return;
  }
  var fetch_attr = fetch + 112;
  var path = HEAPU32[fetch_attr + 64 >> 2];
  path || (path = HEAPU32[fetch + 8 >> 2]);
  var pathStr = UTF8ToString(path);
  try {
    var transaction = db.transaction(["FILES"], "readwrite");
    var packages = transaction.objectStore("FILES");
    var request = packages.delete(pathStr);
    request.onsuccess = event => {
      var value = event.target.result;
      HEAPU32[fetch + 12 >> 2] = 0;
      writeI53ToI64(fetch + 16, 0);
      writeI53ToI64(fetch + 24, 0);
      writeI53ToI64(fetch + 32, 0);
      HEAP16[fetch + 40 >> 1] = 4;
      HEAP16[fetch + 42 >> 1] = 200;
      stringToUTF8("OK", fetch + 44, 64);
      onsuccess(fetch, 0, value);
    };
    request.onerror = error => {
      HEAP16[fetch + 40 >> 1] = 4;
      HEAP16[fetch + 42 >> 1] = 404;
      stringToUTF8("Not Found", fetch + 44, 64);
      onerror(fetch, 0, error);
    };
  } catch (e) {
    onerror(fetch, 0, e);
  }
}
function _emscripten_start_fetch(fetch, successcb, errorcb, progresscb, readystatechangecb) {
  var fetch_attr = fetch + 112;
  var onsuccess = HEAPU32[fetch_attr + 36 >> 2];
  var onerror = HEAPU32[fetch_attr + 40 >> 2];
  var onprogress = HEAPU32[fetch_attr + 44 >> 2];
  var onreadystatechange = HEAPU32[fetch_attr + 48 >> 2];
  var fetchAttributes = HEAPU32[fetch_attr + 52 >> 2];
  var fetchAttrSynchronous = !!(fetchAttributes & 64);
  function doCallback(f) {
    if (fetchAttrSynchronous) {
      f();
    } else {
      callUserCallback(f);
    }
  }
  var reportSuccess = (fetch, xhr, e) => {
    doCallback(() => {
      if (onsuccess) getWasmTableEntry(onsuccess)(fetch);else successcb === null || successcb === void 0 || successcb(fetch);
    });
  };
  var reportProgress = (fetch, xhr, e) => {
    doCallback(() => {
      if (onprogress) getWasmTableEntry(onprogress)(fetch);else progresscb === null || progresscb === void 0 || progresscb(fetch);
    });
  };
  var reportError = (fetch, xhr, e) => {
    doCallback(() => {
      if (onerror) getWasmTableEntry(onerror)(fetch);else errorcb === null || errorcb === void 0 || errorcb(fetch);
    });
  };
  var reportReadyStateChange = (fetch, xhr, e) => {
    doCallback(() => {
      if (onreadystatechange) getWasmTableEntry(onreadystatechange)(fetch);else readystatechangecb === null || readystatechangecb === void 0 || readystatechangecb(fetch);
    });
  };
  var performUncachedXhr = (fetch, xhr, e) => {
    fetchXHR(fetch, reportSuccess, reportError, reportProgress, reportReadyStateChange);
  };
  var cacheResultAndReportSuccess = (fetch, xhr, e) => {
    var storeSuccess = (fetch, xhr, e) => {
      doCallback(() => {
        if (onsuccess) getWasmTableEntry(onsuccess)(fetch);else successcb === null || successcb === void 0 || successcb(fetch);
      });
    };
    var storeError = (fetch, xhr, e) => {
      doCallback(() => {
        if (onsuccess) getWasmTableEntry(onsuccess)(fetch);else successcb === null || successcb === void 0 || successcb(fetch);
      });
    };
    fetchCacheData(Fetch.dbInstance, fetch, xhr.response, storeSuccess, storeError);
  };
  var performCachedXhr = (fetch, xhr, e) => {
    fetchXHR(fetch, cacheResultAndReportSuccess, reportError, reportProgress, reportReadyStateChange);
  };
  var requestMethod = UTF8ToString(fetch_attr + 0);
  var fetchAttrReplace = !!(fetchAttributes & 16);
  var fetchAttrPersistFile = !!(fetchAttributes & 4);
  var fetchAttrNoDownload = !!(fetchAttributes & 32);
  if (requestMethod === "EM_IDB_STORE") {
    var ptr = HEAPU32[fetch_attr + 84 >> 2];
    var size = HEAPU32[fetch_attr + 88 >> 2];
    fetchCacheData(Fetch.dbInstance, fetch, HEAPU8.slice(ptr, ptr + size), reportSuccess, reportError);
  } else if (requestMethod === "EM_IDB_DELETE") {
    fetchDeleteCachedData(Fetch.dbInstance, fetch, reportSuccess, reportError);
  } else if (!fetchAttrReplace) {
    fetchLoadCachedData(Fetch.dbInstance, fetch, reportSuccess, fetchAttrNoDownload ? reportError : fetchAttrPersistFile ? performCachedXhr : performUncachedXhr);
  } else if (!fetchAttrNoDownload) {
    fetchXHR(fetch, fetchAttrPersistFile ? cacheResultAndReportSuccess : reportSuccess, reportError, reportProgress, reportReadyStateChange);
  } else {
    return 0;
  }
  return fetch;
}
var GL = {
  counter: 1,
  buffers: [],
  programs: [],
  framebuffers: [],
  renderbuffers: [],
  textures: [],
  shaders: [],
  vaos: [],
  contexts: [],
  offscreenCanvases: {},
  queries: [],
  samplers: [],
  transformFeedbacks: [],
  syncs: [],
  stringCache: {},
  stringiCache: {},
  unpackAlignment: 4,
  recordError: errorCode => {},
  getNewId: table => {
    var ret = GL.counter++;
    for (var i = table.length; i < ret; i++) {
      table[i] = null;
    }
    return ret;
  },
  genObject: (n, buffers, createFunction, objectTable) => {
    for (var i = 0; i < n; i++) {
      var buffer = GLctx[createFunction]();
      var id = buffer && GL.getNewId(objectTable);
      if (buffer) {
        buffer.name = id;
        objectTable[id] = buffer;
      } else {
        GL.recordError(1282);
      }
      HEAP32[buffers + i * 4 >> 2] = id;
    }
  },
  getSource: (shader, count, string, length) => {
    var source = "";
    for (var i = 0; i < count; ++i) {
      var len = length ? HEAPU32[length + i * 4 >> 2] : undefined;
      source += UTF8ToString(HEAPU32[string + i * 4 >> 2], len);
    }
    return source;
  },
  createContext: (canvas, webGLContextAttributes) => {
    if (!canvas.getContextSafariWebGL2Fixed) {
      canvas.getContextSafariWebGL2Fixed = canvas.getContext;
      function fixedGetContext(ver, attrs) {
        var gl = canvas.getContextSafariWebGL2Fixed(ver, attrs);
        return ver == "webgl" == gl instanceof WebGLRenderingContext ? gl : null;
      }
      canvas.getContext = fixedGetContext;
    }
    var ctx = canvas.getContext("webgl2", webGLContextAttributes);
    if (!ctx) return 0;
    var handle = GL.registerContext(ctx, webGLContextAttributes);
    return handle;
  },
  registerContext: (ctx, webGLContextAttributes) => {
    var handle = GL.getNewId(GL.contexts);
    var context = {
      handle: handle,
      attributes: webGLContextAttributes,
      version: webGLContextAttributes.majorVersion,
      GLctx: ctx
    };
    if (ctx.canvas) ctx.canvas.GLctxObject = context;
    GL.contexts[handle] = context;
    return handle;
  },
  makeContextCurrent: contextHandle => {
    var _GL$currentContext;
    GL.currentContext = GL.contexts[contextHandle];
    Module.ctx = GLctx = (_GL$currentContext = GL.currentContext) === null || _GL$currentContext === void 0 ? void 0 : _GL$currentContext.GLctx;
    return !(contextHandle && !GLctx);
  },
  getContext: contextHandle => GL.contexts[contextHandle],
  deleteContext: contextHandle => {
    if (GL.currentContext === GL.contexts[contextHandle]) {
      GL.currentContext = null;
    }
    if (typeof JSEvents == "object") {
      JSEvents.removeAllHandlersOnTarget(GL.contexts[contextHandle].GLctx.canvas);
    }
    if (GL.contexts[contextHandle] && GL.contexts[contextHandle].GLctx.canvas) {
      GL.contexts[contextHandle].GLctx.canvas.GLctxObject = undefined;
    }
    GL.contexts[contextHandle] = null;
  }
};
var webglPowerPreferences = ["default", "low-power", "high-performance"];
var _emscripten_webgl_do_create_context = (target, attributes) => {
  var a = attributes >> 2;
  var powerPreference = HEAP32[a + (24 >> 2)];
  var contextAttributes = {
    "alpha": !!HEAP32[a + (0 >> 2)],
    "depth": !!HEAP32[a + (4 >> 2)],
    "stencil": !!HEAP32[a + (8 >> 2)],
    "antialias": !!HEAP32[a + (12 >> 2)],
    "premultipliedAlpha": !!HEAP32[a + (16 >> 2)],
    "preserveDrawingBuffer": !!HEAP32[a + (20 >> 2)],
    "powerPreference": webglPowerPreferences[powerPreference],
    "failIfMajorPerformanceCaveat": !!HEAP32[a + (28 >> 2)],
    majorVersion: HEAP32[a + (32 >> 2)],
    minorVersion: HEAP32[a + (36 >> 2)],
    enableExtensionsByDefault: HEAP32[a + (40 >> 2)],
    explicitSwapControl: HEAP32[a + (44 >> 2)],
    proxyContextToMainThread: HEAP32[a + (48 >> 2)],
    renderViaOffscreenBackBuffer: HEAP32[a + (52 >> 2)]
  };
  var canvas = findCanvasEventTarget(target);
  if (!canvas) {
    return 0;
  }
  if (contextAttributes.explicitSwapControl) {
    return 0;
  }
  var contextHandle = GL.createContext(canvas, contextAttributes);
  return contextHandle;
};
var _emscripten_webgl_create_context = _emscripten_webgl_do_create_context;
var _emscripten_webgl_make_context_current = contextHandle => {
  var success = GL.makeContextCurrent(contextHandle);
  return success ? 0 : -5;
};
var _fd_close = fd => 52;
var _fd_read = (fd, iov, iovcnt, pnum) => 52;
var convertI32PairToI53Checked = (lo, hi) => hi + 2097152 >>> 0 < 4194305 - !!lo ? (lo >>> 0) + hi * 4294967296 : NaN;
function _fd_seek(fd, offset_low, offset_high, whence, newOffset) {
  var offset = convertI32PairToI53Checked(offset_low, offset_high);
  return 70;
}
var printCharBuffers = [null, [], []];
var UTF8ArrayToString = (heapOrArray, idx, maxBytesToRead) => {
  var endIdx = idx + maxBytesToRead;
  var endPtr = idx;
  while (heapOrArray[endPtr] && !(endPtr >= endIdx)) ++endPtr;
  return UTF8Decoder.decode(heapOrArray.buffer ? heapOrArray.subarray(idx, endPtr) : new Uint8Array(heapOrArray.slice(idx, endPtr)));
};
var printChar = (stream, curr) => {
  var buffer = printCharBuffers[stream];
  if (curr === 0 || curr === 10) {
    (stream === 1 ? out : err)(UTF8ArrayToString(buffer, 0));
    buffer.length = 0;
  } else {
    buffer.push(curr);
  }
};
var _fd_write = (fd, iov, iovcnt, pnum) => {
  var num = 0;
  for (var i = 0; i < iovcnt; i++) {
    var ptr = HEAPU32[iov >> 2];
    var len = HEAPU32[iov + 4 >> 2];
    iov += 8;
    for (var j = 0; j < len; j++) {
      printChar(fd, HEAPU8[ptr + j]);
    }
    num += len;
  }
  HEAPU32[pnum >> 2] = num;
  return 0;
};
var _glActiveTexture = x0 => GLctx.activeTexture(x0);
var _glAttachShader = (program, shader) => {
  GLctx.attachShader(GL.programs[program], GL.shaders[shader]);
};
var _glBindBuffer = (target, buffer) => {
  if (target == 35051) {
    GLctx.currentPixelPackBufferBinding = buffer;
  } else if (target == 35052) {
    GLctx.currentPixelUnpackBufferBinding = buffer;
  }
  GLctx.bindBuffer(target, GL.buffers[buffer]);
};
var _glBindFramebuffer = (target, framebuffer) => {
  GLctx.bindFramebuffer(target, GL.framebuffers[framebuffer]);
};
var _glBindTexture = (target, texture) => {
  GLctx.bindTexture(target, GL.textures[texture]);
};
var _glBindVertexArray = vao => {
  GLctx.bindVertexArray(GL.vaos[vao]);
};
var _glBlendEquation = x0 => GLctx.blendEquation(x0);
var _glBlendFunc = (x0, x1) => GLctx.blendFunc(x0, x1);
var _glBufferData = (target, size, data, usage) => {
  if (true) {
    if (data && size) {
      GLctx.bufferData(target, HEAPU8, usage, data, size);
    } else {
      GLctx.bufferData(target, size, usage);
    }
  } else {
    GLctx.bufferData(target, data ? HEAPU8.subarray(data, data + size) : size, usage);
  }
};
var _glClear = x0 => GLctx.clear(x0);
var _glClearColor = (x0, x1, x2, x3) => GLctx.clearColor(x0, x1, x2, x3);
var _glCompileShader = shader => {
  GLctx.compileShader(GL.shaders[shader]);
};
var _glCreateProgram = () => {
  var id = GL.getNewId(GL.programs);
  var program = GLctx.createProgram();
  program.name = id;
  program.maxUniformLength = program.maxAttributeLength = program.maxUniformBlockNameLength = 0;
  program.uniformIdCounter = 1;
  GL.programs[id] = program;
  return id;
};
var _glCreateShader = shaderType => {
  var id = GL.getNewId(GL.shaders);
  GL.shaders[id] = GLctx.createShader(shaderType);
  return id;
};
var _glDeleteBuffers = (n, buffers) => {
  for (var i = 0; i < n; i++) {
    var id = HEAP32[buffers + i * 4 >> 2];
    var buffer = GL.buffers[id];
    if (!buffer) continue;
    GLctx.deleteBuffer(buffer);
    buffer.name = 0;
    GL.buffers[id] = null;
    if (id == GLctx.currentPixelPackBufferBinding) GLctx.currentPixelPackBufferBinding = 0;
    if (id == GLctx.currentPixelUnpackBufferBinding) GLctx.currentPixelUnpackBufferBinding = 0;
  }
};
var _glDeleteFramebuffers = (n, framebuffers) => {
  for (var i = 0; i < n; ++i) {
    var id = HEAP32[framebuffers + i * 4 >> 2];
    var framebuffer = GL.framebuffers[id];
    if (!framebuffer) continue;
    GLctx.deleteFramebuffer(framebuffer);
    framebuffer.name = 0;
    GL.framebuffers[id] = null;
  }
};
var _glDeleteProgram = id => {
  if (!id) return;
  var program = GL.programs[id];
  if (!program) {
    GL.recordError(1281);
    return;
  }
  GLctx.deleteProgram(program);
  program.name = 0;
  GL.programs[id] = null;
};
var _glDeleteShader = id => {
  if (!id) return;
  var shader = GL.shaders[id];
  if (!shader) {
    GL.recordError(1281);
    return;
  }
  GLctx.deleteShader(shader);
  GL.shaders[id] = null;
};
var _glDeleteTextures = (n, textures) => {
  for (var i = 0; i < n; i++) {
    var id = HEAP32[textures + i * 4 >> 2];
    var texture = GL.textures[id];
    if (!texture) continue;
    GLctx.deleteTexture(texture);
    texture.name = 0;
    GL.textures[id] = null;
  }
};
var _glDeleteVertexArrays = (n, vaos) => {
  for (var i = 0; i < n; i++) {
    var id = HEAP32[vaos + i * 4 >> 2];
    GLctx.deleteVertexArray(GL.vaos[id]);
    GL.vaos[id] = null;
  }
};
var _glDisable = x0 => GLctx.disable(x0);
var _glDrawArraysInstanced = (mode, first, count, primcount) => {
  GLctx.drawArraysInstanced(mode, first, count, primcount);
};
var _glDrawElements = (mode, count, type, indices) => {
  GLctx.drawElements(mode, count, type, indices);
};
var _glEnable = x0 => GLctx.enable(x0);
var _glEnableVertexAttribArray = index => {
  GLctx.enableVertexAttribArray(index);
};
var _glFramebufferTexture2D = (target, attachment, textarget, texture, level) => {
  GLctx.framebufferTexture2D(target, attachment, textarget, GL.textures[texture], level);
};
var _glGenBuffers = (n, buffers) => {
  GL.genObject(n, buffers, "createBuffer", GL.buffers);
};
var _glGenFramebuffers = (n, ids) => {
  GL.genObject(n, ids, "createFramebuffer", GL.framebuffers);
};
var _glGenTextures = (n, textures) => {
  GL.genObject(n, textures, "createTexture", GL.textures);
};
var _glGenVertexArrays = (n, arrays) => {
  GL.genObject(n, arrays, "createVertexArray", GL.vaos);
};
var _glGetAttribLocation = (program, name) => GLctx.getAttribLocation(GL.programs[program], UTF8ToString(name));
var _glGetProgramInfoLog = (program, maxLength, length, infoLog) => {
  var log = GLctx.getProgramInfoLog(GL.programs[program]);
  var numBytesWrittenExclNull = maxLength > 0 && infoLog ? stringToUTF8(log, infoLog, maxLength) : 0;
  if (length) HEAP32[length >> 2] = numBytesWrittenExclNull;
};
var _glGetProgramiv = (program, pname, p) => {
  if (!p) {
    GL.recordError(1281);
    return;
  }
  if (program >= GL.counter) {
    GL.recordError(1281);
    return;
  }
  program = GL.programs[program];
  if (pname == 35716) {
    var log = GLctx.getProgramInfoLog(program);
    HEAP32[p >> 2] = log.length + 1;
  } else if (pname == 35719) {
    if (!program.maxUniformLength) {
      for (var i = 0; i < GLctx.getProgramParameter(program, 35718); ++i) {
        program.maxUniformLength = Math.max(program.maxUniformLength, GLctx.getActiveUniform(program, i).name.length + 1);
      }
    }
    HEAP32[p >> 2] = program.maxUniformLength;
  } else if (pname == 35722) {
    if (!program.maxAttributeLength) {
      for (var i = 0; i < GLctx.getProgramParameter(program, 35721); ++i) {
        program.maxAttributeLength = Math.max(program.maxAttributeLength, GLctx.getActiveAttrib(program, i).name.length + 1);
      }
    }
    HEAP32[p >> 2] = program.maxAttributeLength;
  } else if (pname == 35381) {
    if (!program.maxUniformBlockNameLength) {
      for (var i = 0; i < GLctx.getProgramParameter(program, 35382); ++i) {
        program.maxUniformBlockNameLength = Math.max(program.maxUniformBlockNameLength, GLctx.getActiveUniformBlockName(program, i).length + 1);
      }
    }
    HEAP32[p >> 2] = program.maxUniformBlockNameLength;
  } else {
    HEAP32[p >> 2] = GLctx.getProgramParameter(program, pname);
  }
};
var _glGetShaderInfoLog = (shader, maxLength, length, infoLog) => {
  var log = GLctx.getShaderInfoLog(GL.shaders[shader]);
  var numBytesWrittenExclNull = maxLength > 0 && infoLog ? stringToUTF8(log, infoLog, maxLength) : 0;
  if (length) HEAP32[length >> 2] = numBytesWrittenExclNull;
};
var _glGetShaderiv = (shader, pname, p) => {
  if (!p) {
    GL.recordError(1281);
    return;
  }
  if (pname == 35716) {
    var log = GLctx.getShaderInfoLog(GL.shaders[shader]);
    var logLength = log ? log.length + 1 : 0;
    HEAP32[p >> 2] = logLength;
  } else if (pname == 35720) {
    var source = GLctx.getShaderSource(GL.shaders[shader]);
    var sourceLength = source ? source.length + 1 : 0;
    HEAP32[p >> 2] = sourceLength;
  } else {
    HEAP32[p >> 2] = GLctx.getShaderParameter(GL.shaders[shader], pname);
  }
};
var jstoi_q = str => parseInt(str);
var webglGetLeftBracePos = name => name.slice(-1) == "]" && name.lastIndexOf("[");
var webglPrepareUniformLocationsBeforeFirstUse = program => {
  var uniformLocsById = program.uniformLocsById,
    uniformSizeAndIdsByName = program.uniformSizeAndIdsByName,
    i,
    j;
  if (!uniformLocsById) {
    program.uniformLocsById = uniformLocsById = {};
    program.uniformArrayNamesById = {};
    for (i = 0; i < GLctx.getProgramParameter(program, 35718); ++i) {
      var u = GLctx.getActiveUniform(program, i);
      var nm = u.name;
      var sz = u.size;
      var lb = webglGetLeftBracePos(nm);
      var arrayName = lb > 0 ? nm.slice(0, lb) : nm;
      var id = program.uniformIdCounter;
      program.uniformIdCounter += sz;
      uniformSizeAndIdsByName[arrayName] = [sz, id];
      for (j = 0; j < sz; ++j) {
        uniformLocsById[id] = j;
        program.uniformArrayNamesById[id++] = arrayName;
      }
    }
  }
};
var _glGetUniformLocation = (program, name) => {
  name = UTF8ToString(name);
  if (program = GL.programs[program]) {
    webglPrepareUniformLocationsBeforeFirstUse(program);
    var uniformLocsById = program.uniformLocsById;
    var arrayIndex = 0;
    var uniformBaseName = name;
    var leftBrace = webglGetLeftBracePos(name);
    if (leftBrace > 0) {
      arrayIndex = jstoi_q(name.slice(leftBrace + 1)) >>> 0;
      uniformBaseName = name.slice(0, leftBrace);
    }
    var sizeAndId = program.uniformSizeAndIdsByName[uniformBaseName];
    if (sizeAndId && arrayIndex < sizeAndId[0]) {
      arrayIndex += sizeAndId[1];
      if (uniformLocsById[arrayIndex] = uniformLocsById[arrayIndex] || GLctx.getUniformLocation(program, name)) {
        return arrayIndex;
      }
    }
  }
  return -1;
};
var _glLinkProgram = program => {
  program = GL.programs[program];
  GLctx.linkProgram(program);
  program.uniformLocsById = 0;
  program.uniformSizeAndIdsByName = {};
};
var _glShaderSource = (shader, count, string, length) => {
  var source = GL.getSource(shader, count, string, length);
  GLctx.shaderSource(GL.shaders[shader], source);
};
var computeUnpackAlignedImageSize = (width, height, sizePerPixel, alignment) => {
  function roundedToNextMultipleOf(x, y) {
    return x + y - 1 & -y;
  }
  var plainRowSize = width * sizePerPixel;
  var alignedRowSize = roundedToNextMultipleOf(plainRowSize, alignment);
  return height * alignedRowSize;
};
var colorChannelsInGlTextureFormat = format => {
  var colorChannels = {
    5: 3,
    6: 4,
    8: 2,
    29502: 3,
    29504: 4,
    26917: 2,
    26918: 2,
    29846: 3,
    29847: 4
  };
  return colorChannels[format - 6402] || 1;
};
var heapObjectForWebGLType = type => {
  type -= 5120;
  if (type == 0) return HEAP8;
  if (type == 1) return HEAPU8;
  if (type == 2) return HEAP16;
  if (type == 4) return HEAP32;
  if (type == 6) return HEAPF32;
  if (type == 5 || type == 28922 || type == 28520 || type == 30779 || type == 30782) return HEAPU32;
  return HEAPU16;
};
var toTypedArrayIndex = (pointer, heap) => pointer >>> 31 - Math.clz32(heap.BYTES_PER_ELEMENT);
var emscriptenWebGLGetTexPixelData = (type, format, width, height, pixels, internalFormat) => {
  var heap = heapObjectForWebGLType(type);
  var sizePerPixel = colorChannelsInGlTextureFormat(format) * heap.BYTES_PER_ELEMENT;
  var bytes = computeUnpackAlignedImageSize(width, height, sizePerPixel, GL.unpackAlignment);
  return heap.subarray(toTypedArrayIndex(pixels, heap), toTypedArrayIndex(pixels + bytes, heap));
};
var _glTexImage2D = (target, level, internalFormat, width, height, border, format, type, pixels) => {
  if (true) {
    if (GLctx.currentPixelUnpackBufferBinding) {
      GLctx.texImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
    } else if (pixels) {
      var heap = heapObjectForWebGLType(type);
      GLctx.texImage2D(target, level, internalFormat, width, height, border, format, type, heap, toTypedArrayIndex(pixels, heap));
    } else {
      GLctx.texImage2D(target, level, internalFormat, width, height, border, format, type, null);
    }
    return;
  }
  GLctx.texImage2D(target, level, internalFormat, width, height, border, format, type, pixels ? emscriptenWebGLGetTexPixelData(type, format, width, height, pixels, internalFormat) : null);
};
var _glTexParameteri = (x0, x1, x2) => GLctx.texParameteri(x0, x1, x2);
var webglGetUniformLocation = location => {
  var p = GLctx.currentProgram;
  var webglLoc = p.uniformLocsById[location];
  if (typeof webglLoc == "number") {
    p.uniformLocsById[location] = webglLoc = GLctx.getUniformLocation(p, p.uniformArrayNamesById[location] + (webglLoc > 0 ? `[${webglLoc}]` : ""));
  }
  return webglLoc;
};
var _glUniform1i = (location, v0) => {
  GLctx.uniform1i(webglGetUniformLocation(location), v0);
};
var _glUniform2f = (location, v0, v1) => {
  GLctx.uniform2f(webglGetUniformLocation(location), v0, v1);
};
var _glUseProgram = program => {
  program = GL.programs[program];
  GLctx.useProgram(program);
  GLctx.currentProgram = program;
};
var _glVertexAttribDivisor = (index, divisor) => {
  GLctx.vertexAttribDivisor(index, divisor);
};
var _glVertexAttribPointer = (index, size, type, normalized, stride, ptr) => {
  GLctx.vertexAttribPointer(index, size, type, !!normalized, stride, ptr);
};
var _glViewport = (x0, x1, x2, x3) => GLctx.viewport(x0, x1, x2, x3);
function _init_gCanvas(charSize, canvasWidth, canvasHeight, font) {
  var canvas = document.createElement("canvas");
  canvas.width = canvasWidth;
  canvas.height = canvasHeight;
  var ctx = canvas.getContext("2d");
  ctx.font = charSize + "px " + UTF8ToString(font);
  ctx.textBaseline = "middle";
  ctx.globalAlpha = 1;
  ctx.fillStyle = "white";
  window["gCanvas" + charSize] = canvas;
  window["gCanvasCtx" + charSize] = ctx;
}
var _load_texture_from_url = function (glTexture, url, outW, outH) {
  var img = new Image();
  img.onload = () => {
    HEAPU32[outW >> 2] = img.width;
    HEAPU32[outH >> 2] = img.height;
    GLctx.bindTexture(GLctx.TEXTURE_2D, GL.textures[glTexture]);
    GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, img);
  };
  img.src = UTF8ToString(url);
};
function _upload_unicode_char_to_texture(charSize, utf8Char) {
  var canvas = window["gCanvas" + charSize];
  var ctx = window["gCanvasCtx" + charSize];
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  var str = UTF8ToString(utf8Char);
  var strPixelWidth = ctx.measureText(str).width;
  ctx.fillText(str, 0, canvas.height / 2);
  GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, canvas);
  return strPixelWidth;
}
Fetch.init();
var GLctx;
var wasmImports = {
  f: ___assert_fail,
  n: ___cxa_throw,
  C: ___syscall_fcntl64,
  W: ___syscall_ioctl,
  X: ___syscall_openat,
  Y: __emscripten_fetch_free,
  Q: __emscripten_get_now_is_monotonic,
  u: _abort,
  d: _emscripten_asm_const_int,
  R: _emscripten_date_now,
  T: _emscripten_get_now,
  _: _emscripten_is_main_browser_thread,
  ua: _emscripten_request_animation_frame_loop,
  S: _emscripten_resize_heap,
  oa: _emscripten_set_canvas_element_size,
  ta: _emscripten_set_keydown_callback_on_thread,
  sa: _emscripten_set_keyup_callback_on_thread,
  ra: _emscripten_set_mousedown_callback_on_thread,
  pa: _emscripten_set_mousemove_callback_on_thread,
  qa: _emscripten_set_mouseup_callback_on_thread,
  Z: _emscripten_start_fetch,
  na: _emscripten_webgl_create_context,
  ma: _emscripten_webgl_make_context_current,
  A: _fd_close,
  V: _fd_read,
  P: _fd_seek,
  B: _fd_write,
  ka: _glActiveTexture,
  F: _glAttachShader,
  c: _glBindBuffer,
  z: _glBindFramebuffer,
  b: _glBindTexture,
  j: _glBindVertexArray,
  r: _glBlendEquation,
  s: _glBlendFunc,
  k: _glBufferData,
  L: _glClear,
  M: _glClearColor,
  ca: _glCompileShader,
  ha: _glCreateProgram,
  ea: _glCreateShader,
  q: _glDeleteBuffers,
  t: _glDeleteFramebuffers,
  l: _glDeleteProgram,
  e: _glDeleteShader,
  U: _glDeleteTextures,
  J: _glDeleteVertexArrays,
  K: _glDisable,
  x: _glDrawArraysInstanced,
  ia: _glDrawElements,
  la: _glEnable,
  g: _glEnableVertexAttribArray,
  O: _glFramebufferTexture2D,
  p: _glGenBuffers,
  N: _glGenFramebuffers,
  m: _glGenTextures,
  G: _glGenVertexArrays,
  i: _glGetAttribLocation,
  fa: _glGetProgramInfoLog,
  E: _glGetProgramiv,
  ba: _glGetShaderInfoLog,
  D: _glGetShaderiv,
  w: _glGetUniformLocation,
  ga: _glLinkProgram,
  da: _glShaderSource,
  v: _glTexImage2D,
  a: _glTexParameteri,
  ja: _glUniform1i,
  H: _glUniform2f,
  I: _glUseProgram,
  o: _glVertexAttribDivisor,
  h: _glVertexAttribPointer,
  y: _glViewport,
  aa: _init_gCanvas,
  va: _load_texture_from_url,
  $: _upload_unicode_char_to_texture
};
var wasmExports = createWasm();
var ___wasm_call_ctors = () => (___wasm_call_ctors = wasmExports["xa"])();
var _malloc = a0 => (_malloc = wasmExports["ya"])(a0);
var _free = a0 => (_free = wasmExports["za"])(a0);
var __Z35ma_device__on_notification_unlockedP9ma_device = Module["__Z35ma_device__on_notification_unlockedP9ma_device"] = a0 => (__Z35ma_device__on_notification_unlockedP9ma_device = Module["__Z35ma_device__on_notification_unlockedP9ma_device"] = wasmExports["Ba"])(a0);
var _ma_malloc_emscripten = Module["_ma_malloc_emscripten"] = (a0, a1) => (_ma_malloc_emscripten = Module["_ma_malloc_emscripten"] = wasmExports["Ca"])(a0, a1);
var _ma_free_emscripten = Module["_ma_free_emscripten"] = (a0, a1) => (_ma_free_emscripten = Module["_ma_free_emscripten"] = wasmExports["Da"])(a0, a1);
var _ma_device_process_pcm_frames_capture__webaudio = Module["_ma_device_process_pcm_frames_capture__webaudio"] = (a0, a1, a2) => (_ma_device_process_pcm_frames_capture__webaudio = Module["_ma_device_process_pcm_frames_capture__webaudio"] = wasmExports["Ea"])(a0, a1, a2);
var _ma_device_process_pcm_frames_playback__webaudio = Module["_ma_device_process_pcm_frames_playback__webaudio"] = (a0, a1, a2) => (_ma_device_process_pcm_frames_playback__webaudio = Module["_ma_device_process_pcm_frames_playback__webaudio"] = wasmExports["Fa"])(a0, a1, a2);
var _main = Module["_main"] = (a0, a1) => (_main = Module["_main"] = wasmExports["Ga"])(a0, a1);
var ___cxa_is_pointer_type = a0 => (___cxa_is_pointer_type = wasmExports["Ha"])(a0);
var calledRun;
dependenciesFulfilled = function runCaller() {
  if (!calledRun) run();
  if (!calledRun) dependenciesFulfilled = runCaller;
};
function callMain() {
  var entryFunction = _main;
  var argc = 0;
  var argv = 0;
  try {
    var ret = entryFunction(argc, argv);
    exitJS(ret, true);
    return ret;
  } catch (e) {
    return handleException(e);
  }
}
function run() {
  if (runDependencies > 0) {
    return;
  }
  preRun();
  if (runDependencies > 0) {
    return;
  }
  function doRun() {
    if (calledRun) return;
    calledRun = true;
    Module["calledRun"] = true;
    if (ABORT) return;
    initRuntime();
    preMain();
    if (Module["onRuntimeInitialized"]) Module["onRuntimeInitialized"]();
    if (shouldRunNow) callMain();
    postRun();
  }
  if (Module["setStatus"]) {
    Module["setStatus"]("Running...");
    setTimeout(function () {
      setTimeout(function () {
        Module["setStatus"]("");
      }, 1);
      doRun();
    }, 1);
  } else {
    doRun();
  }
}
if (Module["preInit"]) {
  if (typeof Module["preInit"] == "function") Module["preInit"] = [Module["preInit"]];
  while (Module["preInit"].length > 0) {
    Module["preInit"].pop()();
  }
}
var shouldRunNow = true;
if (Module["noInitialRun"]) shouldRunNow = false;
run();
