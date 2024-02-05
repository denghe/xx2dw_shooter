var e;
e || (e = typeof Module !== 'undefined' ? Module : {});
var aa = Object.assign({}, e),
  h = (a, b) => {
    throw b;
  },
  k = "";
"undefined" != typeof document && document.currentScript && (k = document.currentScript.src);
k = k.startsWith("blob:") ? "" : k.substr(0, k.replace(/[?#].*/, "").lastIndexOf("/") + 1);
var ba = e.print || console.log.bind(console),
  q = e.printErr || console.error.bind(console);
Object.assign(e, aa);
aa = null;
e.quit && (h = e.quit);
var r;
e.wasmBinary && (r = e.wasmBinary);
"object" != typeof WebAssembly && t("no native wasm support detected");
var u,
  v = !1,
  ca,
  w,
  x,
  da,
  y,
  z,
  ea,
  A;
function fa() {
  var a = u.buffer;
  e.HEAP8 = ca = new Int8Array(a);
  e.HEAP16 = x = new Int16Array(a);
  e.HEAPU8 = w = new Uint8Array(a);
  e.HEAPU16 = da = new Uint16Array(a);
  e.HEAP32 = y = new Int32Array(a);
  e.HEAPU32 = z = new Uint32Array(a);
  e.HEAPF32 = ea = new Float32Array(a);
  e.HEAPF64 = A = new Float64Array(a);
}
var ha = [],
  ia = [],
  ja = [],
  ka = [];
function la() {
  var a = e.preRun.shift();
  ha.unshift(a);
}
var B = 0,
  C = null,
  E = null;
function t(a) {
  var _e$onAbort, _e;
  (_e$onAbort = (_e = e).onAbort) === null || _e$onAbort === void 0 || _e$onAbort.call(_e, a);
  a = "Aborted(" + a + ")";
  q(a);
  v = !0;
  throw new WebAssembly.RuntimeError(a + ". Build with -sASSERTIONS for more info.");
}
var ma = a => a.startsWith("data:application/octet-stream;base64,"),
  F;
F = "s8.wasm";
if (!ma(F)) {
  var na = F;
  F = e.locateFile ? e.locateFile(na, k) : k + na;
}
function oa(a) {
  if (a == F && r) return new Uint8Array(r);
  throw "both async and sync fetching of the wasm failed";
}
function pa(a) {
  return r || "function" != typeof fetch ? Promise.resolve().then(() => oa(a)) : fetch(a, {
    credentials: "same-origin"
  }).then(b => {
    if (!b.ok) throw `failed to load wasm binary file at '${a}'`;
    return b.arrayBuffer();
  }).catch(() => oa(a));
}
function qa(a, b, c) {
  return pa(a).then(d => WebAssembly.instantiate(d, b)).then(d => d).then(c, d => {
    q(`failed to asynchronously prepare wasm: ${d}`);
    t(d);
  });
}
function ra(a, b) {
  var c = F;
  r || "function" != typeof WebAssembly.instantiateStreaming || ma(c) || "function" != typeof fetch ? qa(c, a, b) : fetch(c, {
    credentials: "same-origin"
  }).then(d => WebAssembly.instantiateStreaming(d, a).then(b, function (g) {
    q(`wasm streaming compile failed: ${g}`);
    q("falling back to ArrayBuffer instantiation");
    return qa(c, a, b);
  }));
}
function sa(a) {
  this.name = "ExitStatus";
  this.message = `Program terminated with exit(${a})`;
  this.status = a;
}
var G = a => {
    for (; 0 < a.length;) a.shift()(e);
  },
  ta = e.noExitRuntime || !0,
  ua = new TextDecoder("utf8"),
  H = (a, b) => {
    if (!a) return "";
    b = a + b;
    for (var c = a; !(c >= b) && w[c];) ++c;
    return ua.decode(w.subarray(a, c));
  };
class va {
  constructor(a) {
    this.qa = a - 24;
  }
}
var wa = 0,
  xa = 0,
  I,
  J = 0,
  K = [];
function ya() {
  if (navigator.userActivation ? navigator.userActivation.isActive : J && za.Ga) for (var a = 0; a < K.length; ++a) {
    var b = K[a];
    K.splice(a, 1);
    --a;
    b.Ra.apply(null, b.La);
  }
}
var L = [];
function Aa(a) {
  var b = L[a];
  b.target.removeEventListener(b.ra, b.Ba, b.ya);
  L.splice(a, 1);
}
function Ba(a) {
  if (!a.target) return -4;
  if (a.za) a.Ba = function (c) {
    ++J;
    za = a;
    ya();
    a.Ca(c);
    ya();
    --J;
  }, a.target.addEventListener(a.ra, a.Ba, a.ya), L.push(a);else for (var b = 0; b < L.length; ++b) L[b].target == a.target && L[b].ra == a.ra && Aa(b--);
  return 0;
}
var za,
  Ca,
  M,
  Da = [0, document, window],
  N = a => {
    a = 2 < a ? H(a) : a;
    return Da[a] || document.querySelector(a);
  },
  O = (a, b, c) => {
    var d = w;
    if (!(0 < c)) return 0;
    var g = b;
    c = b + c - 1;
    for (var l = 0; l < a.length; ++l) {
      var f = a.charCodeAt(l);
      if (55296 <= f && 57343 >= f) {
        var n = a.charCodeAt(++l);
        f = 65536 + ((f & 1023) << 10) | n & 1023;
      }
      if (127 >= f) {
        if (b >= c) break;
        d[b++] = f;
      } else {
        if (2047 >= f) {
          if (b + 1 >= c) break;
          d[b++] = 192 | f >> 6;
        } else {
          if (65535 >= f) {
            if (b + 2 >= c) break;
            d[b++] = 224 | f >> 12;
          } else {
            if (b + 3 >= c) break;
            d[b++] = 240 | f >> 18;
            d[b++] = 128 | f >> 12 & 63;
          }
          d[b++] = 128 | f >> 6 & 63;
        }
        d[b++] = 128 | f & 63;
      }
    }
    d[b] = 0;
    return b - g;
  },
  Fa = (a, b, c, d, g, l) => {
    Ca || (Ca = Ea(176));
    a = {
      target: N(a),
      ra: l,
      za: d,
      Ca: f => {
        var n = Ca;
        A[n >> 3] = f.timeStamp;
        var m = n >> 2;
        y[m + 2] = f.location;
        y[m + 3] = f.ctrlKey;
        y[m + 4] = f.shiftKey;
        y[m + 5] = f.altKey;
        y[m + 6] = f.metaKey;
        y[m + 7] = f.repeat;
        y[m + 8] = f.charCode;
        y[m + 9] = f.keyCode;
        y[m + 10] = f.which;
        O(f.key || "", n + 44, 32);
        O(f.code || "", n + 76, 32);
        O(f.char || "", n + 108, 32);
        O(f.locale || "", n + 140, 32);
        I.get(d)(g, n, b) && f.preventDefault();
      },
      ya: c
    };
    return Ba(a);
  },
  Ga = (a, b, c, d, g, l) => {
    M || (M = Ea(72));
    a = N(a);
    return Ba({
      target: a,
      Ga: "mousemove" != l && "mouseenter" != l && "mouseleave" != l,
      ra: l,
      za: d,
      Ca: function () {
        let f = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : event;
        var n = a,
          m = M;
        A[m >> 3] = f.timeStamp;
        m >>= 2;
        y[m + 2] = f.screenX;
        y[m + 3] = f.screenY;
        y[m + 4] = f.clientX;
        y[m + 5] = f.clientY;
        y[m + 6] = f.ctrlKey;
        y[m + 7] = f.shiftKey;
        y[m + 8] = f.altKey;
        y[m + 9] = f.metaKey;
        x[2 * m + 20] = f.button;
        x[2 * m + 21] = f.buttons;
        y[m + 11] = f.movementX;
        y[m + 12] = f.movementY;
        n = 0 > Da.indexOf(n) ? n.getBoundingClientRect() : {
          left: 0,
          top: 0
        };
        y[m + 13] = f.clientX - n.left;
        y[m + 14] = f.clientY - n.top;
        I.get(d)(g, M, b) && f.preventDefault();
      },
      ya: c
    });
  },
  Ha = 1,
  P = [],
  Q = [],
  R = [],
  S = [],
  T = [],
  U = [],
  Ia = [],
  V = a => {
    for (var b = Ha++, c = a.length; c < b; c++) a[c] = null;
    return b;
  },
  Ka = (a, b) => {
    a.qa || (a.qa = a.getContext, a.getContext = function (d, g) {
      g = a.qa(d, g);
      return "webgl" == d == g instanceof WebGLRenderingContext ? g : null;
    });
    var c = a.getContext("webgl2", b);
    return c ? Ja(c, b) : 0;
  },
  Ja = (a, b) => {
    var c = V(Ia);
    b = {
      handle: c,
      attributes: b,
      version: b.Ja,
      Fa: a
    };
    a.canvas && (a.canvas.Ka = b);
    Ia[c] = b;
    return c;
  },
  La,
  Ma = ["default", "low-power", "high-performance"],
  Na = [null, [], []],
  Oa = () => {
    if ("object" == typeof crypto && "function" == typeof crypto.getRandomValues) return a => crypto.getRandomValues(a);
    t("initRandomDevice");
  },
  Pa = a => (Pa = Oa())(a),
  X = (a, b, c, d) => {
    for (var g = 0; g < a; g++) {
      var l = W[c](),
        f = l && V(d);
      l && (l.name = f, d[f] = l);
      y[b + 4 * g >> 2] = f;
    }
  },
  Qa = a => "]" == a.slice(-1) && a.lastIndexOf("["),
  Ra = a => {
    var b = W.Ha,
      c = b.sa[a];
    "number" == typeof c && (b.sa[a] = c = W.getUniformLocation(b, b.Da[a] + (0 < c ? `[${c}]` : "")));
    return c;
  },
  W,
  Sa = {
    e: (a, b, c, d) => {
      t(`Assertion failed: ${H(a)}, at: ` + [b ? H(b) : "unknown filename", c, d ? H(d) : "unknown function"]);
    },
    K: (a, b, c) => {
      var d = new va(a);
      z[d.qa + 16 >> 2] = 0;
      z[d.qa + 4 >> 2] = b;
      z[d.qa + 8 >> 2] = c;
      wa = a;
      xa++;
      throw wa;
    },
    N: () => 1,
    r: () => {
      t("");
    },
    P: () => performance.now(),
    ia: (a, b) => {
      function c(d) {
        I.get(a)(d, b) && requestAnimationFrame(c);
      }
      return requestAnimationFrame(c);
    },
    O: a => {
      var b = w.length;
      a >>>= 0;
      if (2147483648 < a) return !1;
      for (var c = 1; 4 >= c; c *= 2) {
        var d = b * (1 + .2 / c);
        d = Math.min(d, a + 100663296);
        var g = Math;
        d = Math.max(a, d);
        a: {
          g = (g.min.call(g, 2147483648, d + (65536 - d % 65536) % 65536) - u.buffer.byteLength + 65535) / 65536;
          try {
            u.grow(g);
            fa();
            var l = 1;
            break a;
          } catch (f) {}
          l = void 0;
        }
        if (l) return !0;
      }
      return !1;
    },
    ca: (a, b, c) => {
      a = N(a);
      if (!a) return -4;
      a.width = b;
      a.height = c;
      return 0;
    },
    ha: (a, b, c, d) => Fa(a, b, c, d, 2, "keydown"),
    ga: (a, b, c, d) => Fa(a, b, c, d, 3, "keyup"),
    fa: (a, b, c, d) => Ga(a, b, c, d, 5, "mousedown"),
    da: (a, b, c, d) => Ga(a, b, c, d, 8, "mousemove"),
    ea: (a, b, c, d) => Ga(a, b, c, d, 6, "mouseup"),
    ba: (a, b) => {
      b >>= 2;
      b = {
        alpha: !!y[b],
        depth: !!y[b + 1],
        stencil: !!y[b + 2],
        antialias: !!y[b + 3],
        premultipliedAlpha: !!y[b + 4],
        preserveDrawingBuffer: !!y[b + 5],
        powerPreference: Ma[y[b + 6]],
        failIfMajorPerformanceCaveat: !!y[b + 7],
        Ja: y[b + 8],
        Oa: y[b + 9],
        Na: y[b + 10],
        Ia: y[b + 11],
        Pa: y[b + 12],
        Qa: y[b + 13]
      };
      a = N(a);
      return !a || b.Ia ? 0 : Ka(a, b);
    },
    aa: a => {
      var _La;
      La = Ia[a];
      e.Ma = W = (_La = La) === null || _La === void 0 ? void 0 : _La.Fa;
      return !a || W ? 0 : -5;
    },
    u: (a, b, c, d) => {
      for (var g = 0, l = 0; l < c; l++) {
        var f = z[b >> 2],
          n = z[b + 4 >> 2];
        b += 8;
        for (var m = 0; m < n; m++) {
          var p = w[f + m],
            D = Na[a];
          if (0 === p || 10 === p) {
            for (p = 0; D[p] && !(NaN <= p);) ++p;
            p = ua.decode(D.buffer ? D.subarray(0, p) : new Uint8Array(D.slice(0, p)));
            (1 === a ? ba : q)(p);
            D.length = 0;
          } else D.push(p);
        }
        g += n;
      }
      z[d >> 2] = g;
      return 0;
    },
    M: (a, b) => {
      Pa(w.subarray(a, a + b));
      return 0;
    },
    _: function (a) {
      W.activeTexture(a);
    },
    z: (a, b) => {
      W.attachShader(Q[a], T[b]);
    },
    a: (a, b) => {
      35051 == a ? W.Aa = b : 35052 == a && (W.wa = b);
      W.bindBuffer(a, P[b]);
    },
    I: (a, b) => {
      W.bindFramebuffer(a, R[b]);
    },
    c: (a, b) => {
      W.bindTexture(a, S[b]);
    },
    j: a => {
      W.bindVertexArray(U[a]);
    },
    o: function (a) {
      W.blendEquation(a);
    },
    p: function (a, b) {
      W.blendFunc(a, b);
    },
    i: (a, b, c, d) => {
      c && b ? W.bufferData(a, w, d, c, b) : W.bufferData(a, b, d);
    },
    G: function (a) {
      W.clear(a);
    },
    H: function (a, b, c, d) {
      W.clearColor(a, b, c, d);
    },
    T: a => {
      W.compileShader(T[a]);
    },
    Y: () => {
      var a = V(Q),
        b = W.createProgram();
      b.name = a;
      b.va = b.ta = b.ua = 0;
      b.xa = 1;
      Q[a] = b;
      return a;
    },
    V: a => {
      var b = V(T);
      T[b] = W.createShader(a);
      return b;
    },
    n: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2],
          g = P[d];
        g && (W.deleteBuffer(g), g.name = 0, P[d] = null, d == W.Aa && (W.Aa = 0), d == W.wa && (W.wa = 0));
      }
    },
    w: (a, b) => {
      for (var c = 0; c < a; ++c) {
        var d = y[b + 4 * c >> 2],
          g = R[d];
        g && (W.deleteFramebuffer(g), g.name = 0, R[d] = null);
      }
    },
    k: a => {
      if (a) {
        var b = Q[a];
        b && (W.deleteProgram(b), b.name = 0, Q[a] = null);
      }
    },
    d: a => {
      if (a) {
        var b = T[a];
        b && (W.deleteShader(b), T[a] = null);
      }
    },
    ka: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2],
          g = S[d];
        g && (W.deleteTexture(g), g.name = 0, S[d] = null);
      }
    },
    E: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2];
        W.deleteVertexArray(U[d]);
        U[d] = null;
      }
    },
    F: function (a) {
      W.disable(a);
    },
    v: (a, b, c, d) => {
      W.drawArraysInstanced(a, b, c, d);
    },
    B: (a, b, c, d) => {
      W.drawElements(a, b, c, d);
    },
    $: function (a) {
      W.enable(a);
    },
    f: a => {
      W.enableVertexAttribArray(a);
    },
    ja: (a, b, c, d, g) => {
      W.framebufferTexture2D(a, b, c, S[d], g);
    },
    m: (a, b) => {
      X(a, b, "createBuffer", P);
    },
    R: (a, b) => {
      X(a, b, "createFramebuffer", R);
    },
    q: (a, b) => {
      X(a, b, "createTexture", S);
    },
    A: function (a, b) {
      X(a, b, "createVertexArray", U);
    },
    h: (a, b) => W.getAttribLocation(Q[a], H(b)),
    W: (a, b, c, d) => {
      a = W.getProgramInfoLog(Q[a]);
      b = 0 < b && d ? O(a, d, b) : 0;
      c && (y[c >> 2] = b);
    },
    y: (a, b, c) => {
      if (c && !(a >= Ha)) if (a = Q[a], 35716 == b) y[c >> 2] = W.getProgramInfoLog(a).length + 1;else if (35719 == b) {
        if (!a.va) for (b = 0; b < W.getProgramParameter(a, 35718); ++b) a.va = Math.max(a.va, W.getActiveUniform(a, b).name.length + 1);
        y[c >> 2] = a.va;
      } else if (35722 == b) {
        if (!a.ta) for (b = 0; b < W.getProgramParameter(a, 35721); ++b) a.ta = Math.max(a.ta, W.getActiveAttrib(a, b).name.length + 1);
        y[c >> 2] = a.ta;
      } else if (35381 == b) {
        if (!a.ua) for (b = 0; b < W.getProgramParameter(a, 35382); ++b) a.ua = Math.max(a.ua, W.getActiveUniformBlockName(a, b).length + 1);
        y[c >> 2] = a.ua;
      } else y[c >> 2] = W.getProgramParameter(a, b);
    },
    S: (a, b, c, d) => {
      a = W.getShaderInfoLog(T[a]);
      b = 0 < b && d ? O(a, d, b) : 0;
      c && (y[c >> 2] = b);
    },
    x: (a, b, c) => {
      c && (35716 == b ? (a = W.getShaderInfoLog(T[a]), y[c >> 2] = a ? a.length + 1 : 0) : 35720 == b ? (a = W.getShaderSource(T[a]), y[c >> 2] = a ? a.length + 1 : 0) : y[c >> 2] = W.getShaderParameter(T[a], b));
    },
    s: (a, b) => {
      b = H(b);
      if (a = Q[a]) {
        var c = a,
          d = c.sa,
          g = c.Ea,
          l;
        if (!d) for (c.sa = d = {}, c.Da = {}, l = 0; l < W.getProgramParameter(c, 35718); ++l) {
          var f = W.getActiveUniform(c, l);
          var n = f.name;
          f = f.size;
          var m = Qa(n);
          m = 0 < m ? n.slice(0, m) : n;
          var p = c.xa;
          c.xa += f;
          g[m] = [f, p];
          for (n = 0; n < f; ++n) d[p] = n, c.Da[p++] = m;
        }
        c = a.sa;
        d = 0;
        g = b;
        l = Qa(b);
        0 < l && (d = parseInt(b.slice(l + 1)) >>> 0, g = b.slice(0, l));
        if ((g = a.Ea[g]) && d < g[0] && (d += g[1], c[d] = c[d] || W.getUniformLocation(a, b))) return d;
      }
      return -1;
    },
    X: a => {
      a = Q[a];
      W.linkProgram(a);
      a.sa = 0;
      a.Ea = {};
    },
    U: (a, b, c, d) => {
      for (var g = "", l = 0; l < b; ++l) g += H(z[c + 4 * l >> 2], d ? z[d + 4 * l >> 2] : void 0);
      W.shaderSource(T[a], g);
    },
    J: (a, b, c, d, g, l, f, n, m) => {
      if (W.wa) W.texImage2D(a, b, c, d, g, l, f, n, m);else if (m) {
        var p = n - 5120;
        p = 0 == p ? ca : 1 == p ? w : 2 == p ? x : 4 == p ? y : 6 == p ? ea : 5 == p || 28922 == p || 28520 == p || 30779 == p || 30782 == p ? z : da;
        W.texImage2D(a, b, c, d, g, l, f, n, p, m >> 31 - Math.clz32(p.BYTES_PER_ELEMENT));
      } else W.texImage2D(a, b, c, d, g, l, f, n, null);
    },
    b: function (a, b, c) {
      W.texParameteri(a, b, c);
    },
    Z: (a, b) => {
      W.uniform1i(Ra(a), b);
    },
    C: (a, b, c) => {
      W.uniform2f(Ra(a), b, c);
    },
    D: a => {
      a = Q[a];
      W.useProgram(a);
      W.Ha = a;
    },
    l: (a, b) => {
      W.vertexAttribDivisor(a, b);
    },
    g: (a, b, c, d, g, l) => {
      W.vertexAttribPointer(a, b, c, !!d, g, l);
    },
    t: function (a, b, c, d) {
      W.viewport(a, b, c, d);
    },
    Q: function (a, b, c, d) {
      var g = document.createElement("canvas");
      g.width = b;
      g.height = c;
      b = g.getContext("2d");
      b.font = a + "px " + H(d);
      b.textBaseline = "middle";
      b.globalAlpha = 1;
      b.fillStyle = "white";
      window["gCanvas" + a] = g;
      window["gCanvasCtx" + a] = b;
    },
    L: function (a, b) {
      var c = window["gCanvas" + a];
      a = window["gCanvasCtx" + a];
      a.clearRect(0, 0, c.width, c.height);
      b = H(b);
      var d = a.measureText(b).width;
      a.fillText(b, 0, c.height / 2);
      W.texImage2D(W.TEXTURE_2D, 0, W.RGBA, W.RGBA, W.UNSIGNED_BYTE, c);
      return d;
    }
  },
  Y = function (_e$monitorRunDependen2, _e3) {
    function a(c) {
      var _e$monitorRunDependen, _e2;
      Y = c.exports;
      u = Y.la;
      fa();
      I = Y.oa;
      ia.unshift(Y.ma);
      B--;
      (_e$monitorRunDependen = (_e2 = e).monitorRunDependencies) === null || _e$monitorRunDependen === void 0 || _e$monitorRunDependen.call(_e2, B);
      0 == B && (null !== C && (clearInterval(C), C = null), E && (c = E, E = null, c()));
      return Y;
    }
    var b = {
      a: Sa
    };
    B++;
    (_e$monitorRunDependen2 = (_e3 = e).monitorRunDependencies) === null || _e$monitorRunDependen2 === void 0 || _e$monitorRunDependen2.call(_e3, B);
    if (e.instantiateWasm) try {
      return e.instantiateWasm(b, a);
    } catch (c) {
      return q(`Module.instantiateWasm callback failed with error: ${c}`), !1;
    }
    ra(b, function (c) {
      a(c.instance);
    });
    return {};
  }(),
  Ea = a => (Ea = Y.na)(a),
  Ta = e._main = (a, b) => (Ta = e._main = Y.pa)(a, b),
  Z;
E = function Ua() {
  Z || Va();
  Z || (E = Ua);
};
function Va() {
  function a() {
    if (!Z && (Z = !0, e.calledRun = !0, !v)) {
      G(ia);
      G(ja);
      if (e.onRuntimeInitialized) e.onRuntimeInitialized();
      if (Wa) {
        var b = Ta;
        try {
          var _e$onExit, _e4;
          var c = b(0, 0);
          ta || ((_e$onExit = (_e4 = e).onExit) !== null && _e$onExit !== void 0 && _e$onExit.call(_e4, c), v = !0);
          h(c, new sa(c));
        } catch (d) {
          d instanceof sa || "unwind" == d || h(1, d);
        }
      }
      if (e.postRun) for ("function" == typeof e.postRun && (e.postRun = [e.postRun]); e.postRun.length;) b = e.postRun.shift(), ka.unshift(b);
      G(ka);
    }
  }
  if (!(0 < B)) {
    if (e.preRun) for ("function" == typeof e.preRun && (e.preRun = [e.preRun]); e.preRun.length;) la();
    G(ha);
    0 < B || (e.setStatus ? (e.setStatus("Running..."), setTimeout(function () {
      setTimeout(function () {
        e.setStatus("");
      }, 1);
      a();
    }, 1)) : a());
  }
}
if (e.preInit) for ("function" == typeof e.preInit && (e.preInit = [e.preInit]); 0 < e.preInit.length;) e.preInit.pop()();
var Wa = !0;
e.noInitialRun && (Wa = !1);
Va();
