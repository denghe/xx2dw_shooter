var g;
g || (g = typeof Module !== 'undefined' ? Module : {});
var aa = Object.assign({}, g),
  ea = (a, b) => {
    throw b;
  },
  l = "";
"undefined" != typeof document && document.currentScript && (l = document.currentScript.src);
l = l.startsWith("blob:") ? "" : l.substr(0, l.replace(/[?#].*/, "").lastIndexOf("/") + 1);
var fa = g.print || console.log.bind(console),
  n = g.printErr || console.error.bind(console);
Object.assign(g, aa);
aa = null;
g.quit && (ea = g.quit);
var q;
g.wasmBinary && (q = g.wasmBinary);
"object" != typeof WebAssembly && ha("no native wasm support detected");
var ia,
  ja = !1,
  w,
  ka,
  y,
  z,
  la,
  A,
  C,
  ma,
  na;
function oa() {
  var a = ia.buffer;
  g.HEAP8 = ka = new Int8Array(a);
  g.HEAP16 = z = new Int16Array(a);
  g.HEAPU8 = y = new Uint8Array(a);
  g.HEAPU16 = la = new Uint16Array(a);
  g.HEAP32 = A = new Int32Array(a);
  g.HEAPU32 = C = new Uint32Array(a);
  g.HEAPF32 = ma = new Float32Array(a);
  g.HEAPF64 = na = new Float64Array(a);
}
var pa = [],
  qa = [],
  ra = [],
  sa = [];
function ta() {
  var a = g.preRun.shift();
  pa.unshift(a);
}
var D = 0,
  ua = null,
  F = null;
function va() {
  var _g$monitorRunDependen, _g;
  D++;
  (_g$monitorRunDependen = (_g = g).monitorRunDependencies) === null || _g$monitorRunDependen === void 0 || _g$monitorRunDependen.call(_g, D);
}
function wa() {
  var _g$monitorRunDependen2, _g2;
  D--;
  (_g$monitorRunDependen2 = (_g2 = g).monitorRunDependencies) === null || _g$monitorRunDependen2 === void 0 || _g$monitorRunDependen2.call(_g2, D);
  if (0 == D && (null !== ua && (clearInterval(ua), ua = null), F)) {
    var a = F;
    F = null;
    a();
  }
}
function ha(a) {
  var _g$onAbort, _g3;
  (_g$onAbort = (_g3 = g).onAbort) === null || _g$onAbort === void 0 || _g$onAbort.call(_g3, a);
  a = "Aborted(" + a + ")";
  n(a);
  ja = !0;
  w = 1;
  throw new WebAssembly.RuntimeError(a + ". Build with -sASSERTIONS for more info.");
}
var xa = a => a.startsWith("data:application/octet-stream;base64,"),
  G;
G = "s1.wasm";
if (!xa(G)) {
  var ya = G;
  G = g.locateFile ? g.locateFile(ya, l) : l + ya;
}
function za(a) {
  if (a == G && q) return new Uint8Array(q);
  throw "both async and sync fetching of the wasm failed";
}
function Aa(a) {
  return q || "function" != typeof fetch ? Promise.resolve().then(() => za(a)) : fetch(a, {
    credentials: "same-origin"
  }).then(b => {
    if (!b.ok) throw `failed to load wasm binary file at '${a}'`;
    return b.arrayBuffer();
  }).catch(() => za(a));
}
function Ba(a, b, c) {
  return Aa(a).then(d => WebAssembly.instantiate(d, b)).then(c, d => {
    n(`failed to asynchronously prepare wasm: ${d}`);
    ha(d);
  });
}
function Ca(a, b) {
  var c = G;
  q || "function" != typeof WebAssembly.instantiateStreaming || xa(c) || "function" != typeof fetch ? Ba(c, a, b) : fetch(c, {
    credentials: "same-origin"
  }).then(d => WebAssembly.instantiateStreaming(d, a).then(b, function (f) {
    n(`wasm streaming compile failed: ${f}`);
    n("falling back to ArrayBuffer instantiation");
    return Ba(c, a, b);
  }));
}
function Da(a) {
  this.name = "ExitStatus";
  this.message = `Program terminated with exit(${a})`;
  this.status = a;
}
var Ea = a => {
    for (; 0 < a.length;) a.shift()(g);
  },
  Fa = g.noExitRuntime || !0,
  Ga = new TextDecoder("utf8"),
  H = (a, b) => {
    if (!a) return "";
    b = a + b;
    for (var c = a; !(c >= b) && y[c];) ++c;
    return Ga.decode(y.subarray(a, c));
  };
class Ha {
  constructor(a) {
    this.za = a - 24;
  }
}
var Ia = 0,
  Ja = 0,
  N,
  Ka = 0,
  La = [];
function Ma() {
  if (navigator.userActivation ? navigator.userActivation.isActive : Ka && Na.La) for (var a = 0; a < La.length; ++a) {
    var b = La[a];
    La.splice(a, 1);
    --a;
    b.cb(...b.Xa);
  }
}
var O = [];
function Oa(a) {
  var b = O[a];
  b.target.removeEventListener(b.Aa, b.Na, b.Fa);
  O.splice(a, 1);
}
function Pa(a) {
  if (!a.target) return -4;
  if (a.Ga) a.Na = function (c) {
    ++Ka;
    Na = a;
    Ma();
    a.Ia(c);
    Ma();
    --Ka;
  }, a.target.addEventListener(a.Aa, a.Na, a.Fa), O.push(a);else for (var b = 0; b < O.length; ++b) O[b].target == a.target && O[b].Aa == a.Aa && Oa(b--);
  return 0;
}
var Na,
  Qa,
  Ra,
  Sa,
  Ta = [0, document, window],
  P = a => {
    a = 2 < a ? H(a) : a;
    return Ta[a] || document.querySelector(a);
  },
  Q = (a, b, c) => {
    var d = y;
    if (!(0 < c)) return 0;
    var f = b;
    c = b + c - 1;
    for (var m = 0; m < a.length; ++m) {
      var e = a.charCodeAt(m);
      if (55296 <= e && 57343 >= e) {
        var h = a.charCodeAt(++m);
        e = 65536 + ((e & 1023) << 10) | h & 1023;
      }
      if (127 >= e) {
        if (b >= c) break;
        d[b++] = e;
      } else {
        if (2047 >= e) {
          if (b + 1 >= c) break;
          d[b++] = 192 | e >> 6;
        } else {
          if (65535 >= e) {
            if (b + 2 >= c) break;
            d[b++] = 224 | e >> 12;
          } else {
            if (b + 3 >= c) break;
            d[b++] = 240 | e >> 18;
            d[b++] = 128 | e >> 12 & 63;
          }
          d[b++] = 128 | e >> 6 & 63;
        }
        d[b++] = 128 | e & 63;
      }
    }
    d[b] = 0;
    return b - f;
  },
  Ua = (a, b, c, d, f, m) => {
    Qa || (Qa = S(176));
    a = {
      target: P(a),
      Aa: m,
      Ga: d,
      Ia: e => {
        var h = Qa;
        na[h >> 3] = e.timeStamp;
        var k = h >> 2;
        A[k + 2] = e.location;
        A[k + 3] = e.ctrlKey;
        A[k + 4] = e.shiftKey;
        A[k + 5] = e.altKey;
        A[k + 6] = e.metaKey;
        A[k + 7] = e.repeat;
        A[k + 8] = e.charCode;
        A[k + 9] = e.keyCode;
        A[k + 10] = e.which;
        Q(e.key || "", h + 44, 32);
        Q(e.code || "", h + 76, 32);
        Q(e.char || "", h + 108, 32);
        Q(e.locale || "", h + 140, 32);
        N.get(d)(f, h, b) && e.preventDefault();
      },
      Fa: c
    };
    return Pa(a);
  },
  Va = (a, b, c, d, f, m) => {
    Ra || (Ra = S(72));
    a = P(a);
    return Pa({
      target: a,
      La: "mousemove" != m && "mouseenter" != m && "mouseleave" != m,
      Aa: m,
      Ga: d,
      Ia: function () {
        let e = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : event;
        var h = a,
          k = Ra;
        na[k >> 3] = e.timeStamp;
        k >>= 2;
        A[k + 2] = e.screenX;
        A[k + 3] = e.screenY;
        A[k + 4] = e.clientX;
        A[k + 5] = e.clientY;
        A[k + 6] = e.ctrlKey;
        A[k + 7] = e.shiftKey;
        A[k + 8] = e.altKey;
        A[k + 9] = e.metaKey;
        z[2 * k + 20] = e.button;
        z[2 * k + 21] = e.buttons;
        A[k + 11] = e.movementX;
        A[k + 12] = e.movementY;
        h = 0 > Ta.indexOf(h) ? h.getBoundingClientRect() : {
          left: 0,
          top: 0
        };
        A[k + 13] = e.clientX - h.left;
        A[k + 14] = e.clientY - h.top;
        N.get(d)(f, Ra, b) && e.preventDefault();
      },
      Fa: c
    });
  },
  Wa = (a, b, c, d, f, m) => {
    Sa || (Sa = S(1696));
    a = P(a);
    return Pa({
      target: a,
      La: "touchstart" == m || "touchend" == m,
      Aa: m,
      Ga: d,
      Ia: e => {
        for (var h, k = {}, r = e.touches, v = 0; v < r.length; ++v) h = r[v], h.Oa = h.Pa = 0, k[h.identifier] = h;
        for (v = 0; v < e.changedTouches.length; ++v) h = e.changedTouches[v], h.Oa = 1, k[h.identifier] = h;
        for (v = 0; v < e.targetTouches.length; ++v) k[e.targetTouches[v].identifier].Pa = 1;
        r = Sa;
        na[r >> 3] = e.timeStamp;
        var t = r >> 2;
        A[t + 3] = e.ctrlKey;
        A[t + 4] = e.shiftKey;
        A[t + 5] = e.altKey;
        A[t + 6] = e.metaKey;
        t += 7;
        var J = 0 > Ta.indexOf(a) ? a.getBoundingClientRect() : {
            left: 0,
            top: 0
          },
          E = 0;
        for (v in k) if (h = k[v], A[t] = h.identifier, A[t + 1] = h.screenX, A[t + 2] = h.screenY, A[t + 3] = h.clientX, A[t + 4] = h.clientY, A[t + 5] = h.pageX, A[t + 6] = h.pageY, A[t + 7] = h.Oa, A[t + 8] = h.Pa, A[t + 9] = h.clientX - J.left, A[t + 10] = h.clientY - J.top, t += 13, 31 < ++E) break;
        A[r + 8 >> 2] = E;
        N.get(d)(f, r, b) && e.preventDefault();
      },
      Fa: c
    });
  };
function Xa(a) {
  var b = T,
    c = b.Ka.pop() || b.za.length;
  b.za[c] = a;
  return c;
}
class Ya {
  constructor() {
    this.za = [void 0];
    this.Ka = [];
  }
  get(a) {
    return this.za[a];
  }
  has(a) {
    return void 0 !== this.za[a];
  }
}
var T, Za;
function $a(a, b, c, d, f) {
  function m() {
    var x = 0,
      B = 0;
    p.response && M && 0 === C[a + 12 >> 2] && (B = p.response.byteLength);
    0 < B && (x = S(B), y.set(new Uint8Array(p.response), x));
    C[a + 12 >> 2] = x;
    U(a + 16, B);
    U(a + 24, 0);
    (x = p.response ? p.response.byteLength : 0) && U(a + 32, x);
    z[a + 40 >> 1] = p.readyState;
    z[a + 42 >> 1] = p.status;
    p.statusText && Q(p.statusText, a + 44, 64);
  }
  var e = C[a + 8 >> 2];
  if (e) {
    var h = H(e),
      k = a + 112,
      r = H(k + 0);
    r || (r = "GET");
    var v = C[k + 56 >> 2],
      t = C[k + 68 >> 2],
      J = C[k + 72 >> 2];
    e = C[k + 76 >> 2];
    var E = C[k + 80 >> 2],
      R = C[k + 84 >> 2],
      K = C[k + 88 >> 2],
      I = C[k + 52 >> 2],
      M = !!(I & 1),
      ba = !!(I & 2);
    I = !!(I & 64);
    t = t ? H(t) : void 0;
    J = J ? H(J) : void 0;
    var p = new XMLHttpRequest();
    p.withCredentials = !!y[k + 60];
    p.open(r, h, !I, t, J);
    I || (p.timeout = v);
    p.za = h;
    p.responseType = "arraybuffer";
    E && (h = H(E), p.overrideMimeType(h));
    if (e) for (;;) {
      k = C[e >> 2];
      if (!k) break;
      h = C[e + 4 >> 2];
      if (!h) break;
      e += 8;
      k = H(k);
      h = H(h);
      p.setRequestHeader(k, h);
    }
    var L = Xa(p);
    C[a >> 2] = L;
    e = R && K ? y.slice(R, R + K) : null;
    p.onload = x => {
      T.has(L) && (m(), 200 <= p.status && 300 > p.status ? b === null || b === void 0 ? void 0 : b(a, p, x) : c === null || c === void 0 ? void 0 : c(a, p, x));
    };
    p.onerror = x => {
      T.has(L) && (m(), c === null || c === void 0 ? void 0 : c(a, p, x));
    };
    p.ontimeout = x => {
      T.has(L) && (c === null || c === void 0 ? void 0 : c(a, p, x));
    };
    p.onprogress = x => {
      if (T.has(L)) {
        var B = M && ba && p.response ? p.response.byteLength : 0,
          u = 0;
        0 < B && M && ba && (u = S(B), y.set(new Uint8Array(p.response), u));
        C[a + 12 >> 2] = u;
        U(a + 16, B);
        U(a + 24, x.loaded - B);
        U(a + 32, x.total);
        z[a + 40 >> 1] = p.readyState;
        3 <= p.readyState && 0 === p.status && 0 < x.loaded && (p.status = 200);
        z[a + 42 >> 1] = p.status;
        p.statusText && Q(p.statusText, a + 44, 64);
        d === null || d === void 0 || d(a, p, x);
        u && ab(u);
      }
    };
    p.onreadystatechange = x => {
      T.has(L) && (z[a + 40 >> 1] = p.readyState, 2 <= p.readyState && (z[a + 42 >> 1] = p.status), f === null || f === void 0 ? void 0 : f(a, p, x));
    };
    try {
      p.send(e);
    } catch (x) {
      c === null || c === void 0 || c(a, p, x);
    }
  } else c(a, 0, "no url specified!");
}
var bb = a => {
    a instanceof Da || "unwind" == a || ea(1, a);
  },
  cb = a => {
    var _g$onExit, _g4;
    w = a;
    Fa || ((_g$onExit = (_g4 = g).onExit) !== null && _g$onExit !== void 0 && _g$onExit.call(_g4, a), ja = !0);
    ea(a, new Da(a));
  },
  U = (a, b) => {
    C[a >> 2] = b;
    C[a + 4 >> 2] = (b - C[a >> 2]) / 4294967296;
  };
function db(a, b, c, d) {
  var f = Za;
  if (f) {
    var m = C[a + 112 + 64 >> 2];
    m || (m = C[a + 8 >> 2]);
    var e = H(m);
    try {
      var h = f.transaction(["FILES"], "readwrite").objectStore("FILES").put(b, e);
      h.onsuccess = () => {
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 200;
        Q("OK", a + 44, 64);
        c(a, 0, e);
      };
      h.onerror = k => {
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 413;
        Q("Payload Too Large", a + 44, 64);
        d(a, 0, k);
      };
    } catch (k) {
      d(a, 0, k);
    }
  } else d(a, 0, "IndexedDB not available!");
}
function eb(a, b, c) {
  var d = Za;
  if (d) {
    var f = C[a + 112 + 64 >> 2];
    f || (f = C[a + 8 >> 2]);
    f = H(f);
    try {
      var m = d.transaction(["FILES"], "readonly").objectStore("FILES").get(f);
      m.onsuccess = e => {
        if (e.target.result) {
          e = e.target.result;
          var h = e.byteLength || e.length,
            k = S(h);
          y.set(new Uint8Array(e), k);
          C[a + 12 >> 2] = k;
          U(a + 16, h);
          U(a + 24, 0);
          U(a + 32, h);
          z[a + 40 >> 1] = 4;
          z[a + 42 >> 1] = 200;
          Q("OK", a + 44, 64);
          b(a, 0, e);
        } else z[a + 40 >> 1] = 4, z[a + 42 >> 1] = 404, Q("Not Found", a + 44, 64), c(a, 0, "no data");
      };
      m.onerror = e => {
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 404;
        Q("Not Found", a + 44, 64);
        c(a, 0, e);
      };
    } catch (e) {
      c(a, 0, e);
    }
  } else c(a, 0, "IndexedDB not available!");
}
function fb(a, b, c) {
  var d = Za;
  if (d) {
    var f = C[a + 112 + 64 >> 2];
    f || (f = C[a + 8 >> 2]);
    f = H(f);
    try {
      var m = d.transaction(["FILES"], "readwrite").objectStore("FILES").delete(f);
      m.onsuccess = e => {
        e = e.target.result;
        C[a + 12 >> 2] = 0;
        U(a + 16, 0);
        U(a + 24, 0);
        U(a + 32, 0);
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 200;
        Q("OK", a + 44, 64);
        b(a, 0, e);
      };
      m.onerror = e => {
        z[a + 40 >> 1] = 4;
        z[a + 42 >> 1] = 404;
        Q("Not Found", a + 44, 64);
        c(a, 0, e);
      };
    } catch (e) {
      c(a, 0, e);
    }
  } else c(a, 0, "IndexedDB not available!");
}
var gb = 1,
  hb = [],
  V = [],
  ib = [],
  W = [],
  X = [],
  jb = [],
  kb = [],
  lb = a => {
    for (var b = gb++, c = a.length; c < b; c++) a[c] = null;
    return b;
  },
  mb = (a, b, c, d) => {
    for (var f = 0; f < a; f++) {
      var m = Y[c](),
        e = m && lb(d);
      m && (m.name = e, d[e] = m);
      A[b + 4 * f >> 2] = e;
    }
  },
  ob = (a, b) => {
    a.za || (a.za = a.getContext, a.getContext = function (d, f) {
      f = a.za(d, f);
      return "webgl" == d == f instanceof WebGLRenderingContext ? f : null;
    });
    var c = a.getContext("webgl2", b);
    return c ? nb(c, b) : 0;
  },
  nb = (a, b) => {
    var c = lb(kb);
    b = {
      handle: c,
      attributes: b,
      version: b.Va,
      Sa: a
    };
    a.canvas && (a.canvas.Wa = b);
    kb[c] = b;
    return c;
  },
  pb,
  qb = ["default", "low-power", "high-performance"],
  rb = [null, [], []],
  sb = a => "]" == a.slice(-1) && a.lastIndexOf("["),
  tb = a => {
    var b = Y.Ta,
      c = b.Ba[a];
    "number" == typeof c && (b.Ba[a] = c = Y.getUniformLocation(b, b.Qa[a] + (0 < c ? `[${c}]` : "")));
    return c;
  };
T = new Ya();
va();
(function (a, b) {
  try {
    var c = indexedDB.open("emscripten_filesystem", 1);
  } catch (d) {
    b(d);
    return;
  }
  c.onupgradeneeded = d => {
    d = d.target.result;
    d.objectStoreNames.contains("FILES") && d.deleteObjectStore("FILES");
    d.createObjectStore("FILES");
  };
  c.onsuccess = d => a(d.target.result);
  c.onerror = b;
})(a => {
  Za = a;
  wa();
}, () => {
  Za = !1;
  wa();
});
var Y,
  ub = {
    c: (a, b, c, d) => {
      ha(`Assertion failed: ${H(a)}, at: ` + [b ? H(b) : "unknown filename", c, d ? H(d) : "unknown function"]);
    },
    u: (a, b, c) => {
      var d = new Ha(a);
      C[d.za + 16 >> 2] = 0;
      C[d.za + 4 >> 2] = b;
      C[d.za + 8 >> 2] = c;
      Ia = a;
      Ja++;
      throw Ia;
    },
    O: function (a) {
      if (T.has(a)) {
        var b = T.get(a),
          c = T;
        c.za[a] = void 0;
        c.Ka.push(a);
        0 < b.readyState && 4 > b.readyState && b.abort();
      }
    },
    K: () => 1,
    r: () => {
      ha("");
    },
    L: () => Date.now(),
    N: () => performance.now(),
    Q: () => !0,
    pa: (a, b) => {
      function c(d) {
        N.get(a)(d, b) && requestAnimationFrame(c);
      }
      return requestAnimationFrame(c);
    },
    M: a => {
      var b = y.length;
      a >>>= 0;
      if (2147483648 < a) return !1;
      for (var c = 1; 4 >= c; c *= 2) {
        var d = b * (1 + .2 / c);
        d = Math.min(d, a + 100663296);
        var f = Math;
        d = Math.max(a, d);
        a: {
          f = (f.min.call(f, 2147483648, d + (65536 - d % 65536) % 65536) - ia.buffer.byteLength + 65535) / 65536;
          try {
            ia.grow(f);
            oa();
            var m = 1;
            break a;
          } catch (e) {}
          m = void 0;
        }
        if (m) return !0;
      }
      return !1;
    },
    fa: (a, b, c) => {
      a = P(a);
      if (!a) return -4;
      a.width = b;
      a.height = c;
      return 0;
    },
    oa: (a, b, c, d) => Ua(a, b, c, d, 2, "keydown"),
    na: (a, b, c, d) => Ua(a, b, c, d, 3, "keyup"),
    ma: (a, b, c, d) => Va(a, b, c, d, 5, "mousedown"),
    ka: (a, b, c, d) => Va(a, b, c, d, 8, "mousemove"),
    la: (a, b, c, d) => Va(a, b, c, d, 6, "mouseup"),
    ga: (a, b, c, d) => Wa(a, b, c, d, 25, "touchcancel"),
    ha: (a, b, c, d) => Wa(a, b, c, d, 23, "touchend"),
    ia: (a, b, c, d) => Wa(a, b, c, d, 24, "touchmove"),
    ja: (a, b, c, d) => Wa(a, b, c, d, 22, "touchstart"),
    P: function (a, b, c, d, f) {
      function m(u) {
        if (J) u();else if (!ja) try {
          if (u(), !Fa) try {
            w = u = w, cb(u);
          } catch (ca) {
            bb(ca);
          }
        } catch (ca) {
          bb(ca);
        }
      }
      var e = a + 112,
        h = C[e + 36 >> 2],
        k = C[e + 40 >> 2],
        r = C[e + 44 >> 2],
        v = C[e + 48 >> 2],
        t = C[e + 52 >> 2],
        J = !!(t & 64),
        E = u => {
          m(() => {
            h ? N.get(h)(u) : b === null || b === void 0 ? void 0 : b(u);
          });
        },
        R = u => {
          m(() => {
            r ? N.get(r)(u) : d === null || d === void 0 ? void 0 : d(u);
          });
        },
        K = u => {
          m(() => {
            k ? N.get(k)(u) : c === null || c === void 0 ? void 0 : c(u);
          });
        },
        I = u => {
          m(() => {
            v ? N.get(v)(u) : f === null || f === void 0 ? void 0 : f(u);
          });
        },
        M = u => {
          $a(u, E, K, R, I);
        },
        ba = (u, ca) => {
          db(u, ca.response, da => {
            m(() => {
              h ? N.get(h)(da) : b === null || b === void 0 ? void 0 : b(da);
            });
          }, da => {
            m(() => {
              h ? N.get(h)(da) : b === null || b === void 0 ? void 0 : b(da);
            });
          });
        },
        p = u => {
          $a(u, ba, K, R, I);
        },
        L = H(e + 0),
        x = !!(t & 16),
        B = !!(t & 4);
      t = !!(t & 32);
      if ("EM_IDB_STORE" === L) M = C[e + 84 >> 2], db(a, y.slice(M, M + C[e + 88 >> 2]), E, K);else if ("EM_IDB_DELETE" === L) fb(a, E, K);else if (x) {
        if (t) return 0;
        $a(a, B ? ba : E, K, R, I);
      } else eb(a, E, t ? K : B ? p : M);
      return a;
    },
    ea: (a, b) => {
      b >>= 2;
      b = {
        alpha: !!A[b],
        depth: !!A[b + 1],
        stencil: !!A[b + 2],
        antialias: !!A[b + 3],
        premultipliedAlpha: !!A[b + 4],
        preserveDrawingBuffer: !!A[b + 5],
        powerPreference: qb[A[b + 6]],
        failIfMajorPerformanceCaveat: !!A[b + 7],
        Va: A[b + 8],
        $a: A[b + 9],
        Za: A[b + 10],
        Ua: A[b + 11],
        ab: A[b + 12],
        bb: A[b + 13]
      };
      a = P(a);
      return !a || b.Ua ? 0 : ob(a, b);
    },
    da: a => {
      var _pb;
      pb = kb[a];
      g.Ya = Y = (_pb = pb) === null || _pb === void 0 ? void 0 : _pb.Sa;
      return !a || Y ? 0 : -5;
    },
    w: (a, b, c, d) => {
      for (var f = 0, m = 0; m < c; m++) {
        var e = C[b >> 2],
          h = C[b + 4 >> 2];
        b += 8;
        for (var k = 0; k < h; k++) {
          var r = y[e + k],
            v = rb[a];
          if (0 === r || 10 === r) {
            for (r = 0; v[r] && !(NaN <= r);) ++r;
            r = Ga.decode(v.buffer ? v.subarray(0, r) : new Uint8Array(v.slice(0, r)));
            (1 === a ? fa : n)(r);
            v.length = 0;
          } else v.push(r);
        }
        f += h;
      }
      C[d >> 2] = f;
      return 0;
    },
    ba: a => Y.activeTexture(a),
    A: (a, b) => {
      Y.attachShader(V[a], X[b]);
    },
    d: (a, b) => {
      35051 == a ? Y.Ma = b : 35052 == a && (Y.Ha = b);
      Y.bindBuffer(a, hb[b]);
    },
    J: (a, b) => {
      Y.bindFramebuffer(a, ib[b]);
    },
    b: (a, b) => {
      Y.bindTexture(a, W[b]);
    },
    i: a => {
      Y.bindVertexArray(jb[a]);
    },
    p: a => Y.blendEquation(a),
    q: (a, b) => Y.blendFunc(a, b),
    k: (a, b, c, d) => {
      c && b ? Y.bufferData(a, y, d, c, b) : Y.bufferData(a, b, d);
    },
    H: a => Y.clear(a),
    I: (a, b, c, d) => Y.clearColor(a, b, c, d),
    U: a => {
      Y.compileShader(X[a]);
    },
    Z: () => {
      var a = lb(V),
        b = Y.createProgram();
      b.name = a;
      b.Ea = b.Ca = b.Da = 0;
      b.Ja = 1;
      V[a] = b;
      return a;
    },
    W: a => {
      var b = lb(X);
      X[b] = Y.createShader(a);
      return b;
    },
    o: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = A[b + 4 * c >> 2],
          f = hb[d];
        f && (Y.deleteBuffer(f), f.name = 0, hb[d] = null, d == Y.Ma && (Y.Ma = 0), d == Y.Ha && (Y.Ha = 0));
      }
    },
    x: (a, b) => {
      for (var c = 0; c < a; ++c) {
        var d = A[b + 4 * c >> 2],
          f = ib[d];
        f && (Y.deleteFramebuffer(f), f.name = 0, ib[d] = null);
      }
    },
    j: a => {
      if (a) {
        var b = V[a];
        b && (Y.deleteProgram(b), b.name = 0, V[a] = null);
      }
    },
    e: a => {
      if (a) {
        var b = X[a];
        b && (Y.deleteShader(b), X[a] = null);
      }
    },
    sa: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = A[b + 4 * c >> 2],
          f = W[d];
        f && (Y.deleteTexture(f), f.name = 0, W[d] = null);
      }
    },
    E: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = A[b + 4 * c >> 2];
        Y.deleteVertexArray(jb[d]);
        jb[d] = null;
      }
    },
    F: a => Y.disable(a),
    G: (a, b, c, d) => {
      Y.drawArraysInstanced(a, b, c, d);
    },
    _: (a, b, c, d) => {
      Y.drawElements(a, b, c, d);
    },
    ca: a => Y.enable(a),
    f: a => {
      Y.enableVertexAttribArray(a);
    },
    ra: (a, b, c, d, f) => {
      Y.framebufferTexture2D(a, b, c, W[d], f);
    },
    n: (a, b) => {
      mb(a, b, "createBuffer", hb);
    },
    S: (a, b) => {
      mb(a, b, "createFramebuffer", ib);
    },
    m: (a, b) => {
      mb(a, b, "createTexture", W);
    },
    B: (a, b) => {
      mb(a, b, "createVertexArray", jb);
    },
    h: (a, b) => Y.getAttribLocation(V[a], H(b)),
    X: (a, b, c, d) => {
      a = Y.getProgramInfoLog(V[a]);
      b = 0 < b && d ? Q(a, d, b) : 0;
      c && (A[c >> 2] = b);
    },
    z: (a, b, c) => {
      if (c && !(a >= gb)) if (a = V[a], 35716 == b) A[c >> 2] = Y.getProgramInfoLog(a).length + 1;else if (35719 == b) {
        if (!a.Ea) for (b = 0; b < Y.getProgramParameter(a, 35718); ++b) a.Ea = Math.max(a.Ea, Y.getActiveUniform(a, b).name.length + 1);
        A[c >> 2] = a.Ea;
      } else if (35722 == b) {
        if (!a.Ca) for (b = 0; b < Y.getProgramParameter(a, 35721); ++b) a.Ca = Math.max(a.Ca, Y.getActiveAttrib(a, b).name.length + 1);
        A[c >> 2] = a.Ca;
      } else if (35381 == b) {
        if (!a.Da) for (b = 0; b < Y.getProgramParameter(a, 35382); ++b) a.Da = Math.max(a.Da, Y.getActiveUniformBlockName(a, b).length + 1);
        A[c >> 2] = a.Da;
      } else A[c >> 2] = Y.getProgramParameter(a, b);
    },
    T: (a, b, c, d) => {
      a = Y.getShaderInfoLog(X[a]);
      b = 0 < b && d ? Q(a, d, b) : 0;
      c && (A[c >> 2] = b);
    },
    y: (a, b, c) => {
      c && (35716 == b ? (a = Y.getShaderInfoLog(X[a]), A[c >> 2] = a ? a.length + 1 : 0) : 35720 == b ? (a = Y.getShaderSource(X[a]), A[c >> 2] = a ? a.length + 1 : 0) : A[c >> 2] = Y.getShaderParameter(X[a], b));
    },
    s: (a, b) => {
      b = H(b);
      if (a = V[a]) {
        var c = a,
          d = c.Ba,
          f = c.Ra,
          m;
        if (!d) for (c.Ba = d = {}, c.Qa = {}, m = 0; m < Y.getProgramParameter(c, 35718); ++m) {
          var e = Y.getActiveUniform(c, m);
          var h = e.name;
          e = e.size;
          var k = sb(h);
          k = 0 < k ? h.slice(0, k) : h;
          var r = c.Ja;
          c.Ja += e;
          f[k] = [e, r];
          for (h = 0; h < e; ++h) d[r] = h, c.Qa[r++] = k;
        }
        c = a.Ba;
        d = 0;
        f = b;
        m = sb(b);
        0 < m && (d = parseInt(b.slice(m + 1)) >>> 0, f = b.slice(0, m));
        if ((f = a.Ra[f]) && d < f[0] && (d += f[1], c[d] = c[d] || Y.getUniformLocation(a, b))) return d;
      }
      return -1;
    },
    Y: a => {
      a = V[a];
      Y.linkProgram(a);
      a.Ba = 0;
      a.Ra = {};
    },
    V: (a, b, c, d) => {
      for (var f = "", m = 0; m < b; ++m) f += H(C[c + 4 * m >> 2], d ? C[d + 4 * m >> 2] : void 0);
      Y.shaderSource(X[a], f);
    },
    v: (a, b, c, d, f, m, e, h, k) => {
      if (Y.Ha) Y.texImage2D(a, b, c, d, f, m, e, h, k);else if (k) {
        var r = h - 5120;
        r = 0 == r ? ka : 1 == r ? y : 2 == r ? z : 4 == r ? A : 6 == r ? ma : 5 == r || 28922 == r || 28520 == r || 30779 == r || 30782 == r ? C : la;
        Y.texImage2D(a, b, c, d, f, m, e, h, r, k >>> 31 - Math.clz32(r.BYTES_PER_ELEMENT));
      } else Y.texImage2D(a, b, c, d, f, m, e, h, null);
    },
    a: (a, b, c) => Y.texParameteri(a, b, c),
    $: (a, b) => {
      Y.uniform1i(tb(a), b);
    },
    C: (a, b, c) => {
      Y.uniform2f(tb(a), b, c);
    },
    D: a => {
      a = V[a];
      Y.useProgram(a);
      Y.Ta = a;
    },
    l: (a, b) => {
      Y.vertexAttribDivisor(a, b);
    },
    g: (a, b, c, d, f, m) => {
      Y.vertexAttribPointer(a, b, c, !!d, f, m);
    },
    t: (a, b, c, d) => Y.viewport(a, b, c, d),
    R: function (a, b, c, d) {
      var f = document.createElement("canvas");
      f.width = b;
      f.height = c;
      b = f.getContext("2d");
      b.font = a + "px " + H(d);
      b.textBaseline = "middle";
      b.globalAlpha = 1;
      b.fillStyle = "white";
      window["gCanvas" + a] = f;
      window["gCanvasCtx" + a] = b;
    },
    qa: function (a, b, c, d) {
      var f = new Image();
      f.onload = () => {
        C[c >> 2] = f.width;
        C[d >> 2] = f.height;
        Y.bindTexture(Y.TEXTURE_2D, W[a]);
        Y.texImage2D(Y.TEXTURE_2D, 0, Y.RGBA, Y.RGBA, Y.UNSIGNED_BYTE, f);
      };
      f.src = H(b);
    },
    aa: function (a, b) {
      var c = window["gCanvas" + a];
      a = window["gCanvasCtx" + a];
      a.clearRect(0, 0, c.width, c.height);
      b = H(b);
      var d = a.measureText(b).width;
      a.fillText(b, 0, c.height / 2);
      Y.texImage2D(Y.TEXTURE_2D, 0, Y.RGBA, Y.RGBA, Y.UNSIGNED_BYTE, c);
      return d;
    }
  },
  Z = function () {
    function a(c) {
      Z = c.exports;
      ia = Z.ta;
      oa();
      N = Z.xa;
      qa.unshift(Z.ua);
      wa();
      return Z;
    }
    var b = {
      a: ub
    };
    va();
    if (g.instantiateWasm) try {
      return g.instantiateWasm(b, a);
    } catch (c) {
      return n(`Module.instantiateWasm callback failed with error: ${c}`), !1;
    }
    Ca(b, function (c) {
      a(c.instance);
    });
    return {};
  }(),
  S = a => (S = Z.va)(a),
  ab = a => (ab = Z.wa)(a),
  vb = g._main = (a, b) => (vb = g._main = Z.ya)(a, b),
  wb;
F = function xb() {
  wb || yb();
  wb || (F = xb);
};
function yb() {
  function a() {
    if (!wb && (wb = !0, g.calledRun = !0, !ja)) {
      Ea(qa);
      Ea(ra);
      if (g.onRuntimeInitialized) g.onRuntimeInitialized();
      if (zb) {
        var b = vb;
        try {
          var c = b(0, 0);
          w = c;
          cb(c);
        } catch (d) {
          bb(d);
        }
      }
      if (g.postRun) for ("function" == typeof g.postRun && (g.postRun = [g.postRun]); g.postRun.length;) b = g.postRun.shift(), sa.unshift(b);
      Ea(sa);
    }
  }
  if (!(0 < D)) {
    if (g.preRun) for ("function" == typeof g.preRun && (g.preRun = [g.preRun]); g.preRun.length;) ta();
    Ea(pa);
    0 < D || (g.setStatus ? (g.setStatus("Running..."), setTimeout(function () {
      setTimeout(function () {
        g.setStatus("");
      }, 1);
      a();
    }, 1)) : a());
  }
}
if (g.preInit) for ("function" == typeof g.preInit && (g.preInit = [g.preInit]); 0 < g.preInit.length;) g.preInit.pop()();
var zb = !0;
g.noInitialRun && (zb = !1);
yb();
