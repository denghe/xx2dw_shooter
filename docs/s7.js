var g;g||(g=typeof Module !== 'undefined' ? Module : {});var da=Object.assign({},g),ea=(a,b)=>{throw b;},k="";"undefined"!=typeof document&&document.currentScript&&(k=document.currentScript.src);k=0!==k.indexOf("blob:")?k.substr(0,k.replace(/[?#].*/,"").lastIndexOf("/")+1):"";var l=g.printErr||console.error.bind(console);Object.assign(g,da);da=null;g.quit&&(ea=g.quit);var q;g.wasmBinary&&(q=g.wasmBinary);var noExitRuntime=g.noExitRuntime||!0;"object"!=typeof WebAssembly&&v("no native wasm support detected");
var fa,w,ha=!1,x,ia,y,ja,A,C,D,ka,ma;function na(){var a=fa.buffer;g.HEAP8=ia=new Int8Array(a);g.HEAP16=ja=new Int16Array(a);g.HEAP32=C=new Int32Array(a);g.HEAPU8=y=new Uint8Array(a);g.HEAPU16=A=new Uint16Array(a);g.HEAPU32=D=new Uint32Array(a);g.HEAPF32=ka=new Float32Array(a);g.HEAPF64=ma=new Float64Array(a)}var E,oa=[],pa=[],qa=[],ra=[],sa=[];function ta(){var a=g.preRun.shift();oa.unshift(a)}var F=0,ua=null,J=null;function va(){F++;g.monitorRunDependencies&&g.monitorRunDependencies(F)}
function wa(){F--;g.monitorRunDependencies&&g.monitorRunDependencies(F);if(0==F&&(null!==ua&&(clearInterval(ua),ua=null),J)){var a=J;J=null;a()}}function v(a){if(g.onAbort)g.onAbort(a);a="Aborted("+a+")";l(a);ha=!0;x=1;throw new WebAssembly.RuntimeError(a+". Build with -sASSERTIONS for more info.");}function xa(a){return a.startsWith("data:application/octet-stream;base64,")}var M;M="s7.wasm";if(!xa(M)){var ya=M;M=g.locateFile?g.locateFile(ya,k):k+ya}
function za(a){if(a==M&&q)return new Uint8Array(q);throw"both async and sync fetching of the wasm failed";}function Aa(a){return q||"function"!=typeof fetch?Promise.resolve().then(()=>za(a)):fetch(a,{credentials:"same-origin"}).then(b=>{if(!b.ok)throw"failed to load wasm binary file at '"+a+"'";return b.arrayBuffer()}).catch(()=>za(a))}function Ba(a,b,c){return Aa(a).then(d=>WebAssembly.instantiate(d,b)).then(d=>d).then(c,d=>{l("failed to asynchronously prepare wasm: "+d);v(d)})}
function Ca(a,b){var c=M;q||"function"!=typeof WebAssembly.instantiateStreaming||xa(c)||"function"!=typeof fetch?Ba(c,a,b):fetch(c,{credentials:"same-origin"}).then(d=>WebAssembly.instantiateStreaming(d,a).then(b,function(f){l("wasm streaming compile failed: "+f);l("falling back to ArrayBuffer instantiation");return Ba(c,a,b)}))}function Da(a){this.name="ExitStatus";this.message=`Program terminated with exit(${a})`;this.status=a}
var Ea=a=>{for(;0<a.length;)a.shift()(g)},Fa=new TextDecoder("utf8"),N=(a,b)=>{if(!a)return"";b=a+b;for(var c=a;!(c>=b)&&y[c];)++c;return Fa.decode(y.subarray(a,c))};function Ga(a){this.ua=a-24;this.Va=function(b){D[this.ua+4>>2]=b};this.Ua=function(b){D[this.ua+8>>2]=b};this.Sa=function(b,c){this.Ta();this.Va(b);this.Ua(c)};this.Ta=function(){D[this.ua+16>>2]=0}}var Ha=0,Ia=0,Ja=0;function Ka(){for(var a=O.length-1;0<=a;--a)La(a);O=[];Ma=[]}var Ma=[];
function Na(){if(navigator.userActivation?navigator.userActivation.isActive:Ja&&Oa.Ea)for(var a=0;a<Ma.length;++a){var b=Ma[a];Ma.splice(a,1);--a;b.cb.apply(null,b.Xa)}}var O=[];function La(a){var b=O[a];b.target.removeEventListener(b.wa,b.Oa,b.Da);O.splice(a,1)}
function Pa(a){function b(d){++Ja;Oa=a;Na();a.Ia(d);Na();--Ja}if(!a.target)return-4;if(a.Fa)a.Oa=b,a.target.addEventListener(a.wa,b,a.Da),O.push(a),Qa||(ra.push(Ka),Qa=!0);else for(var c=0;c<O.length;++c)O[c].target==a.target&&O[c].wa==a.wa&&La(c--);return 0}var Qa,Oa,Ra,Sa,Ta=[0,document,window];function Ua(a){a=2<a?N(a):a;return Ta[a]||document.querySelector(a)}
var Q=(a,b,c)=>{var d=y;if(!(0<c))return 0;var f=b;c=b+c-1;for(var m=0;m<a.length;++m){var e=a.charCodeAt(m);if(55296<=e&&57343>=e){var n=a.charCodeAt(++m);e=65536+((e&1023)<<10)|n&1023}if(127>=e){if(b>=c)break;d[b++]=e}else{if(2047>=e){if(b+1>=c)break;d[b++]=192|e>>6}else{if(65535>=e){if(b+2>=c)break;d[b++]=224|e>>12}else{if(b+3>=c)break;d[b++]=240|e>>18;d[b++]=128|e>>12&63}d[b++]=128|e>>6&63}d[b++]=128|e&63}}d[b]=0;return b-f};
function Va(a,b,c,d,f,m){Ra||(Ra=R(176));a={target:Ua(a),Ea:!0,wa:m,Fa:d,Ia:function(e){var n=Ra;ma[n>>3]=e.timeStamp;var h=n>>2;C[h+2]=e.location;C[h+3]=e.ctrlKey;C[h+4]=e.shiftKey;C[h+5]=e.altKey;C[h+6]=e.metaKey;C[h+7]=e.repeat;C[h+8]=e.charCode;C[h+9]=e.keyCode;C[h+10]=e.which;Q(e.key||"",n+44,32);Q(e.code||"",n+76,32);Q(e.char||"",n+108,32);Q(e.locale||"",n+140,32);E.get(d)(f,n,b)&&e.preventDefault()},Da:c};return Pa(a)}
function Wa(a,b,c,d,f,m){Sa||(Sa=R(72));a=Ua(a);return Pa({target:a,Ea:"mousemove"!=m&&"mouseenter"!=m&&"mouseleave"!=m,wa:m,Fa:d,Ia:function(e=event){var n=a,h=Sa;ma[h>>3]=e.timeStamp;h>>=2;C[h+2]=e.screenX;C[h+3]=e.screenY;C[h+4]=e.clientX;C[h+5]=e.clientY;C[h+6]=e.ctrlKey;C[h+7]=e.shiftKey;C[h+8]=e.altKey;C[h+9]=e.metaKey;ja[2*h+20]=e.button;ja[2*h+21]=e.buttons;C[h+11]=e.movementX;C[h+12]=e.movementY;n=0>Ta.indexOf(n)?n.getBoundingClientRect():{left:0,top:0};C[h+13]=e.clientX-n.left;C[h+14]=e.clientY-
n.top;E.get(d)(f,Sa,b)&&e.preventDefault()},Da:c})}function Xa(){this.va=[void 0];this.Ha=[]}var T,U;
function Ya(a,b,c,d,f){function m(){var u=0,z=0;p.response&&K&&0===D[a+12>>2]&&(z=p.response.byteLength);0<z&&(u=R(z),y.set(new Uint8Array(p.response),u));D[a+12>>2]=u;V(a+16,z);V(a+24,0);(u=p.response?p.response.byteLength:0)&&V(a+32,u);A[a+40>>1]=p.readyState;A[a+42>>1]=p.status;p.statusText&&Q(p.statusText,a+44,64)}var e=D[a+8>>2];if(e){var n=N(e),h=a+112,t=N(h+0);t||(t="GET");var la=D[h+56>>2],B=D[h+68>>2],S=D[h+72>>2];e=D[h+76>>2];var L=D[h+80>>2],P=D[h+84>>2],H=D[h+88>>2],G=D[h+52>>2],K=!!(G&
1),aa=!!(G&2);G=!!(G&64);B=B?N(B):void 0;S=S?N(S):void 0;var p=new XMLHttpRequest;p.withCredentials=!!y[h+60>>0];p.open(t,n,!G,B,S);G||(p.timeout=la);p.ua=n;p.responseType="arraybuffer";L&&(n=N(L),p.overrideMimeType(n));if(e)for(;;){h=D[e>>2];if(!h)break;n=D[e+4>>2];if(!n)break;e+=8;h=N(h);n=N(n);p.setRequestHeader(h,n)}var I=T.Ma(p);D[a>>2]=I;e=P&&H?y.slice(P,P+H):null;p.onload=u=>{T.has(I)&&(m(),200<=p.status&&300>p.status?b&&b(a,p,u):c&&c(a,p,u))};p.onerror=u=>{T.has(I)&&(m(),c&&c(a,p,u))};p.ontimeout=
u=>{T.has(I)&&c&&c(a,p,u)};p.onprogress=u=>{if(T.has(I)){var z=K&&aa&&p.response?p.response.byteLength:0,r=0;0<z&&K&&aa&&(r=R(z),y.set(new Uint8Array(p.response),r));D[a+12>>2]=r;V(a+16,z);V(a+24,u.loaded-z);V(a+32,u.total);A[a+40>>1]=p.readyState;3<=p.readyState&&0===p.status&&0<u.loaded&&(p.status=200);A[a+42>>1]=p.status;p.statusText&&Q(p.statusText,a+44,64);d&&d(a,p,u);r&&Za(r)}};p.onreadystatechange=u=>{T.has(I)&&(A[a+40>>1]=p.readyState,2<=p.readyState&&(A[a+42>>1]=p.status),f&&f(a,p,u))};try{p.send(e)}catch(u){c&&
c(a,p,u)}}else c(a,0,"no url specified!")}var $a=a=>{a instanceof Da||"unwind"==a||ea(1,a)},ab=a=>{x=a;if(!noExitRuntime){if(g.onExit)g.onExit(a);ha=!0}ea(a,new Da(a))};function V(a,b){D[a>>2]=b;D[a+4>>2]=(b-D[a>>2])/4294967296}
function bb(a,b,c,d){var f=U;if(f){var m=D[a+112+64>>2];m||(m=D[a+8>>2]);var e=N(m);try{var n=f.transaction(["FILES"],"readwrite").objectStore("FILES").put(b,e);n.onsuccess=()=>{A[a+40>>1]=4;A[a+42>>1]=200;Q("OK",a+44,64);c(a,0,e)};n.onerror=h=>{A[a+40>>1]=4;A[a+42>>1]=413;Q("Payload Too Large",a+44,64);d(a,0,h)}}catch(h){d(a,0,h)}}else d(a,0,"IndexedDB not available!")}
function cb(a,b,c){var d=U;if(d){var f=D[a+112+64>>2];f||(f=D[a+8>>2]);f=N(f);try{var m=d.transaction(["FILES"],"readonly").objectStore("FILES").get(f);m.onsuccess=e=>{if(e.target.result){e=e.target.result;var n=e.byteLength||e.length,h=R(n);y.set(new Uint8Array(e),h);D[a+12>>2]=h;V(a+16,n);V(a+24,0);V(a+32,n);A[a+40>>1]=4;A[a+42>>1]=200;Q("OK",a+44,64);b(a,0,e)}else A[a+40>>1]=4,A[a+42>>1]=404,Q("Not Found",a+44,64),c(a,0,"no data")};m.onerror=e=>{A[a+40>>1]=4;A[a+42>>1]=404;Q("Not Found",a+44,64);
c(a,0,e)}}catch(e){c(a,0,e)}}else c(a,0,"IndexedDB not available!")}function db(a,b,c){var d=U;if(d){var f=D[a+112+64>>2];f||(f=D[a+8>>2]);f=N(f);try{var m=d.transaction(["FILES"],"readwrite").objectStore("FILES").delete(f);m.onsuccess=e=>{e=e.target.result;D[a+12>>2]=0;V(a+16,0);V(a+24,0);V(a+32,0);A[a+40>>1]=4;A[a+42>>1]=200;Q("OK",a+44,64);b(a,0,e)};m.onerror=e=>{A[a+40>>1]=4;A[a+42>>1]=404;Q("Not Found",a+44,64);c(a,0,e)}}catch(e){c(a,0,e)}}else c(a,0,"IndexedDB not available!")}
var eb=1,fb=[],W=[],gb=[],X=[],Y=[],hb=[],ib=[];function jb(a){for(var b=eb++,c=a.length;c<b;c++)a[c]=null;return b}function kb(a,b){a.ua||(a.ua=a.getContext,a.getContext=function(d,f){f=a.ua(d,f);return"webgl"==d==f instanceof WebGLRenderingContext?f:null});var c=a.getContext("webgl2",b);return c?lb(c,b):0}function lb(a,b){var c=jb(ib);b={handle:c,attributes:b,version:b.Ra,La:a};a.canvas&&(a.canvas.Wa=b);ib[c]=b;return c}
var mb,nb=["default","low-power","high-performance"],ob=()=>{if("object"==typeof crypto&&"function"==typeof crypto.getRandomValues)return a=>crypto.getRandomValues(a);v("initRandomDevice")},pb=a=>(pb=ob())(a);function qb(a,b,c,d){for(var f=0;f<a;f++){var m=Z[c](),e=m&&jb(d);m&&(m.name=e,d[e]=m);C[b+4*f>>2]=e}}function rb(a){return"]"==a.slice(-1)&&a.lastIndexOf("[")}function sb(a){var b=Z.Na,c=b.xa[a];"number"==typeof c&&(b.xa[a]=c=Z.getUniformLocation(b,b.Ja[a]+(0<c?"["+c+"]":"")));return c}T=new Xa;
va();(function(a,b){try{var c=indexedDB.open("emscripten_filesystem",1)}catch(d){b(d);return}c.onupgradeneeded=d=>{d=d.target.result;d.objectStoreNames.contains("FILES")&&d.deleteObjectStore("FILES");d.createObjectStore("FILES")};c.onsuccess=d=>a(d.target.result);c.onerror=d=>b(d)})(a=>{U=a;wa()},()=>{U=!1;wa()});Object.assign(Xa.prototype,{get(a){return this.va[a]},has(a){return void 0!==this.va[a]},Ma(a){var b=this.Ha.pop()||this.va.length;this.va[b]=a;return b},Qa(a){this.va[a]=void 0;this.Ha.push(a)}});
var Z,tb={a:(a,b,c,d)=>{v(`Assertion failed: ${N(a)}, at: `+[b?N(b):"unknown filename",c,d?N(d):"unknown function"])},f:function(a,b,c){(new Ga(a)).Sa(b,c);Ha=a;Ia++;throw Ha;},L:function(a){if(T.has(a)){var b=T.get(a);T.Qa(a);0<b.readyState&&4>b.readyState&&b.abort()}},J:()=>!0,s:()=>{v("")},I:()=>performance.now(),N:function(){return!0},W:function(a,b){function c(d){E.get(a)(d,b)&&requestAnimationFrame(c)}return requestAnimationFrame(c)},K:a=>{var b=y.length;a>>>=0;if(2147483648<a)return!1;for(var c=
1;4>=c;c*=2){var d=b*(1+.2/c);d=Math.min(d,a+100663296);var f=Math;d=Math.max(a,d);a:{f=f.min.call(f,2147483648,d+(65536-d%65536)%65536)-fa.buffer.byteLength+65535>>>16;try{fa.grow(f);na();var m=1;break a}catch(e){}m=void 0}if(m)return!0}return!1},fa:function(a,b,c){a=Ua(a);if(!a)return-4;a.width=b;a.height=c;return 0},ka:function(a,b,c,d){return Va(a,b,c,d,2,"keydown")},ja:function(a,b,c,d){return Va(a,b,c,d,3,"keyup")},ia:function(a,b,c,d){return Wa(a,b,c,d,5,"mousedown")},ga:function(a,b,c,d){return Wa(a,
b,c,d,8,"mousemove")},ha:function(a,b,c,d){return Wa(a,b,c,d,6,"mouseup")},M:function(a,b,c,d,f){function m(r){if(S)r();else if(!ha)try{if(r(),!noExitRuntime)try{x=r=x,ab(r)}catch(ba){$a(ba)}}catch(ba){$a(ba)}}var e=a+112,n=D[e+36>>2],h=D[e+40>>2],t=D[e+44>>2],la=D[e+48>>2],B=D[e+52>>2],S=!!(B&64),L=r=>{m(()=>{n?E.get(n)(r):b&&b(r)})},P=r=>{m(()=>{t?E.get(t)(r):d&&d(r)})},H=r=>{m(()=>{h?E.get(h)(r):c&&c(r)})},G=r=>{m(()=>{la?E.get(la)(r):f&&f(r)})},K=r=>{Ya(r,L,H,P,G)},aa=(r,ba)=>{bb(r,ba.response,
ca=>{m(()=>{n?E.get(n)(ca):b&&b(ca)})},ca=>{m(()=>{n?E.get(n)(ca):b&&b(ca)})})},p=r=>{Ya(r,aa,H,P,G)},I=N(e+0),u=!!(B&16),z=!!(B&4);B=!!(B&32);if("EM_IDB_STORE"===I)K=D[e+84>>2],bb(a,y.slice(K,K+D[e+88>>2]),L,H);else if("EM_IDB_DELETE"===I)db(a,L,H);else if(u){if(B)return 0;Ya(a,z?aa:L,H,P,G)}else cb(a,L,B?H:z?p:K);return a},da:function(a,b){b>>=2;b={alpha:!!C[b],depth:!!C[b+1],stencil:!!C[b+2],antialias:!!C[b+3],premultipliedAlpha:!!C[b+4],preserveDrawingBuffer:!!C[b+5],powerPreference:nb[C[b+6]],
failIfMajorPerformanceCaveat:!!C[b+7],Ra:C[b+8],$a:C[b+9],Za:C[b+10],Pa:C[b+11],ab:C[b+12],bb:C[b+13]};a=Ua(a);return!a||b.Pa?0:kb(a,b)},ea:function(a){a>>=2;for(var b=0;14>b;++b)C[a+b]=0;C[a]=C[a+1]=C[a+3]=C[a+4]=C[a+8]=C[a+10]=1},ca:function(a){mb=ib[a];g.Ya=Z=mb&&mb.La;return!a||Z?0:-5},H:(a,b)=>{pb(y.subarray(a,a+b));return 0},$:function(a){Z.activeTexture(a)},y:function(a,b){Z.attachShader(W[a],Y[b])},d:function(a,b){35051==a?Z.Ga=b:35052==a&&(Z.Ba=b);Z.bindBuffer(a,fb[b])},E:function(a,b){Z.bindFramebuffer(a,
gb[b])},c:function(a,b){Z.bindTexture(a,X[b])},j:function(a){Z.bindVertexArray(hb[a])},aa:function(a,b){Z.blendFunc(a,b)},l:function(a,b,c,d){c&&b?Z.bufferData(a,y,d,c,b):Z.bufferData(a,b,d)},G:function(a){Z.clear(a)},u:function(a,b,c,d){Z.clearColor(a,b,c,d)},S:function(a){Z.compileShader(Y[a])},Y:function(){var a=jb(W),b=Z.createProgram();b.name=a;b.Aa=b.ya=b.za=0;b.Ca=1;W[a]=b;return a},U:function(a){var b=jb(Y);Y[b]=Z.createShader(a);return b},q:function(a,b){for(var c=0;c<a;c++){var d=C[b+4*
c>>2],f=fb[d];f&&(Z.deleteBuffer(f),f.name=0,fb[d]=null,d==Z.Ga&&(Z.Ga=0),d==Z.Ba&&(Z.Ba=0))}},v:function(a,b){for(var c=0;c<a;++c){var d=C[b+4*c>>2],f=gb[d];f&&(Z.deleteFramebuffer(f),f.name=0,gb[d]=null)}},k:function(a){if(a){var b=W[a];b&&(Z.deleteProgram(b),b.name=0,W[a]=null)}},e:function(a){if(a){var b=Y[a];b&&(Z.deleteShader(b),Y[a]=null)}},ma:function(a,b){for(var c=0;c<a;c++){var d=C[b+4*c>>2],f=X[d];f&&(Z.deleteTexture(f),f.name=0,X[d]=null)}},D:function(a,b){for(var c=0;c<a;c++){var d=
C[b+4*c>>2];Z.deleteVertexArray(hb[d]);hb[d]=null}},m:function(a){Z.depthMask(!!a)},A:function(a,b,c,d){Z.drawArraysInstanced(a,b,c,d)},Z:function(a,b,c,d){Z.drawElements(a,b,c,d)},ba:function(a){Z.enable(a)},g:function(a){Z.enableVertexAttribArray(a)},la:function(a,b,c,d,f){Z.framebufferTexture2D(a,b,c,X[d],f)},p:function(a,b){qb(a,b,"createBuffer",fb)},Q:function(a,b){qb(a,b,"createFramebuffer",gb)},r:function(a,b){qb(a,b,"createTexture",X)},z:function(a,b){qb(a,b,"createVertexArray",hb)},i:function(a,
b){return Z.getAttribLocation(W[a],N(b))},V:function(a,b,c,d){a=Z.getProgramInfoLog(W[a]);b=0<b&&d?Q(a,d,b):0;c&&(C[c>>2]=b)},x:function(a,b,c){if(c&&!(a>=eb))if(a=W[a],35716==b)C[c>>2]=Z.getProgramInfoLog(a).length+1;else if(35719==b){if(!a.Aa)for(b=0;b<Z.getProgramParameter(a,35718);++b)a.Aa=Math.max(a.Aa,Z.getActiveUniform(a,b).name.length+1);C[c>>2]=a.Aa}else if(35722==b){if(!a.ya)for(b=0;b<Z.getProgramParameter(a,35721);++b)a.ya=Math.max(a.ya,Z.getActiveAttrib(a,b).name.length+1);C[c>>2]=a.ya}else if(35381==
b){if(!a.za)for(b=0;b<Z.getProgramParameter(a,35382);++b)a.za=Math.max(a.za,Z.getActiveUniformBlockName(a,b).length+1);C[c>>2]=a.za}else C[c>>2]=Z.getProgramParameter(a,b)},R:function(a,b,c,d){a=Z.getShaderInfoLog(Y[a]);b=0<b&&d?Q(a,d,b):0;c&&(C[c>>2]=b)},w:function(a,b,c){c&&(35716==b?(a=Z.getShaderInfoLog(Y[a]),C[c>>2]=a?a.length+1:0):35720==b?(a=Z.getShaderSource(Y[a]),C[c>>2]=a?a.length+1:0):C[c>>2]=Z.getShaderParameter(Y[a],b))},t:function(a,b){b=N(b);if(a=W[a]){var c=a,d=c.xa,f=c.Ka,m;if(!d)for(c.xa=
d={},c.Ja={},m=0;m<Z.getProgramParameter(c,35718);++m){var e=Z.getActiveUniform(c,m);var n=e.name;e=e.size;var h=rb(n);h=0<h?n.slice(0,h):n;var t=c.Ca;c.Ca+=e;f[h]=[e,t];for(n=0;n<e;++n)d[t]=n,c.Ja[t++]=h}c=a.xa;d=0;f=b;m=rb(b);0<m&&(d=parseInt(b.slice(m+1))>>>0,f=b.slice(0,m));if((f=a.Ka[f])&&d<f[0]&&(d+=f[1],c[d]=c[d]||Z.getUniformLocation(a,b)))return d}return-1},X:function(a){a=W[a];Z.linkProgram(a);a.xa=0;a.Ka={}},T:function(a,b,c,d){for(var f="",m=0;m<b;++m){var e=d?C[d+4*m>>2]:-1;f+=N(C[c+
4*m>>2],0>e?void 0:e)}Z.shaderSource(Y[a],f)},F:function(a,b,c,d,f,m,e,n,h){if(Z.Ba)Z.texImage2D(a,b,c,d,f,m,e,n,h);else if(h){var t=n-5120;t=0==t?ia:1==t?y:2==t?ja:4==t?C:6==t?ka:5==t||28922==t||28520==t||30779==t||30782==t?D:A;Z.texImage2D(a,b,c,d,f,m,e,n,t,h>>31-Math.clz32(t.BYTES_PER_ELEMENT))}else Z.texImage2D(a,b,c,d,f,m,e,n,null)},b:function(a,b,c){Z.texParameteri(a,b,c)},_:function(a,b){Z.uniform1i(sb(a),b)},B:function(a,b,c){Z.uniform2f(sb(a),b,c)},C:function(a){a=W[a];Z.useProgram(a);Z.Na=
a},o:function(a,b){Z.vertexAttribDivisor(a,b)},h:function(a,b,c,d,f,m){Z.vertexAttribPointer(a,b,c,!!d,f,m)},n:function(a,b,c,d){Z.viewport(a,b,c,d)},P:function(a,b,c,d){var f=document.createElement("canvas");f.width=b;f.height=c;b=f.getContext("2d");b.font=a+"px "+N(d);b.textBaseline="middle";b.globalAlpha=1;b.fillStyle="white";window["gCanvas"+a]=f;window["gCanvasCtx"+a]=b},O:function(a,b,c,d){var f=new Image;f.onload=()=>{D[c>>2]=f.width;D[d>>2]=f.height;Z.bindTexture(Z.TEXTURE_2D,X[a]);Z.texImage2D(Z.TEXTURE_2D,
0,Z.RGBA,Z.RGBA,Z.UNSIGNED_BYTE,f)};f.src=N(b)},na:function(a,b){var c=window["gCanvas"+a];a=window["gCanvasCtx"+a];a.clearRect(0,0,c.width,c.height);b=N(b);var d=a.measureText(b).width;a.fillText(b,0,c.height/2);Z.texImage2D(Z.TEXTURE_2D,0,Z.RGBA,Z.RGBA,Z.UNSIGNED_BYTE,c);return d}};
(function(){function a(c){w=c=c.exports;fa=w.oa;na();E=w.sa;pa.unshift(w.pa);wa();return c}var b={a:tb};va();if(g.instantiateWasm)try{return g.instantiateWasm(b,a)}catch(c){return l("Module.instantiateWasm callback failed with error: "+c),!1}Ca(b,function(c){a(c.instance)});return{}})();var Za=a=>(Za=w.qa)(a),R=a=>(R=w.ra)(a),ub=g._main=(a,b)=>(ub=g._main=w.ta)(a,b),vb;J=function wb(){vb||xb();vb||(J=wb)};
function xb(){function a(){if(!vb&&(vb=!0,g.calledRun=!0,!ha)){Ea(pa);Ea(qa);if(g.onRuntimeInitialized)g.onRuntimeInitialized();if(yb){var b=ub;try{var c=b(0,0);x=c;ab(c)}catch(d){$a(d)}}if(g.postRun)for("function"==typeof g.postRun&&(g.postRun=[g.postRun]);g.postRun.length;)b=g.postRun.shift(),sa.unshift(b);Ea(sa)}}if(!(0<F)){if(g.preRun)for("function"==typeof g.preRun&&(g.preRun=[g.preRun]);g.preRun.length;)ta();Ea(oa);0<F||(g.setStatus?(g.setStatus("Running..."),setTimeout(function(){setTimeout(function(){g.setStatus("")},
1);a()},1)):a())}}if(g.preInit)for("function"==typeof g.preInit&&(g.preInit=[g.preInit]);0<g.preInit.length;)g.preInit.pop()();var yb=!0;g.noInitialRun&&(yb=!1);xb();
