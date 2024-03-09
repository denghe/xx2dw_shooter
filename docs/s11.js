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
  D = null;
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
F = "s11.wasm";
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
  return pa(a).then(d => WebAssembly.instantiate(d, b)).then(c, d => {
    q(`failed to asynchronously prepare wasm: ${d}`);
    t(d);
  });
}
function ra(a, b) {
  var c = F;
  r || "function" != typeof WebAssembly.instantiateStreaming || ma(c) || "function" != typeof fetch ? qa(c, a, b) : fetch(c, {
    credentials: "same-origin"
  }).then(d => WebAssembly.instantiateStreaming(d, a).then(b, function (f) {
    q(`wasm streaming compile failed: ${f}`);
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
    this.ra = a - 24;
  }
}
var wa = 0,
  xa = 0,
  I,
  J = 0,
  K = [];
function ya() {
  if (navigator.userActivation ? navigator.userActivation.isActive : J && za.Ha) for (var a = 0; a < K.length; ++a) {
    var b = K[a];
    K.splice(a, 1);
    --a;
    b.Sa(...b.Ma);
  }
}
var L = [];
function Aa(a) {
  var b = L[a];
  b.target.removeEventListener(b.sa, b.Ca, b.za);
  L.splice(a, 1);
}
function Ba(a) {
  if (!a.target) return -4;
  if (a.Aa) a.Ca = function (c) {
    ++J;
    za = a;
    ya();
    a.Da(c);
    ya();
    --J;
  }, a.target.addEventListener(a.sa, a.Ca, a.za), L.push(a);else for (var b = 0; b < L.length; ++b) L[b].target == a.target && L[b].sa == a.sa && Aa(b--);
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
    var f = b;
    c = b + c - 1;
    for (var l = 0; l < a.length; ++l) {
      var g = a.charCodeAt(l);
      if (55296 <= g && 57343 >= g) {
        var n = a.charCodeAt(++l);
        g = 65536 + ((g & 1023) << 10) | n & 1023;
      }
      if (127 >= g) {
        if (b >= c) break;
        d[b++] = g;
      } else {
        if (2047 >= g) {
          if (b + 1 >= c) break;
          d[b++] = 192 | g >> 6;
        } else {
          if (65535 >= g) {
            if (b + 2 >= c) break;
            d[b++] = 224 | g >> 12;
          } else {
            if (b + 3 >= c) break;
            d[b++] = 240 | g >> 18;
            d[b++] = 128 | g >> 12 & 63;
          }
          d[b++] = 128 | g >> 6 & 63;
        }
        d[b++] = 128 | g & 63;
      }
    }
    d[b] = 0;
    return b - f;
  },
  Fa = (a, b, c, d, f, l) => {
    Ca || (Ca = Ea(176));
    a = {
      target: N(a),
      sa: l,
      Aa: d,
      Da: g => {
        var n = Ca;
        A[n >> 3] = g.timeStamp;
        var m = n >> 2;
        y[m + 2] = g.location;
        y[m + 3] = g.ctrlKey;
        y[m + 4] = g.shiftKey;
        y[m + 5] = g.altKey;
        y[m + 6] = g.metaKey;
        y[m + 7] = g.repeat;
        y[m + 8] = g.charCode;
        y[m + 9] = g.keyCode;
        y[m + 10] = g.which;
        O(g.key || "", n + 44, 32);
        O(g.code || "", n + 76, 32);
        O(g.char || "", n + 108, 32);
        O(g.locale || "", n + 140, 32);
        I.get(d)(f, n, b) && g.preventDefault();
      },
      za: c
    };
    return Ba(a);
  },
  Ga = (a, b, c, d, f, l) => {
    M || (M = Ea(72));
    a = N(a);
    return Ba({
      target: a,
      Ha: "mousemove" != l && "mouseenter" != l && "mouseleave" != l,
      sa: l,
      Aa: d,
      Da: function () {
        let g = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : event;
        var n = a,
          m = M;
        A[m >> 3] = g.timeStamp;
        m >>= 2;
        y[m + 2] = g.screenX;
        y[m + 3] = g.screenY;
        y[m + 4] = g.clientX;
        y[m + 5] = g.clientY;
        y[m + 6] = g.ctrlKey;
        y[m + 7] = g.shiftKey;
        y[m + 8] = g.altKey;
        y[m + 9] = g.metaKey;
        x[2 * m + 20] = g.button;
        x[2 * m + 21] = g.buttons;
        y[m + 11] = g.movementX;
        y[m + 12] = g.movementY;
        n = 0 > Da.indexOf(n) ? n.getBoundingClientRect() : {
          left: 0,
          top: 0
        };
        y[m + 13] = g.clientX - n.left;
        y[m + 14] = g.clientY - n.top;
        I.get(d)(f, M, b) && g.preventDefault();
      },
      za: c
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
  X = (a, b, c, d) => {
    for (var f = 0; f < a; f++) {
      var l = W[c](),
        g = l && V(d);
      l && (l.name = g, d[g] = l);
      y[b + 4 * f >> 2] = g;
    }
  },
  Ka = (a, b) => {
    a.ra || (a.ra = a.getContext, a.getContext = function (d, f) {
      f = a.ra(d, f);
      return "webgl" == d == f instanceof WebGLRenderingContext ? f : null;
    });
    var c = a.getContext("webgl2", b);
    return c ? Ja(c, b) : 0;
  },
  Ja = (a, b) => {
    var c = V(Ia);
    b = {
      handle: c,
      attributes: b,
      version: b.Ka,
      Ga: a
    };
    a.canvas && (a.canvas.La = b);
    Ia[c] = b;
    return c;
  },
  La,
  Ma = ["default", "low-power", "high-performance"],
  Na = [null, [], []],
  Oa = a => "]" == a.slice(-1) && a.lastIndexOf("["),
  Pa = a => {
    var b = W.Ia,
      c = b.ta[a];
    "number" == typeof c && (b.ta[a] = c = W.getUniformLocation(b, b.Ea[a] + (0 < c ? `[${c}]` : "")));
    return c;
  },
  W,
  Qa = {
    e: (a, b, c, d) => {
      t(`Assertion failed: ${H(a)}, at: ` + [b ? H(b) : "unknown filename", c, d ? H(d) : "unknown function"]);
    },
    r: (a, b, c) => {
      var d = new va(a);
      z[d.ra + 16 >> 2] = 0;
      z[d.ra + 4 >> 2] = b;
      z[d.ra + 8 >> 2] = c;
      wa = a;
      xa++;
      throw wa;
    },
    K: () => 1,
    s: () => {
      t("");
    },
    L: () => Date.now(),
    N: () => performance.now(),
    ia: (a, b) => {
      function c(d) {
        I.get(a)(d, b) && requestAnimationFrame(c);
      }
      return requestAnimationFrame(c);
    },
    M: a => {
      var b = w.length;
      a >>>= 0;
      if (2147483648 < a) return !1;
      for (var c = 1; 4 >= c; c *= 2) {
        var d = b * (1 + .2 / c);
        d = Math.min(d, a + 100663296);
        var f = Math;
        d = Math.max(a, d);
        a: {
          f = (f.min.call(f, 2147483648, d + (65536 - d % 65536) % 65536) - u.buffer.byteLength + 65535) / 65536;
          try {
            u.grow(f);
            fa();
            var l = 1;
            break a;
          } catch (g) {}
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
        Ka: y[b + 8],
        Pa: y[b + 9],
        Oa: y[b + 10],
        Ja: y[b + 11],
        Qa: y[b + 12],
        Ra: y[b + 13]
      };
      a = N(a);
      return !a || b.Ja ? 0 : Ka(a, b);
    },
    aa: a => {
      var _La;
      La = Ia[a];
      e.Na = W = (_La = La) === null || _La === void 0 ? void 0 : _La.Ga;
      return !a || W ? 0 : -5;
    },
    w: (a, b, c, d) => {
      for (var f = 0, l = 0; l < c; l++) {
        var g = z[b >> 2],
          n = z[b + 4 >> 2];
        b += 8;
        for (var m = 0; m < n; m++) {
          var p = w[g + m],
            E = Na[a];
          if (0 === p || 10 === p) {
            for (p = 0; E[p] && !(NaN <= p);) ++p;
            p = ua.decode(E.buffer ? E.subarray(0, p) : new Uint8Array(E.slice(0, p)));
            (1 === a ? ba : q)(p);
            E.length = 0;
          } else E.push(p);
        }
        f += n;
      }
      z[d >> 2] = f;
      return 0;
    },
    _: a => W.activeTexture(a),
    A: (a, b) => {
      W.attachShader(Q[a], T[b]);
    },
    c: (a, b) => {
      35051 == a ? W.Ba = b : 35052 == a && (W.xa = b);
      W.bindBuffer(a, P[b]);
    },
    J: (a, b) => {
      W.bindFramebuffer(a, R[b]);
    },
    b: (a, b) => {
      W.bindTexture(a, S[b]);
    },
    i: a => {
      W.bindVertexArray(U[a]);
    },
    p: a => W.blendEquation(a),
    q: (a, b) => W.blendFunc(a, b),
    k: (a, b, c, d) => {
      c && b ? W.bufferData(a, w, d, c, b) : W.bufferData(a, b, d);
    },
    H: a => W.clear(a),
    I: (a, b, c, d) => W.clearColor(a, b, c, d),
    R: a => {
      W.compileShader(T[a]);
    },
    W: () => {
      var a = V(Q),
        b = W.createProgram();
      b.name = a;
      b.wa = b.ua = b.va = 0;
      b.ya = 1;
      Q[a] = b;
      return a;
    },
    T: a => {
      var b = V(T);
      T[b] = W.createShader(a);
      return b;
    },
    o: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2],
          f = P[d];
        f && (W.deleteBuffer(f), f.name = 0, P[d] = null, d == W.Ba && (W.Ba = 0), d == W.xa && (W.xa = 0));
      }
    },
    x: (a, b) => {
      for (var c = 0; c < a; ++c) {
        var d = y[b + 4 * c >> 2],
          f = R[d];
        f && (W.deleteFramebuffer(f), f.name = 0, R[d] = null);
      }
    },
    j: a => {
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
    la: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2],
          f = S[d];
        f && (W.deleteTexture(f), f.name = 0, S[d] = null);
      }
    },
    E: (a, b) => {
      for (var c = 0; c < a; c++) {
        var d = y[b + 4 * c >> 2];
        W.deleteVertexArray(U[d]);
        U[d] = null;
      }
    },
    G: a => W.disable(a),
    F: (a, b, c, d) => {
      W.drawArraysInstanced(a, b, c, d);
    },
    Y: (a, b, c, d) => {
      W.drawElements(a, b, c, d);
    },
    $: a => W.enable(a),
    f: a => {
      W.enableVertexAttribArray(a);
    },
    ka: (a, b, c, d, f) => {
      W.framebufferTexture2D(a, b, c, S[d], f);
    },
    n: (a, b) => {
      X(a, b, "createBuffer", P);
    },
    P: (a, b) => {
      X(a, b, "createFramebuffer", R);
    },
    l: (a, b) => {
      X(a, b, "createTexture", S);
    },
    B: (a, b) => {
      X(a, b, "createVertexArray", U);
    },
    h: (a, b) => W.getAttribLocation(Q[a], H(b)),
    U: (a, b, c, d) => {
      a = W.getProgramInfoLog(Q[a]);
      b = 0 < b && d ? O(a, d, b) : 0;
      c && (y[c >> 2] = b);
    },
    z: (a, b, c) => {
      if (c && !(a >= Ha)) if (a = Q[a], 35716 == b) y[c >> 2] = W.getProgramInfoLog(a).length + 1;else if (35719 == b) {
        if (!a.wa) for (b = 0; b < W.getProgramParameter(a, 35718); ++b) a.wa = Math.max(a.wa, W.getActiveUniform(a, b).name.length + 1);
        y[c >> 2] = a.wa;
      } else if (35722 == b) {
        if (!a.ua) for (b = 0; b < W.getProgramParameter(a, 35721); ++b) a.ua = Math.max(a.ua, W.getActiveAttrib(a, b).name.length + 1);
        y[c >> 2] = a.ua;
      } else if (35381 == b) {
        if (!a.va) for (b = 0; b < W.getProgramParameter(a, 35382); ++b) a.va = Math.max(a.va, W.getActiveUniformBlockName(a, b).length + 1);
        y[c >> 2] = a.va;
      } else y[c >> 2] = W.getProgramParameter(a, b);
    },
    Q: (a, b, c, d) => {
      a = W.getShaderInfoLog(T[a]);
      b = 0 < b && d ? O(a, d, b) : 0;
      c && (y[c >> 2] = b);
    },
    y: (a, b, c) => {
      c && (35716 == b ? (a = W.getShaderInfoLog(T[a]), y[c >> 2] = a ? a.length + 1 : 0) : 35720 == b ? (a = W.getShaderSource(T[a]), y[c >> 2] = a ? a.length + 1 : 0) : y[c >> 2] = W.getShaderParameter(T[a], b));
    },
    t: (a, b) => {
      b = H(b);
      if (a = Q[a]) {
        var c = a,
          d = c.ta,
          f = c.Fa,
          l;
        if (!d) for (c.ta = d = {}, c.Ea = {}, l = 0; l < W.getProgramParameter(c, 35718); ++l) {
          var g = W.getActiveUniform(c, l);
          var n = g.name;
          g = g.size;
          var m = Oa(n);
          m = 0 < m ? n.slice(0, m) : n;
          var p = c.ya;
          c.ya += g;
          f[m] = [g, p];
          for (n = 0; n < g; ++n) d[p] = n, c.Ea[p++] = m;
        }
        c = a.ta;
        d = 0;
        f = b;
        l = Oa(b);
        0 < l && (d = parseInt(b.slice(l + 1)) >>> 0, f = b.slice(0, l));
        if ((f = a.Fa[f]) && d < f[0] && (d += f[1], c[d] = c[d] || W.getUniformLocation(a, b))) return d;
      }
      return -1;
    },
    V: a => {
      a = Q[a];
      W.linkProgram(a);
      a.ta = 0;
      a.Fa = {};
    },
    S: (a, b, c, d) => {
      for (var f = "", l = 0; l < b; ++l) f += H(z[c + 4 * l >> 2], d ? z[d + 4 * l >> 2] : void 0);
      W.shaderSource(T[a], f);
    },
    v: (a, b, c, d, f, l, g, n, m) => {
      if (W.xa) W.texImage2D(a, b, c, d, f, l, g, n, m);else if (m) {
        var p = n - 5120;
        p = 0 == p ? ca : 1 == p ? w : 2 == p ? x : 4 == p ? y : 6 == p ? ea : 5 == p || 28922 == p || 28520 == p || 30779 == p || 30782 == p ? z : da;
        W.texImage2D(a, b, c, d, f, l, g, n, p, m >>> 31 - Math.clz32(p.BYTES_PER_ELEMENT));
      } else W.texImage2D(a, b, c, d, f, l, g, n, null);
    },
    a: (a, b, c) => W.texParameteri(a, b, c),
    Z: (a, b) => {
      W.uniform1i(Pa(a), b);
    },
    C: (a, b, c) => {
      W.uniform2f(Pa(a), b, c);
    },
    D: a => {
      a = Q[a];
      W.useProgram(a);
      W.Ia = a;
    },
    m: (a, b) => {
      W.vertexAttribDivisor(a, b);
    },
    g: (a, b, c, d, f, l) => {
      W.vertexAttribPointer(a, b, c, !!d, f, l);
    },
    u: (a, b, c, d) => W.viewport(a, b, c, d),
    O: function (a, b, c, d) {
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
    ja: function (a, b, c, d) {
      var f = new Image();
      f.onload = () => {
        z[c >> 2] = f.width;
        z[d >> 2] = f.height;
        W.bindTexture(W.TEXTURE_2D, S[a]);
        W.texImage2D(W.TEXTURE_2D, 0, W.RGBA, W.RGBA, W.UNSIGNED_BYTE, f);
      };
      f.src = H(b);
    },
    X: function (a, b) {
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
      u = Y.ma;
      fa();
      I = Y.pa;
      ia.unshift(Y.na);
      B--;
      (_e$monitorRunDependen = (_e2 = e).monitorRunDependencies) === null || _e$monitorRunDependen === void 0 || _e$monitorRunDependen.call(_e2, B);
      0 == B && (null !== C && (clearInterval(C), C = null), D && (c = D, D = null, c()));
      return Y;
    }
    var b = {
      a: Qa
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
  Ea = a => (Ea = Y.oa)(a),
  Ra = e._main = (a, b) => (Ra = e._main = Y.qa)(a, b),
  Z;
D = function Sa() {
  Z || Ta();
  Z || (D = Sa);
};
function Ta() {
  function a() {
    if (!Z && (Z = !0, e.calledRun = !0, !v)) {
      G(ia);
      G(ja);
      if (e.onRuntimeInitialized) e.onRuntimeInitialized();
      if (Ua) {
        var b = Ra;
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
var Ua = !0;
e.noInitialRun && (Ua = !1);
Ta();
