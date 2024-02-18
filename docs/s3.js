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
"object" != typeof WebAssembly && u("no native wasm support detected");
var ha,
  ia = !1,
  w,
  ja,
  x,
  y,
  ka,
  A,
  D,
  la,
  ma;
function na() {
  var a = ha.buffer;
  g.HEAP8 = ja = new Int8Array(a);
  g.HEAP16 = y = new Int16Array(a);
  g.HEAPU8 = x = new Uint8Array(a);
  g.HEAPU16 = ka = new Uint16Array(a);
  g.HEAP32 = A = new Int32Array(a);
  g.HEAPU32 = D = new Uint32Array(a);
  g.HEAPF32 = la = new Float32Array(a);
  g.HEAPF64 = ma = new Float64Array(a);
}
var oa = [],
  pa = [],
  qa = [],
  ra = [];
function sa() {
  var a = g.preRun.shift();
  oa.unshift(a);
}
var E = 0,
  ta = null,
  F = null;
function ua() {
  var _g$monitorRunDependen, _g;
  E++;
  (_g$monitorRunDependen = (_g = g).monitorRunDependencies) === null || _g$monitorRunDependen === void 0 || _g$monitorRunDependen.call(_g, E);
}
function va() {
  var _g$monitorRunDependen2, _g2;
  E--;
  (_g$monitorRunDependen2 = (_g2 = g).monitorRunDependencies) === null || _g$monitorRunDependen2 === void 0 || _g$monitorRunDependen2.call(_g2, E);
  if (0 == E && (null !== ta && (clearInterval(ta), ta = null), F)) {
    var a = F;
    F = null;
    a();
  }
}
function u(a) {
  var _g$onAbort, _g3;
  (_g$onAbort = (_g3 = g).onAbort) === null || _g$onAbort === void 0 || _g$onAbort.call(_g3, a);
  a = "Aborted(" + a + ")";
  n(a);
  ia = !0;
  w = 1;
  throw new WebAssembly.RuntimeError(a + ". Build with -sASSERTIONS for more info.");
}
var wa = a => a.startsWith("data:application/octet-stream;base64,"),
  G;
G = "s3.wasm";
if (!wa(G)) {
  var xa = G;
  G = g.locateFile ? g.locateFile(xa, l) : l + xa;
}
function ya(a) {
  if (a == G && q) return new Uint8Array(q);
  throw "both async and sync fetching of the wasm failed";
}
function za(a) {
  return q || "function" != typeof fetch ? Promise.resolve().then(() => ya(a)) : fetch(a, {
    credentials: "same-origin"
  }).then(b => {
    if (!b.ok) throw `failed to load wasm binary file at '${a}'`;
    return b.arrayBuffer();
  }).catch(() => ya(a));
}
function Aa(a, b, c) {
  return za(a).then(d => WebAssembly.instantiate(d, b)).then(c, d => {
    n(`failed to asynchronously prepare wasm: ${d}`);
    u(d);
  });
}
function Ba(a, b) {
  var c = G;
  q || "function" != typeof WebAssembly.instantiateStreaming || wa(c) || "function" != typeof fetch ? Aa(c, a, b) : fetch(c, {
    credentials: "same-origin"
  }).then(d => WebAssembly.instantiateStreaming(d, a).then(b, function (f) {
    n(`wasm streaming compile failed: ${f}`);
    n("falling back to ArrayBuffer instantiation");
    return Aa(c, a, b);
  }));
}
function Ca(a) {
  this.name = "ExitStatus";
  this.message = `Program terminated with exit(${a})`;
  this.status = a;
}
var Da = a => {
    for (; 0 < a.length;) a.shift()(g);
  },
  Ea = g.noExitRuntime || !0,
  Fa = new TextDecoder("utf8"),
  M = (a, b) => {
    if (!a) return "";
    b = a + b;
    for (var c = a; !(c >= b) && x[c];) ++c;
    return Fa.decode(x.subarray(a, c));
  };
class Ga {
  constructor(a) {
    this.va = a - 24;
  }
}
var Ha = 0,
  Ia = 0,
  N,
  Ja = 0,
  Ka = [];
function La() {
  if (navigator.userActivation ? navigator.userActivation.isActive : Ja && Ma.Ma) for (var a = 0; a < Ka.length; ++a) {
    var b = Ka[a];
    Ka.splice(a, 1);
    --a;
    b.Xa(...b.Ra);
  }
}
var O = [];
function Na(a) {
  var b = O[a];
  b.target.removeEventListener(b.wa, b.Ga, b.Da);
  O.splice(a, 1);
}
function Oa(a) {
  if (!a.target) return -4;
  if (a.Ea) a.Ga = function (c) {
    ++Ja;
    Ma = a;
    La();
    a.Ha(c);
    La();
    --Ja;
  }, a.target.addEventListener(a.wa, a.Ga, a.Da), O.push(a);else for (var b = 0; b < O.length; ++b) O[b].target == a.target && O[b].wa == a.wa && Na(b--);
  return 0;
}
var Ma,
  Pa,
  Qa,
  Ra = [0, document, window],
  Sa = a => {
    a = 2 < a ? M(a) : a;
    return Ra[a] || document.querySelector(a);
  },
  Q = (a, b, c) => {
    var d = x;
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
  Ta = (a, b, c, d, f, k) => {
    Pa || (Pa = R(176));
    a = {
      target: Sa(a),
      wa: k,
      Ea: d,
      Ha: e => {
        var m = Pa;
        ma[m >> 3] = e.timeStamp;
        var h = m >> 2;
        A[h + 2] = e.location;
        A[h + 3] = e.ctrlKey;
        A[h + 4] = e.shiftKey;
        A[h + 5] = e.altKey;
        A[h + 6] = e.metaKey;
        A[h + 7] = e.repeat;
        A[h + 8] = e.charCode;
        A[h + 9] = e.keyCode;
        A[h + 10] = e.which;
        Q(e.key || "", m + 44, 32);
        Q(e.code || "", m + 76, 32);
        Q(e.char || "", m + 108, 32);
        Q(e.locale || "", m + 140, 32);
        N.get(d)(f, m, b) && e.preventDefault();
      },
      Da: c
    };
    return Oa(a);
  },
  Ua = (a, b, c, d, f, k) => {
    Qa || (Qa = R(72));
    a = Sa(a);
    return Oa({
      target: a,
      Ma: "mousemove" != k && "mouseenter" != k && "mouseleave" != k,
      wa: k,
      Ea: d,
      Ha: function () {
        let e = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : event;
        var m = a,
          h = Qa;
        ma[h >> 3] = e.timeStamp;
        h >>= 2;
        A[h + 2] = e.screenX;
        A[h + 3] = e.screenY;
        A[h + 4] = e.clientX;
        A[h + 5] = e.clientY;
        A[h + 6] = e.ctrlKey;
        A[h + 7] = e.shiftKey;
        A[h + 8] = e.altKey;
        A[h + 9] = e.metaKey;
        y[2 * h + 20] = e.button;
        y[2 * h + 21] = e.buttons;
        A[h + 11] = e.movementX;
        A[h + 12] = e.movementY;
        m = 0 > Ra.indexOf(m) ? m.getBoundingClientRect() : {
          left: 0,
          top: 0
        };
        A[h + 13] = e.clientX - m.left;
        A[h + 14] = e.clientY - m.top;
        N.get(d)(f, Qa, b) && e.preventDefault();
      },
      Da: c
    });
  };
function Va(a) {
  var b = S,
    c = b.Ka.pop() || b.va.length;
  b.va[c] = a;
  return c;
}
class Wa {
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
var S, Xa;
function Ya(a, b, c, d, f) {
  function k() {
    var v = 0,
      z = 0;
    p.response && K && 0 === D[a + 12 >> 2] && (z = p.response.byteLength);
    0 < z && (v = R(z), x.set(new Uint8Array(p.response), v));
    D[a + 12 >> 2] = v;
    T(a + 16, z);
    T(a + 24, 0);
    (v = p.response ? p.response.byteLength : 0) && T(a + 32, v);
    y[a + 40 >> 1] = p.readyState;
    y[a + 42 >> 1] = p.status;
    p.statusText && Q(p.statusText, a + 44, 64);
  }
  var e = D[a + 8 >> 2];
  if (e) {
    var m = M(e),
      h = a + 112,
      r = M(h + 0);
    r || (r = "GET");
    var C = D[h + 56 >> 2],
      B = D[h + 68 >> 2],
      U = D[h + 72 >> 2];
    e = D[h + 76 >> 2];
    var L = D[h + 80 >> 2],
      P = D[h + 84 >> 2],
      I = D[h + 88 >> 2],
      H = D[h + 52 >> 2],
      K = !!(H & 1),
      ba = !!(H & 2);
    H = !!(H & 64);
    B = B ? M(B) : void 0;
    U = U ? M(U) : void 0;
    var p = new XMLHttpRequest();
    p.withCredentials = !!x[h + 60];
    p.open(r, m, !H, B, U);
    H || (p.timeout = C);
    p.va = m;
    p.responseType = "arraybuffer";
    L && (m = M(L), p.overrideMimeType(m));
    if (e) for (;;) {
      h = D[e >> 2];
      if (!h) break;
      m = D[e + 4 >> 2];
      if (!m) break;
      e += 8;
      h = M(h);
      m = M(m);
      p.setRequestHeader(h, m);
    }
    var J = Va(p);
    D[a >> 2] = J;
    e = P && I ? x.slice(P, P + I) : null;
    p.onload = v => {
      S.has(J) && (k(), 200 <= p.status && 300 > p.status ? b === null || b === void 0 ? void 0 : b(a, p, v) : c === null || c === void 0 ? void 0 : c(a, p, v));
    };
    p.onerror = v => {
      S.has(J) && (k(), c === null || c === void 0 ? void 0 : c(a, p, v));
    };
    p.ontimeout = v => {
      S.has(J) && (c === null || c === void 0 ? void 0 : c(a, p, v));
    };
    p.onprogress = v => {
      if (S.has(J)) {
        var z = K && ba && p.response ? p.response.byteLength : 0,
          t = 0;
        0 < z && K && ba && (t = R(z), x.set(new Uint8Array(p.response), t));
        D[a + 12 >> 2] = t;
        T(a + 16, z);
        T(a + 24, v.loaded - z);
        T(a + 32, v.total);
        y[a + 40 >> 1] = p.readyState;
        3 <= p.readyState && 0 === p.status && 0 < v.loaded && (p.status = 200);
        y[a + 42 >> 1] = p.status;
        p.statusText && Q(p.statusText, a + 44, 64);
        d === null || d === void 0 || d(a, p, v);
        t && Za(t);
      }
    };
    p.onreadystatechange = v => {
      S.has(J) && (y[a + 40 >> 1] = p.readyState, 2 <= p.readyState && (y[a + 42 >> 1] = p.status), f === null || f === void 0 ? void 0 : f(a, p, v));
    };
    try {
      p.send(e);
    } catch (v) {
      c === null || c === void 0 || c(a, p, v);
    }
  } else c(a, 0, "no url specified!");
}
var $a = a => {
    a instanceof Ca || "unwind" == a || ea(1, a);
  },
  ab = a => {
    var _g$onExit, _g4;
    w = a;
    Ea || ((_g$onExit = (_g4 = g).onExit) !== null && _g$onExit !== void 0 && _g$onExit.call(_g4, a), ia = !0);
    ea(a, new Ca(a));
  },
  T = (a, b) => {
    D[a >> 2] = b;
    D[a + 4 >> 2] = (b - D[a >> 2]) / 4294967296;
  };
function bb(a, b, c, d) {
  var f = Xa;
  if (f) {
    var k = D[a + 112 + 64 >> 2];
    k || (k = D[a + 8 >> 2]);
    var e = M(k);
    try {
      var m = f.transaction(["FILES"], "readwrite").objectStore("FILES").put(b, e);
      m.onsuccess = () => {
        y[a + 40 >> 1] = 4;
        y[a + 42 >> 1] = 200;
        Q("OK", a + 44, 64);
        c(a, 0, e);
      };
      m.onerror = h => {
        y[a + 40 >> 1] = 4;
        y[a + 42 >> 1] = 413;
        Q("Payload Too Large", a + 44, 64);
        d(a, 0, h);
      };
    } catch (h) {
      d(a, 0, h);
    }
  } else d(a, 0, "IndexedDB not available!");
}
function cb(a, b, c) {
  var d = Xa;
  if (d) {
    var f = D[a + 112 + 64 >> 2];
    f || (f = D[a + 8 >> 2]);
    f = M(f);
    try {
      var k = d.transaction(["FILES"], "readonly").objectStore("FILES").get(f);
      k.onsuccess = e => {
        if (e.target.result) {
          e = e.target.result;
          var m = e.byteLength || e.length,
            h = R(m);
          x.set(new Uint8Array(e), h);
          D[a + 12 >> 2] = h;
          T(a + 16, m);
          T(a + 24, 0);
          T(a + 32, m);
          y[a + 40 >> 1] = 4;
          y[a + 42 >> 1] = 200;
          Q("OK", a + 44, 64);
          b(a, 0, e);
        } else y[a + 40 >> 1] = 4, y[a + 42 >> 1] = 404, Q("Not Found", a + 44, 64), c(a, 0, "no data");
      };
      k.onerror = e => {
        y[a + 40 >> 1] = 4;
        y[a + 42 >> 1] = 404;
        Q("Not Found", a + 44, 64);
        c(a, 0, e);
      };
    } catch (e) {
      c(a, 0, e);
    }
  } else c(a, 0, "IndexedDB not available!");
}
function db(a, b, c) {
  var d = Xa;
  if (d) {
    var f = D[a + 112 + 64 >> 2];
    f || (f = D[a + 8 >> 2]);
    f = M(f);
    try {
      var k = d.transaction(["FILES"], "readwrite").objectStore("FILES").delete(f);
      k.onsuccess = e => {
        e = e.target.result;
        D[a + 12 >> 2] = 0;
        T(a + 16, 0);
        T(a + 24, 0);
        T(a + 32, 0);
        y[a + 40 >> 1] = 4;
        y[a + 42 >> 1] = 200;
        Q("OK", a + 44, 64);
        b(a, 0, e);
      };
      k.onerror = e => {
        y[a + 40 >> 1] = 4;
        y[a + 42 >> 1] = 404;
        Q("Not Found", a + 44, 64);
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
      A[b + 4 * f >> 2] = e;
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
  qb = () => {
    if ("object" == typeof crypto && "function" == typeof crypto.getRandomValues) return a => crypto.getRandomValues(a);
    u("initRandomDevice");
  },
  rb = a => (rb = qb())(a),
  sb = a => "]" == a.slice(-1) && a.lastIndexOf("["),
  tb = a => {
    var b = Y.Na,
      c = b.xa[a];
    "number" == typeof c && (b.xa[a] = c = Y.getUniformLocation(b, b.Ia[a] + (0 < c ? `[${c}]` : "")));
    return c;
  };
S = new Wa();
ua();
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
  Xa = a;
  va();
}, () => {
  Xa = !1;
  va();
});
var Y,
  ub = {
    d: (a, b, c, d) => {
      u(`Assertion failed: ${M(a)}, at: ` + [b ? M(b) : "unknown filename", c, d ? M(d) : "unknown function"]);
    },
    J: (a, b, c) => {
      var d = new Ga(a);
      D[d.va + 16 >> 2] = 0;
      D[d.va + 4 >> 2] = b;
      D[d.va + 8 >> 2] = c;
      Ha = a;
      Ia++;
      throw Ha;
    },
    P: function (a) {
      if (S.has(a)) {
        var b = S.get(a),
          c = S;
        c.va[a] = void 0;
        c.Ka.push(a);
        0 < b.readyState && 4 > b.readyState && b.abort();
      }
    },
    M: () => 1,
    r: () => {
      u("");
    },
    O: () => performance.now(),
    R: () => !0,
    oa: (a, b) => {
      function c(d) {
        N.get(a)(d, b) && requestAnimationFrame(c);
      }
      return requestAnimationFrame(c);
    },
    N: a => {
      var b = x.length;
      a >>>= 0;
      if (2147483648 < a) return !1;
      for (var c = 1; 4 >= c; c *= 2) {
        var d = b * (1 + .2 / c);
        d = Math.min(d, a + 100663296);
        var f = Math;
        d = Math.max(a, d);
        a: {
          f = (f.min.call(f, 2147483648, d + (65536 - d % 65536) % 65536) - ha.buffer.byteLength + 65535) / 65536;
          try {
            ha.grow(f);
            na();
            var k = 1;
            break a;
          } catch (e) {}
          k = void 0;
        }
        if (k) return !0;
      }
      return !1;
    },
    ia: (a, b, c) => {
      a = Sa(a);
      if (!a) return -4;
      a.width = b;
      a.height = c;
      return 0;
    },
    na: (a, b, c, d) => Ta(a, b, c, d, 2, "keydown"),
    ma: (a, b, c, d) => Ta(a, b, c, d, 3, "keyup"),
    la: (a, b, c, d) => Ua(a, b, c, d, 5, "mousedown"),
    ja: (a, b, c, d) => Ua(a, b, c, d, 8, "mousemove"),
    ka: (a, b, c, d) => Ua(a, b, c, d, 6, "mouseup"),
    Q: function (a, b, c, d, f) {
      function k(t) {
        if (U) t();else if (!ia) try {
          if (t(), !Ea) try {
            w = t = w, ab(t);
          } catch (ca) {
            $a(ca);
          }
        } catch (ca) {
          $a(ca);
        }
      }
      var e = a + 112,
        m = D[e + 36 >> 2],
        h = D[e + 40 >> 2],
        r = D[e + 44 >> 2],
        C = D[e + 48 >> 2],
        B = D[e + 52 >> 2],
        U = !!(B & 64),
        L = t => {
          k(() => {
            m ? N.get(m)(t) : b === null || b === void 0 ? void 0 : b(t);
          });
        },
        P = t => {
          k(() => {
            r ? N.get(r)(t) : d === null || d === void 0 ? void 0 : d(t);
          });
        },
        I = t => {
          k(() => {
            h ? N.get(h)(t) : c === null || c === void 0 ? void 0 : c(t);
          });
        },
        H = t => {
          k(() => {
            C ? N.get(C)(t) : f === null || f === void 0 ? void 0 : f(t);
          });
        },
        K = t => {
          Ya(t, L, I, P, H);
        },
        ba = (t, ca) => {
          bb(t, ca.response, da => {
            k(() => {
              m ? N.get(m)(da) : b === null || b === void 0 ? void 0 : b(da);
            });
          }, da => {
            k(() => {
              m ? N.get(m)(da) : b === null || b === void 0 ? void 0 : b(da);
            });
          });
        },
        p = t => {
          Ya(t, ba, I, P, H);
        },
        J = M(e + 0),
        v = !!(B & 16),
        z = !!(B & 4);
      B = !!(B & 32);
      if ("EM_IDB_STORE" === J) K = D[e + 84 >> 2], bb(a, x.slice(K, K + D[e + 88 >> 2]), L, I);else if ("EM_IDB_DELETE" === J) db(a, L, I);else if (v) {
        if (B) return 0;
        Ya(a, z ? ba : L, I, P, H);
      } else cb(a, L, B ? I : z ? p : K);
      return a;
    },
    ha: (a, b) => {
      b >>= 2;
      b = {
        alpha: !!A[b],
        depth: !!A[b + 1],
        stencil: !!A[b + 2],
        antialias: !!A[b + 3],
        premultipliedAlpha: !!A[b + 4],
        preserveDrawingBuffer: !!A[b + 5],
        powerPreference: ob[A[b + 6]],
        failIfMajorPerformanceCaveat: !!A[b + 7],
        Pa: A[b + 8],
        Ua: A[b + 9],
        Ta: A[b + 10],
        Oa: A[b + 11],
        Va: A[b + 12],
        Wa: A[b + 13]
      };
      a = Sa(a);
      return !a || b.Oa ? 0 : mb(a, b);
    },
    ga: a => {
      var _nb;
      nb = ib[a];
      g.Sa = Y = (_nb = nb) === null || _nb === void 0 ? void 0 : _nb.La;
      return !a || Y ? 0 : -5;
    },
    v: (a, b, c, d) => {
      for (var f = 0, k = 0; k < c; k++) {
        var e = D[b >> 2],
          m = D[b + 4 >> 2];
        b += 8;
        for (var h = 0; h < m; h++) {
          var r = x[e + h],
            C = pb[a];
          if (0 === r || 10 === r) {
            for (r = 0; C[r] && !(NaN <= r);) ++r;
            r = Fa.decode(C.buffer ? C.subarray(0, r) : new Uint8Array(C.slice(0, r)));
            (1 === a ? fa : n)(r);
            C.length = 0;
          } else C.push(r);
        }
        f += m;
      }
      D[d >> 2] = f;
      return 0;
    },
    L: (a, b) => {
      rb(x.subarray(a, a + b));
      return 0;
    },
    ea: a => Y.activeTexture(a),
    D: (a, b) => {
      Y.attachShader(V[a], X[b]);
    },
    c: (a, b) => {
      35051 == a ? Y.Fa = b : 35052 == a && (Y.Ba = b);
      Y.bindBuffer(a, fb[b]);
    },
    y: (a, b) => {
      Y.bindFramebuffer(a, gb[b]);
    },
    b: (a, b) => {
      Y.bindTexture(a, W[b]);
    },
    i: a => {
      Y.bindVertexArray(hb[a]);
    },
    m: a => Y.blendEquation(a),
    n: (a, b) => Y.blendFunc(a, b),
    j: (a, b, c, d) => {
      c && b ? Y.bufferData(a, x, d, c, b) : Y.bufferData(a, b, d);
    },
    w: a => Y.clear(a),
    x: (a, b, c, d) => Y.clearColor(a, b, c, d),
    Y: a => {
      Y.compileShader(X[a]);
    },
    ba: () => {
      var a = jb(V),
        b = Y.createProgram();
      b.name = a;
      b.Aa = b.ya = b.za = 0;
      b.Ca = 1;
      V[a] = b;
      return a;
    },
    _: a => {
      var b = jb(X);
      X[b] = Y.createShader(a);
      return b;
    },
    q: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = A[b + 4 * c >> 2],
          f = fb[d];
        f && (Y.deleteBuffer(f), f.name = 0, fb[d] = null, d == Y.Fa && (Y.Fa = 0), d == Y.Ba && (Y.Ba = 0));
      }
    },
    A: (a, b) => {
      for (var c = 0; c < a; ++c) {
        var d = A[b + 4 * c >> 2],
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
    e: a => {
      if (a) {
        var b = X[a];
        b && (Y.deleteShader(b), X[a] = null);
      }
    },
    V: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = A[b + 4 * c >> 2],
          f = W[d];
        f && (Y.deleteTexture(f), f.name = 0, W[d] = null);
      }
    },
    H: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = A[b + 4 * c >> 2];
        Y.deleteVertexArray(hb[d]);
        hb[d] = null;
      }
    },
    I: a => Y.disable(a),
    t: (a, b, c, d) => {
      Y.drawArraysInstanced(a, b, c, d);
    },
    ca: (a, b, c, d) => {
      Y.drawElements(a, b, c, d);
    },
    fa: a => Y.enable(a),
    f: a => {
      Y.enableVertexAttribArray(a);
    },
    S: (a, b, c, d, f) => {
      Y.framebufferTexture2D(a, b, c, W[d], f);
    },
    p: (a, b) => {
      kb(a, b, "createBuffer", fb);
    },
    W: (a, b) => {
      kb(a, b, "createFramebuffer", gb);
    },
    l: (a, b) => {
      kb(a, b, "createTexture", W);
    },
    E: (a, b) => {
      kb(a, b, "createVertexArray", hb);
    },
    h: (a, b) => Y.getAttribLocation(V[a], M(b)),
    $: (a, b, c, d) => {
      a = Y.getProgramInfoLog(V[a]);
      b = 0 < b && d ? Q(a, d, b) : 0;
      c && (A[c >> 2] = b);
    },
    C: (a, b, c) => {
      if (c && !(a >= eb)) if (a = V[a], 35716 == b) A[c >> 2] = Y.getProgramInfoLog(a).length + 1;else if (35719 == b) {
        if (!a.Aa) for (b = 0; b < Y.getProgramParameter(a, 35718); ++b) a.Aa = Math.max(a.Aa, Y.getActiveUniform(a, b).name.length + 1);
        A[c >> 2] = a.Aa;
      } else if (35722 == b) {
        if (!a.ya) for (b = 0; b < Y.getProgramParameter(a, 35721); ++b) a.ya = Math.max(a.ya, Y.getActiveAttrib(a, b).name.length + 1);
        A[c >> 2] = a.ya;
      } else if (35381 == b) {
        if (!a.za) for (b = 0; b < Y.getProgramParameter(a, 35382); ++b) a.za = Math.max(a.za, Y.getActiveUniformBlockName(a, b).length + 1);
        A[c >> 2] = a.za;
      } else A[c >> 2] = Y.getProgramParameter(a, b);
    },
    X: (a, b, c, d) => {
      a = Y.getShaderInfoLog(X[a]);
      b = 0 < b && d ? Q(a, d, b) : 0;
      c && (A[c >> 2] = b);
    },
    B: (a, b, c) => {
      c && (35716 == b ? (a = Y.getShaderInfoLog(X[a]), A[c >> 2] = a ? a.length + 1 : 0) : 35720 == b ? (a = Y.getShaderSource(X[a]), A[c >> 2] = a ? a.length + 1 : 0) : A[c >> 2] = Y.getShaderParameter(X[a], b));
    },
    u: (a, b) => {
      b = M(b);
      if (a = V[a]) {
        var c = a,
          d = c.xa,
          f = c.Ja,
          k;
        if (!d) for (c.xa = d = {}, c.Ia = {}, k = 0; k < Y.getProgramParameter(c, 35718); ++k) {
          var e = Y.getActiveUniform(c, k);
          var m = e.name;
          e = e.size;
          var h = sb(m);
          h = 0 < h ? m.slice(0, h) : m;
          var r = c.Ca;
          c.Ca += e;
          f[h] = [e, r];
          for (m = 0; m < e; ++m) d[r] = m, c.Ia[r++] = h;
        }
        c = a.xa;
        d = 0;
        f = b;
        k = sb(b);
        0 < k && (d = parseInt(b.slice(k + 1)) >>> 0, f = b.slice(0, k));
        if ((f = a.Ja[f]) && d < f[0] && (d += f[1], c[d] = c[d] || Y.getUniformLocation(a, b))) return d;
      }
      return -1;
    },
    aa: a => {
      a = V[a];
      Y.linkProgram(a);
      a.xa = 0;
      a.Ja = {};
    },
    Z: (a, b, c, d) => {
      for (var f = "", k = 0; k < b; ++k) f += M(D[c + 4 * k >> 2], d ? D[d + 4 * k >> 2] : void 0);
      Y.shaderSource(X[a], f);
    },
    z: (a, b, c, d, f, k, e, m, h) => {
      if (Y.Ba) Y.texImage2D(a, b, c, d, f, k, e, m, h);else if (h) {
        var r = m - 5120;
        r = 0 == r ? ja : 1 == r ? x : 2 == r ? y : 4 == r ? A : 6 == r ? la : 5 == r || 28922 == r || 28520 == r || 30779 == r || 30782 == r ? D : ka;
        Y.texImage2D(a, b, c, d, f, k, e, m, r, h >>> 31 - Math.clz32(r.BYTES_PER_ELEMENT));
      } else Y.texImage2D(a, b, c, d, f, k, e, m, null);
    },
    a: (a, b, c) => Y.texParameteri(a, b, c),
    da: (a, b) => {
      Y.uniform1i(tb(a), b);
    },
    F: (a, b, c) => {
      Y.uniform2f(tb(a), b, c);
    },
    G: a => {
      a = V[a];
      Y.useProgram(a);
      Y.Na = a;
    },
    o: (a, b) => {
      Y.vertexAttribDivisor(a, b);
    },
    g: (a, b, c, d, f, k) => {
      Y.vertexAttribPointer(a, b, c, !!d, f, k);
    },
    s: (a, b, c, d) => Y.viewport(a, b, c, d),
    U: function (a, b, c, d) {
      var f = document.createElement("canvas");
      f.width = b;
      f.height = c;
      b = f.getContext("2d");
      b.font = a + "px " + M(d);
      b.textBaseline = "middle";
      b.globalAlpha = 1;
      b.fillStyle = "white";
      window["gCanvas" + a] = f;
      window["gCanvasCtx" + a] = b;
    },
    K: function (a, b, c, d) {
      var f = new Image();
      f.onload = () => {
        D[c >> 2] = f.width;
        D[d >> 2] = f.height;
        Y.bindTexture(Y.TEXTURE_2D, W[a]);
        Y.texImage2D(Y.TEXTURE_2D, 0, Y.RGBA, Y.RGBA, Y.UNSIGNED_BYTE, f);
      };
      f.src = M(b);
    },
    T: function (a, b) {
      var c = window["gCanvas" + a];
      a = window["gCanvasCtx" + a];
      a.clearRect(0, 0, c.width, c.height);
      b = M(b);
      var d = a.measureText(b).width;
      a.fillText(b, 0, c.height / 2);
      Y.texImage2D(Y.TEXTURE_2D, 0, Y.RGBA, Y.RGBA, Y.UNSIGNED_BYTE, c);
      return d;
    }
  },
  Z = function () {
    function a(c) {
      Z = c.exports;
      ha = Z.pa;
      na();
      N = Z.ta;
      pa.unshift(Z.qa);
      va();
      return Z;
    }
    var b = {
      a: ub
    };
    ua();
    if (g.instantiateWasm) try {
      return g.instantiateWasm(b, a);
    } catch (c) {
      return n(`Module.instantiateWasm callback failed with error: ${c}`), !1;
    }
    Ba(b, function (c) {
      a(c.instance);
    });
    return {};
  }(),
  Za = a => (Za = Z.ra)(a),
  R = a => (R = Z.sa)(a),
  vb = g._main = (a, b) => (vb = g._main = Z.ua)(a, b),
  wb;
F = function xb() {
  wb || yb();
  wb || (F = xb);
};
function yb() {
  function a() {
    if (!wb && (wb = !0, g.calledRun = !0, !ia)) {
      Da(pa);
      Da(qa);
      if (g.onRuntimeInitialized) g.onRuntimeInitialized();
      if (zb) {
        var b = vb;
        try {
          var c = b(0, 0);
          w = c;
          ab(c);
        } catch (d) {
          $a(d);
        }
      }
      if (g.postRun) for ("function" == typeof g.postRun && (g.postRun = [g.postRun]); g.postRun.length;) b = g.postRun.shift(), ra.unshift(b);
      Da(ra);
    }
  }
  if (!(0 < E)) {
    if (g.preRun) for ("function" == typeof g.preRun && (g.preRun = [g.preRun]); g.preRun.length;) sa();
    Da(oa);
    0 < E || (g.setStatus ? (g.setStatus("Running..."), setTimeout(function () {
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
