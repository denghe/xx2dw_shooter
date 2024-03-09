var g;
g || (g = typeof Module !== 'undefined' ? Module : {});
var da = Object.assign({}, g),
  ea = (a, b) => {
    throw b;
  },
  l = "";
"undefined" != typeof document && document.currentScript && (l = document.currentScript.src);
l = l.startsWith("blob:") ? "" : l.substr(0, l.replace(/[?#].*/, "").lastIndexOf("/") + 1);
var fa = g.print || console.log.bind(console),
  n = g.printErr || console.error.bind(console);
Object.assign(g, da);
da = null;
g.quit && (ea = g.quit);
var q;
g.wasmBinary && (q = g.wasmBinary);
"object" != typeof WebAssembly && ha("no native wasm support detected");
var ia,
  ja = !1,
  u,
  ka,
  w,
  x,
  la,
  y,
  A,
  ma,
  na;
function oa() {
  var a = ia.buffer;
  g.HEAP8 = ka = new Int8Array(a);
  g.HEAP16 = x = new Int16Array(a);
  g.HEAPU8 = w = new Uint8Array(a);
  g.HEAPU16 = la = new Uint16Array(a);
  g.HEAP32 = y = new Int32Array(a);
  g.HEAPU32 = A = new Uint32Array(a);
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
  E = null;
function va() {
  var _g$monitorRunDependen, _g;
  D++;
  (_g$monitorRunDependen = (_g = g).monitorRunDependencies) === null || _g$monitorRunDependen === void 0 || _g$monitorRunDependen.call(_g, D);
}
function wa() {
  var _g$monitorRunDependen2, _g2;
  D--;
  (_g$monitorRunDependen2 = (_g2 = g).monitorRunDependencies) === null || _g$monitorRunDependen2 === void 0 || _g$monitorRunDependen2.call(_g2, D);
  if (0 == D && (null !== ua && (clearInterval(ua), ua = null), E)) {
    var a = E;
    E = null;
    a();
  }
}
function ha(a) {
  var _g$onAbort, _g3;
  (_g$onAbort = (_g3 = g).onAbort) === null || _g$onAbort === void 0 || _g$onAbort.call(_g3, a);
  a = "Aborted(" + a + ")";
  n(a);
  ja = !0;
  u = 1;
  throw new WebAssembly.RuntimeError(a + ". Build with -sASSERTIONS for more info.");
}
var xa = a => a.startsWith("data:application/octet-stream;base64,"),
  F;
F = "s25.wasm";
if (!xa(F)) {
  var ya = F;
  F = g.locateFile ? g.locateFile(ya, l) : l + ya;
}
function za(a) {
  if (a == F && q) return new Uint8Array(q);
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
  var c = F;
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
  G = (a, b) => {
    if (!a) return "";
    b = a + b;
    for (var c = a; !(c >= b) && w[c];) ++c;
    return Ga.decode(w.subarray(a, c));
  };
class Ha {
  constructor(a) {
    this.va = a - 24;
  }
}
var Ia = 0,
  Ja = 0,
  M,
  Ka = 0,
  La = [];
function Ma() {
  if (navigator.userActivation ? navigator.userActivation.isActive : Ka && Na.Ma) for (var a = 0; a < La.length; ++a) {
    var b = La[a];
    La.splice(a, 1);
    --a;
    b.Xa(...b.Ra);
  }
}
var N = [];
function Oa(a) {
  var b = N[a];
  b.target.removeEventListener(b.wa, b.Ga, b.Da);
  N.splice(a, 1);
}
function Pa(a) {
  if (!a.target) return -4;
  if (a.Ea) a.Ga = function (c) {
    ++Ka;
    Na = a;
    Ma();
    a.Ha(c);
    Ma();
    --Ka;
  }, a.target.addEventListener(a.wa, a.Ga, a.Da), N.push(a);else for (var b = 0; b < N.length; ++b) N[b].target == a.target && N[b].wa == a.wa && Oa(b--);
  return 0;
}
var Na,
  Qa,
  Ra,
  Sa = [0, document, window],
  Ta = a => {
    a = 2 < a ? G(a) : a;
    return Sa[a] || document.querySelector(a);
  },
  O = (a, b, c) => {
    var d = w;
    if (!(0 < c)) return 0;
    var f = b;
    c = b + c - 1;
    for (var k = 0; k < a.length; ++k) {
      var e = a.charCodeAt(k);
      if (55296 <= e && 57343 >= e) {
        var m = a.charCodeAt(++k);
        e = 65536 + ((e & 1023) << 10) | m & 1023;
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
  Ua = (a, b, c, d, f, k) => {
    Qa || (Qa = Q(176));
    a = {
      target: Ta(a),
      wa: k,
      Ea: d,
      Ha: e => {
        var m = Qa;
        na[m >> 3] = e.timeStamp;
        var h = m >> 2;
        y[h + 2] = e.location;
        y[h + 3] = e.ctrlKey;
        y[h + 4] = e.shiftKey;
        y[h + 5] = e.altKey;
        y[h + 6] = e.metaKey;
        y[h + 7] = e.repeat;
        y[h + 8] = e.charCode;
        y[h + 9] = e.keyCode;
        y[h + 10] = e.which;
        O(e.key || "", m + 44, 32);
        O(e.code || "", m + 76, 32);
        O(e.char || "", m + 108, 32);
        O(e.locale || "", m + 140, 32);
        M.get(d)(f, m, b) && e.preventDefault();
      },
      Da: c
    };
    return Pa(a);
  },
  Va = (a, b, c, d, f, k) => {
    Ra || (Ra = Q(72));
    a = Ta(a);
    return Pa({
      target: a,
      Ma: "mousemove" != k && "mouseenter" != k && "mouseleave" != k,
      wa: k,
      Ea: d,
      Ha: function () {
        let e = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : event;
        var m = a,
          h = Ra;
        na[h >> 3] = e.timeStamp;
        h >>= 2;
        y[h + 2] = e.screenX;
        y[h + 3] = e.screenY;
        y[h + 4] = e.clientX;
        y[h + 5] = e.clientY;
        y[h + 6] = e.ctrlKey;
        y[h + 7] = e.shiftKey;
        y[h + 8] = e.altKey;
        y[h + 9] = e.metaKey;
        x[2 * h + 20] = e.button;
        x[2 * h + 21] = e.buttons;
        y[h + 11] = e.movementX;
        y[h + 12] = e.movementY;
        m = 0 > Sa.indexOf(m) ? m.getBoundingClientRect() : {
          left: 0,
          top: 0
        };
        y[h + 13] = e.clientX - m.left;
        y[h + 14] = e.clientY - m.top;
        M.get(d)(f, Ra, b) && e.preventDefault();
      },
      Da: c
    });
  };
function Wa(a) {
  var b = R,
    c = b.Ka.pop() || b.va.length;
  b.va[c] = a;
  return c;
}
class Xa {
  constructor() {
    this.va = [void 0];
    this.Ka = [];
  }
  get(a) {
    return this.va[a];
  }
  has(a) {
    return void 0 !== this.va[a];
  }
}
var R, S;
function Ya(a, b, c, d, f) {
  function k() {
    var v = 0,
      z = 0;
    p.response && K && 0 === A[a + 12 >> 2] && (z = p.response.byteLength);
    0 < z && (v = Q(z), w.set(new Uint8Array(p.response), v));
    A[a + 12 >> 2] = v;
    T(a + 16, z);
    T(a + 24, 0);
    (v = p.response ? p.response.byteLength : 0) && T(a + 32, v);
    x[a + 40 >> 1] = p.readyState;
    x[a + 42 >> 1] = p.status;
    p.statusText && O(p.statusText, a + 44, 64);
  }
  var e = A[a + 8 >> 2];
  if (e) {
    var m = G(e),
      h = a + 112,
      r = G(h + 0);
    r || (r = "GET");
    var C = A[h + 56 >> 2],
      B = A[h + 68 >> 2],
      U = A[h + 72 >> 2];
    e = A[h + 76 >> 2];
    var L = A[h + 80 >> 2],
      P = A[h + 84 >> 2],
      I = A[h + 88 >> 2],
      H = A[h + 52 >> 2],
      K = !!(H & 1),
      aa = !!(H & 2);
    H = !!(H & 64);
    B = B ? G(B) : void 0;
    U = U ? G(U) : void 0;
    var p = new XMLHttpRequest();
    p.withCredentials = !!w[h + 60];
    p.open(r, m, !H, B, U);
    H || (p.timeout = C);
    p.va = m;
    p.responseType = "arraybuffer";
    L && (m = G(L), p.overrideMimeType(m));
    if (e) for (;;) {
      h = A[e >> 2];
      if (!h) break;
      m = A[e + 4 >> 2];
      if (!m) break;
      e += 8;
      h = G(h);
      m = G(m);
      p.setRequestHeader(h, m);
    }
    var J = Wa(p);
    A[a >> 2] = J;
    e = P && I ? w.slice(P, P + I) : null;
    p.onload = v => {
      R.has(J) && (k(), 200 <= p.status && 300 > p.status ? b === null || b === void 0 ? void 0 : b(a, p, v) : c === null || c === void 0 ? void 0 : c(a, p, v));
    };
    p.onerror = v => {
      R.has(J) && (k(), c === null || c === void 0 ? void 0 : c(a, p, v));
    };
    p.ontimeout = v => {
      R.has(J) && (c === null || c === void 0 ? void 0 : c(a, p, v));
    };
    p.onprogress = v => {
      if (R.has(J)) {
        var z = K && aa && p.response ? p.response.byteLength : 0,
          t = 0;
        0 < z && K && aa && (t = Q(z), w.set(new Uint8Array(p.response), t));
        A[a + 12 >> 2] = t;
        T(a + 16, z);
        T(a + 24, v.loaded - z);
        T(a + 32, v.total);
        x[a + 40 >> 1] = p.readyState;
        3 <= p.readyState && 0 === p.status && 0 < v.loaded && (p.status = 200);
        x[a + 42 >> 1] = p.status;
        p.statusText && O(p.statusText, a + 44, 64);
        d === null || d === void 0 || d(a, p, v);
        t && Za(t);
      }
    };
    p.onreadystatechange = v => {
      R.has(J) && (x[a + 40 >> 1] = p.readyState, 2 <= p.readyState && (x[a + 42 >> 1] = p.status), f === null || f === void 0 ? void 0 : f(a, p, v));
    };
    try {
      p.send(e);
    } catch (v) {
      c === null || c === void 0 || c(a, p, v);
    }
  } else c(a, 0, "no url specified!");
}
var $a = a => {
    a instanceof Da || "unwind" == a || ea(1, a);
  },
  ab = a => {
    var _g$onExit, _g4;
    u = a;
    Fa || ((_g$onExit = (_g4 = g).onExit) !== null && _g$onExit !== void 0 && _g$onExit.call(_g4, a), ja = !0);
    ea(a, new Da(a));
  },
  T = (a, b) => {
    A[a >> 2] = b;
    A[a + 4 >> 2] = (b - A[a >> 2]) / 4294967296;
  };
function bb(a, b, c, d) {
  var f = S;
  if (f) {
    var k = A[a + 112 + 64 >> 2];
    k || (k = A[a + 8 >> 2]);
    var e = G(k);
    try {
      var m = f.transaction(["FILES"], "readwrite").objectStore("FILES").put(b, e);
      m.onsuccess = () => {
        x[a + 40 >> 1] = 4;
        x[a + 42 >> 1] = 200;
        O("OK", a + 44, 64);
        c(a, 0, e);
      };
      m.onerror = h => {
        x[a + 40 >> 1] = 4;
        x[a + 42 >> 1] = 413;
        O("Payload Too Large", a + 44, 64);
        d(a, 0, h);
      };
    } catch (h) {
      d(a, 0, h);
    }
  } else d(a, 0, "IndexedDB not available!");
}
function cb(a, b, c) {
  var d = S;
  if (d) {
    var f = A[a + 112 + 64 >> 2];
    f || (f = A[a + 8 >> 2]);
    f = G(f);
    try {
      var k = d.transaction(["FILES"], "readonly").objectStore("FILES").get(f);
      k.onsuccess = e => {
        if (e.target.result) {
          e = e.target.result;
          var m = e.byteLength || e.length,
            h = Q(m);
          w.set(new Uint8Array(e), h);
          A[a + 12 >> 2] = h;
          T(a + 16, m);
          T(a + 24, 0);
          T(a + 32, m);
          x[a + 40 >> 1] = 4;
          x[a + 42 >> 1] = 200;
          O("OK", a + 44, 64);
          b(a, 0, e);
        } else x[a + 40 >> 1] = 4, x[a + 42 >> 1] = 404, O("Not Found", a + 44, 64), c(a, 0, "no data");
      };
      k.onerror = e => {
        x[a + 40 >> 1] = 4;
        x[a + 42 >> 1] = 404;
        O("Not Found", a + 44, 64);
        c(a, 0, e);
      };
    } catch (e) {
      c(a, 0, e);
    }
  } else c(a, 0, "IndexedDB not available!");
}
function db(a, b, c) {
  var d = S;
  if (d) {
    var f = A[a + 112 + 64 >> 2];
    f || (f = A[a + 8 >> 2]);
    f = G(f);
    try {
      var k = d.transaction(["FILES"], "readwrite").objectStore("FILES").delete(f);
      k.onsuccess = e => {
        e = e.target.result;
        A[a + 12 >> 2] = 0;
        T(a + 16, 0);
        T(a + 24, 0);
        T(a + 32, 0);
        x[a + 40 >> 1] = 4;
        x[a + 42 >> 1] = 200;
        O("OK", a + 44, 64);
        b(a, 0, e);
      };
      k.onerror = e => {
        x[a + 40 >> 1] = 4;
        x[a + 42 >> 1] = 404;
        O("Not Found", a + 44, 64);
        c(a, 0, e);
      };
    } catch (e) {
      c(a, 0, e);
    }
  } else c(a, 0, "IndexedDB not available!");
}
var eb = 1,
  fb = [],
  V = [],
  gb = [],
  W = [],
  X = [],
  hb = [],
  ib = [],
  jb = a => {
    for (var b = eb++, c = a.length; c < b; c++) a[c] = null;
    return b;
  },
  kb = (a, b, c, d) => {
    for (var f = 0; f < a; f++) {
      var k = Y[c](),
        e = k && jb(d);
      k && (k.name = e, d[e] = k);
      y[b + 4 * f >> 2] = e;
    }
  },
  mb = (a, b) => {
    a.va || (a.va = a.getContext, a.getContext = function (d, f) {
      f = a.va(d, f);
      return "webgl" == d == f instanceof WebGLRenderingContext ? f : null;
    });
    var c = a.getContext("webgl2", b);
    return c ? lb(c, b) : 0;
  },
  lb = (a, b) => {
    var c = jb(ib);
    b = {
      handle: c,
      attributes: b,
      version: b.Pa,
      La: a
    };
    a.canvas && (a.canvas.Qa = b);
    ib[c] = b;
    return c;
  },
  nb,
  ob = ["default", "low-power", "high-performance"],
  pb = [null, [], []],
  qb = a => "]" == a.slice(-1) && a.lastIndexOf("["),
  rb = a => {
    var b = Y.Na,
      c = b.xa[a];
    "number" == typeof c && (b.xa[a] = c = Y.getUniformLocation(b, b.Ia[a] + (0 < c ? `[${c}]` : "")));
    return c;
  };
R = new Xa();
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
  S = a;
  wa();
}, () => {
  S = !1;
  wa();
});
var Y,
  sb = {
    e: (a, b, c, d) => {
      ha(`Assertion failed: ${G(a)}, at: ` + [b ? G(b) : "unknown filename", c, d ? G(d) : "unknown function"]);
    },
    z: (a, b, c) => {
      var d = new Ha(a);
      A[d.va + 16 >> 2] = 0;
      A[d.va + 4 >> 2] = b;
      A[d.va + 8 >> 2] = c;
      Ia = a;
      Ja++;
      throw Ia;
    },
    R: function (a) {
      if (R.has(a)) {
        var b = R.get(a),
          c = R;
        c.va[a] = void 0;
        c.Ka.push(a);
        0 < b.readyState && 4 > b.readyState && b.abort();
      }
    },
    N: () => 1,
    s: () => {
      ha("");
    },
    O: () => Date.now(),
    Q: () => performance.now(),
    T: () => !0,
    na: (a, b) => {
      function c(d) {
        M.get(a)(d, b) && requestAnimationFrame(c);
      }
      return requestAnimationFrame(c);
    },
    P: a => {
      var b = w.length;
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
            var k = 1;
            break a;
          } catch (e) {}
          k = void 0;
        }
        if (k) return !0;
      }
      return !1;
    },
    ha: (a, b, c) => {
      a = Ta(a);
      if (!a) return -4;
      a.width = b;
      a.height = c;
      return 0;
    },
    ma: (a, b, c, d) => Ua(a, b, c, d, 2, "keydown"),
    la: (a, b, c, d) => Ua(a, b, c, d, 3, "keyup"),
    ka: (a, b, c, d) => Va(a, b, c, d, 5, "mousedown"),
    ia: (a, b, c, d) => Va(a, b, c, d, 8, "mousemove"),
    ja: (a, b, c, d) => Va(a, b, c, d, 6, "mouseup"),
    S: function (a, b, c, d, f) {
      function k(t) {
        if (U) t();else if (!ja) try {
          if (t(), !Fa) try {
            u = t = u, ab(t);
          } catch (ba) {
            $a(ba);
          }
        } catch (ba) {
          $a(ba);
        }
      }
      var e = a + 112,
        m = A[e + 36 >> 2],
        h = A[e + 40 >> 2],
        r = A[e + 44 >> 2],
        C = A[e + 48 >> 2],
        B = A[e + 52 >> 2],
        U = !!(B & 64),
        L = t => {
          k(() => {
            m ? M.get(m)(t) : b === null || b === void 0 ? void 0 : b(t);
          });
        },
        P = t => {
          k(() => {
            r ? M.get(r)(t) : d === null || d === void 0 ? void 0 : d(t);
          });
        },
        I = t => {
          k(() => {
            h ? M.get(h)(t) : c === null || c === void 0 ? void 0 : c(t);
          });
        },
        H = t => {
          k(() => {
            C ? M.get(C)(t) : f === null || f === void 0 ? void 0 : f(t);
          });
        },
        K = t => {
          Ya(t, L, I, P, H);
        },
        aa = (t, ba) => {
          bb(t, ba.response, ca => {
            k(() => {
              m ? M.get(m)(ca) : b === null || b === void 0 ? void 0 : b(ca);
            });
          }, ca => {
            k(() => {
              m ? M.get(m)(ca) : b === null || b === void 0 ? void 0 : b(ca);
            });
          });
        },
        p = t => {
          Ya(t, aa, I, P, H);
        },
        J = G(e + 0),
        v = !!(B & 16),
        z = !!(B & 4);
      B = !!(B & 32);
      if ("EM_IDB_STORE" === J) K = A[e + 84 >> 2], bb(a, w.slice(K, K + A[e + 88 >> 2]), L, I);else if ("EM_IDB_DELETE" === J) db(a, L, I);else if (v) {
        if (B) return 0;
        Ya(a, z ? aa : L, I, P, H);
      } else cb(a, L, B ? I : z ? p : K);
      return a;
    },
    ga: (a, b) => {
      b >>= 2;
      b = {
        alpha: !!y[b],
        depth: !!y[b + 1],
        stencil: !!y[b + 2],
        antialias: !!y[b + 3],
        premultipliedAlpha: !!y[b + 4],
        preserveDrawingBuffer: !!y[b + 5],
        powerPreference: ob[y[b + 6]],
        failIfMajorPerformanceCaveat: !!y[b + 7],
        Pa: y[b + 8],
        Ua: y[b + 9],
        Ta: y[b + 10],
        Oa: y[b + 11],
        Va: y[b + 12],
        Wa: y[b + 13]
      };
      a = Ta(a);
      return !a || b.Oa ? 0 : mb(a, b);
    },
    fa: a => {
      var _nb;
      nb = ib[a];
      g.Sa = Y = (_nb = nb) === null || _nb === void 0 ? void 0 : _nb.La;
      return !a || Y ? 0 : -5;
    },
    y: (a, b, c, d) => {
      for (var f = 0, k = 0; k < c; k++) {
        var e = A[b >> 2],
          m = A[b + 4 >> 2];
        b += 8;
        for (var h = 0; h < m; h++) {
          var r = w[e + h],
            C = pb[a];
          if (0 === r || 10 === r) {
            for (r = 0; C[r] && !(NaN <= r);) ++r;
            r = Ga.decode(C.buffer ? C.subarray(0, r) : new Uint8Array(C.slice(0, r)));
            (1 === a ? fa : n)(r);
            C.length = 0;
          } else C.push(r);
        }
        f += m;
      }
      A[d >> 2] = f;
      return 0;
    },
    da: a => Y.activeTexture(a),
    C: (a, b) => {
      Y.attachShader(V[a], X[b]);
    },
    c: (a, b) => {
      35051 == a ? Y.Fa = b : 35052 == a && (Y.Ba = b);
      Y.bindBuffer(a, fb[b]);
    },
    x: (a, b) => {
      Y.bindFramebuffer(a, gb[b]);
    },
    b: (a, b) => {
      Y.bindTexture(a, W[b]);
    },
    i: a => {
      Y.bindVertexArray(hb[a]);
    },
    p: a => Y.blendEquation(a),
    q: (a, b) => Y.blendFunc(a, b),
    j: (a, b, c, d) => {
      c && b ? Y.bufferData(a, w, d, c, b) : Y.bufferData(a, b, d);
    },
    I: a => Y.clear(a),
    J: (a, b, c, d) => Y.clearColor(a, b, c, d),
    X: a => {
      Y.compileShader(X[a]);
    },
    aa: () => {
      var a = jb(V),
        b = Y.createProgram();
      b.name = a;
      b.Aa = b.ya = b.za = 0;
      b.Ca = 1;
      V[a] = b;
      return a;
    },
    Z: a => {
      var b = jb(X);
      X[b] = Y.createShader(a);
      return b;
    },
    o: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2],
          f = fb[d];
        f && (Y.deleteBuffer(f), f.name = 0, fb[d] = null, d == Y.Fa && (Y.Fa = 0), d == Y.Ba && (Y.Ba = 0));
      }
    },
    r: (a, b) => {
      for (var c = 0; c < a; ++c) {
        var d = y[b + 4 * c >> 2],
          f = gb[d];
        f && (Y.deleteFramebuffer(f), f.name = 0, gb[d] = null);
      }
    },
    k: a => {
      if (a) {
        var b = V[a];
        b && (Y.deleteProgram(b), b.name = 0, V[a] = null);
      }
    },
    d: a => {
      if (a) {
        var b = X[a];
        b && (Y.deleteShader(b), X[a] = null);
      }
    },
    M: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2],
          f = W[d];
        f && (Y.deleteTexture(f), f.name = 0, W[d] = null);
      }
    },
    G: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2];
        Y.deleteVertexArray(hb[d]);
        hb[d] = null;
      }
    },
    H: a => Y.disable(a),
    v: (a, b, c, d) => {
      Y.drawArraysInstanced(a, b, c, d);
    },
    ba: (a, b, c, d) => {
      Y.drawElements(a, b, c, d);
    },
    ea: a => Y.enable(a),
    f: a => {
      Y.enableVertexAttribArray(a);
    },
    L: (a, b, c, d, f) => {
      Y.framebufferTexture2D(a, b, c, W[d], f);
    },
    n: (a, b) => {
      kb(a, b, "createBuffer", fb);
    },
    K: (a, b) => {
      kb(a, b, "createFramebuffer", gb);
    },
    l: (a, b) => {
      kb(a, b, "createTexture", W);
    },
    D: (a, b) => {
      kb(a, b, "createVertexArray", hb);
    },
    h: (a, b) => Y.getAttribLocation(V[a], G(b)),
    _: (a, b, c, d) => {
      a = Y.getProgramInfoLog(V[a]);
      b = 0 < b && d ? O(a, d, b) : 0;
      c && (y[c >> 2] = b);
    },
    B: (a, b, c) => {
      if (c && !(a >= eb)) if (a = V[a], 35716 == b) y[c >> 2] = Y.getProgramInfoLog(a).length + 1;else if (35719 == b) {
        if (!a.Aa) for (b = 0; b < Y.getProgramParameter(a, 35718); ++b) a.Aa = Math.max(a.Aa, Y.getActiveUniform(a, b).name.length + 1);
        y[c >> 2] = a.Aa;
      } else if (35722 == b) {
        if (!a.ya) for (b = 0; b < Y.getProgramParameter(a, 35721); ++b) a.ya = Math.max(a.ya, Y.getActiveAttrib(a, b).name.length + 1);
        y[c >> 2] = a.ya;
      } else if (35381 == b) {
        if (!a.za) for (b = 0; b < Y.getProgramParameter(a, 35382); ++b) a.za = Math.max(a.za, Y.getActiveUniformBlockName(a, b).length + 1);
        y[c >> 2] = a.za;
      } else y[c >> 2] = Y.getProgramParameter(a, b);
    },
    W: (a, b, c, d) => {
      a = Y.getShaderInfoLog(X[a]);
      b = 0 < b && d ? O(a, d, b) : 0;
      c && (y[c >> 2] = b);
    },
    A: (a, b, c) => {
      c && (35716 == b ? (a = Y.getShaderInfoLog(X[a]), y[c >> 2] = a ? a.length + 1 : 0) : 35720 == b ? (a = Y.getShaderSource(X[a]), y[c >> 2] = a ? a.length + 1 : 0) : y[c >> 2] = Y.getShaderParameter(X[a], b));
    },
    u: (a, b) => {
      b = G(b);
      if (a = V[a]) {
        var c = a,
          d = c.xa,
          f = c.Ja,
          k;
        if (!d) for (c.xa = d = {}, c.Ia = {}, k = 0; k < Y.getProgramParameter(c, 35718); ++k) {
          var e = Y.getActiveUniform(c, k);
          var m = e.name;
          e = e.size;
          var h = qb(m);
          h = 0 < h ? m.slice(0, h) : m;
          var r = c.Ca;
          c.Ca += e;
          f[h] = [e, r];
          for (m = 0; m < e; ++m) d[r] = m, c.Ia[r++] = h;
        }
        c = a.xa;
        d = 0;
        f = b;
        k = qb(b);
        0 < k && (d = parseInt(b.slice(k + 1)) >>> 0, f = b.slice(0, k));
        if ((f = a.Ja[f]) && d < f[0] && (d += f[1], c[d] = c[d] || Y.getUniformLocation(a, b))) return d;
      }
      return -1;
    },
    $: a => {
      a = V[a];
      Y.linkProgram(a);
      a.xa = 0;
      a.Ja = {};
    },
    Y: (a, b, c, d) => {
      for (var f = "", k = 0; k < b; ++k) f += G(A[c + 4 * k >> 2], d ? A[d + 4 * k >> 2] : void 0);
      Y.shaderSource(X[a], f);
    },
    t: (a, b, c, d, f, k, e, m, h) => {
      if (Y.Ba) Y.texImage2D(a, b, c, d, f, k, e, m, h);else if (h) {
        var r = m - 5120;
        r = 0 == r ? ka : 1 == r ? w : 2 == r ? x : 4 == r ? y : 6 == r ? ma : 5 == r || 28922 == r || 28520 == r || 30779 == r || 30782 == r ? A : la;
        Y.texImage2D(a, b, c, d, f, k, e, m, r, h >>> 31 - Math.clz32(r.BYTES_PER_ELEMENT));
      } else Y.texImage2D(a, b, c, d, f, k, e, m, null);
    },
    a: (a, b, c) => Y.texParameteri(a, b, c),
    ca: (a, b) => {
      Y.uniform1i(rb(a), b);
    },
    E: (a, b, c) => {
      Y.uniform2f(rb(a), b, c);
    },
    F: a => {
      a = V[a];
      Y.useProgram(a);
      Y.Na = a;
    },
    m: (a, b) => {
      Y.vertexAttribDivisor(a, b);
    },
    g: (a, b, c, d, f, k) => {
      Y.vertexAttribPointer(a, b, c, !!d, f, k);
    },
    w: (a, b, c, d) => Y.viewport(a, b, c, d),
    V: function (a, b, c, d) {
      var f = document.createElement("canvas");
      f.width = b;
      f.height = c;
      b = f.getContext("2d");
      b.font = a + "px " + G(d);
      b.textBaseline = "middle";
      b.globalAlpha = 1;
      b.fillStyle = "white";
      window["gCanvas" + a] = f;
      window["gCanvasCtx" + a] = b;
    },
    oa: function (a, b, c, d) {
      var f = new Image();
      f.onload = () => {
        A[c >> 2] = f.width;
        A[d >> 2] = f.height;
        Y.bindTexture(Y.TEXTURE_2D, W[a]);
        Y.texImage2D(Y.TEXTURE_2D, 0, Y.RGBA, Y.RGBA, Y.UNSIGNED_BYTE, f);
      };
      f.src = G(b);
    },
    U: function (a, b) {
      var c = window["gCanvas" + a];
      a = window["gCanvasCtx" + a];
      a.clearRect(0, 0, c.width, c.height);
      b = G(b);
      var d = a.measureText(b).width;
      a.fillText(b, 0, c.height / 2);
      Y.texImage2D(Y.TEXTURE_2D, 0, Y.RGBA, Y.RGBA, Y.UNSIGNED_BYTE, c);
      return d;
    }
  },
  Z = function () {
    function a(c) {
      Z = c.exports;
      ia = Z.pa;
      oa();
      M = Z.ta;
      qa.unshift(Z.qa);
      wa();
      return Z;
    }
    var b = {
      a: sb
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
  Q = a => (Q = Z.ra)(a),
  Za = a => (Za = Z.sa)(a),
  tb = g._main = (a, b) => (tb = g._main = Z.ua)(a, b),
  ub;
E = function vb() {
  ub || wb();
  ub || (E = vb);
};
function wb() {
  function a() {
    if (!ub && (ub = !0, g.calledRun = !0, !ja)) {
      Ea(qa);
      Ea(ra);
      if (g.onRuntimeInitialized) g.onRuntimeInitialized();
      if (xb) {
        var b = tb;
        try {
          var c = b(0, 0);
          u = c;
          ab(c);
        } catch (d) {
          $a(d);
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
var xb = !0;
g.noInitialRun && (xb = !1);
wb();
