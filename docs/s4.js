var g;g||(g=typeof Module !== 'undefined' ? Module : {});var aa=Object.assign({},g),ba=(a,b)=>{throw b;},h="";"undefined"!=typeof document&&document.currentScript&&(h=document.currentScript.src);h=0!==h.indexOf("blob:")?h.substr(0,h.replace(/[?#].*/,"").lastIndexOf("/")+1):"";var n=g.printErr||console.error.bind(console);Object.assign(g,aa);aa=null;g.quit&&(ba=g.quit);var q;g.wasmBinary&&(q=g.wasmBinary);var noExitRuntime=g.noExitRuntime||!0;"object"!=typeof WebAssembly&&w("no native wasm support detected");
var fa,x,ha=!1,z,ia,A,ja,C,D,F,ka,la;function ma(){var a=fa.buffer;g.HEAP8=ia=new Int8Array(a);g.HEAP16=ja=new Int16Array(a);g.HEAP32=D=new Int32Array(a);g.HEAPU8=A=new Uint8Array(a);g.HEAPU16=C=new Uint16Array(a);g.HEAPU32=F=new Uint32Array(a);g.HEAPF32=ka=new Float32Array(a);g.HEAPF64=la=new Float64Array(a)}var G,na=[],oa=[],pa=[],qa=[],ra=[];function sa(){var a=g.preRun.shift();na.unshift(a)}var H=0,ta=null,M=null;function ua(){H++;g.monitorRunDependencies&&g.monitorRunDependencies(H)}
function va(){H--;g.monitorRunDependencies&&g.monitorRunDependencies(H);if(0==H&&(null!==ta&&(clearInterval(ta),ta=null),M)){var a=M;M=null;a()}}function w(a){if(g.onAbort)g.onAbort(a);a="Aborted("+a+")";n(a);ha=!0;z=1;throw new WebAssembly.RuntimeError(a+". Build with -sASSERTIONS for more info.");}function wa(a){return a.startsWith("data:application/octet-stream;base64,")}var O;O="s4.wasm";if(!wa(O)){var xa=O;O=g.locateFile?g.locateFile(xa,h):h+xa}
function ya(a){if(a==O&&q)return new Uint8Array(q);throw"both async and sync fetching of the wasm failed";}function za(a){return q||"function"!=typeof fetch?Promise.resolve().then(()=>ya(a)):fetch(a,{credentials:"same-origin"}).then(b=>{if(!b.ok)throw"failed to load wasm binary file at '"+a+"'";return b.arrayBuffer()}).catch(()=>ya(a))}function Aa(a,b,c){return za(a).then(d=>WebAssembly.instantiate(d,b)).then(d=>d).then(c,d=>{n("failed to asynchronously prepare wasm: "+d);w(d)})}
function Ba(a,b){var c=O;q||"function"!=typeof WebAssembly.instantiateStreaming||wa(c)||"function"!=typeof fetch?Aa(c,a,b):fetch(c,{credentials:"same-origin"}).then(d=>WebAssembly.instantiateStreaming(d,a).then(b,function(f){n("wasm streaming compile failed: "+f);n("falling back to ArrayBuffer instantiation");return Aa(c,a,b)}))}function Ca(a){this.name="ExitStatus";this.message=`Program terminated with exit(${a})`;this.status=a}
var Da=a=>{for(;0<a.length;)a.shift()(g)},Ea=new TextDecoder("utf8"),P=(a,b)=>{if(!a)return"";b=a+b;for(var c=a;!(c>=b)&&A[c];)++c;return Ea.decode(A.subarray(a,c))};function Fa(a){this.ya=a-24;this.Ta=function(b){F[this.ya+4>>2]=b};this.Sa=function(b){F[this.ya+8>>2]=b};this.ab=function(b,c){this.Ra();this.Ta(b);this.Sa(c)};this.Ra=function(){F[this.ya+16>>2]=0}}var Ga=0,Ha=0,Ia=0;function Ja(){for(var a=Q.length-1;0<=a;--a)Ka(a);Q=[];La=[]}var La=[];
function Ma(){if(navigator.userActivation?navigator.userActivation.isActive:Ia&&Na.Ga)for(var a=0;a<La.length;++a){var b=La[a];La.splice(a,1);--a;b.jb.apply(null,b.cb)}}var Q=[];function Ka(a){var b=Q[a];b.target.removeEventListener(b.za,b.Xa,b.Fa);Q.splice(a,1)}
function Oa(a){function b(d){++Ia;Na=a;Ma();a.Ja(d);Ma();--Ia}if(!a.target)return-4;if(a.Ha)a.Xa=b,a.target.addEventListener(a.za,b,a.Fa),Q.push(a),Pa||(qa.push(Ja),Pa=!0);else for(var c=0;c<Q.length;++c)Q[c].target==a.target&&Q[c].za==a.za&&Ka(c--);return 0}var Pa,Na,Qa,Ra,Sa,Ta=[0,document,window];function Ua(a){a=2<a?P(a):a;return Ta[a]||document.querySelector(a)}
var R=(a,b,c)=>{var d=A;if(!(0<c))return 0;var f=b;c=b+c-1;for(var m=0;m<a.length;++m){var e=a.charCodeAt(m);if(55296<=e&&57343>=e){var k=a.charCodeAt(++m);e=65536+((e&1023)<<10)|k&1023}if(127>=e){if(b>=c)break;d[b++]=e}else{if(2047>=e){if(b+1>=c)break;d[b++]=192|e>>6}else{if(65535>=e){if(b+2>=c)break;d[b++]=224|e>>12}else{if(b+3>=c)break;d[b++]=240|e>>18;d[b++]=128|e>>12&63}d[b++]=128|e>>6&63}d[b++]=128|e&63}}d[b]=0;return b-f};
function Va(a,b,c,d,f,m){Qa||(Qa=T(176));a={target:Ua(a),Ga:!0,za:m,Ha:d,Ja:function(e){var k=Qa;la[k>>3]=e.timeStamp;var l=k>>2;D[l+2]=e.location;D[l+3]=e.ctrlKey;D[l+4]=e.shiftKey;D[l+5]=e.altKey;D[l+6]=e.metaKey;D[l+7]=e.repeat;D[l+8]=e.charCode;D[l+9]=e.keyCode;D[l+10]=e.which;R(e.key||"",k+44,32);R(e.code||"",k+76,32);R(e.char||"",k+108,32);R(e.locale||"",k+140,32);G.get(d)(f,k,b)&&e.preventDefault()},Fa:c};return Oa(a)}
function Wa(a){return 0>Ta.indexOf(a)?a.getBoundingClientRect():{left:0,top:0}}
function Xa(a,b,c,d,f,m){Ra||(Ra=T(72));a=Ua(a);return Oa({target:a,Ga:"mousemove"!=m&&"mouseenter"!=m&&"mouseleave"!=m,za:m,Ha:d,Ja:function(e=event){var k=a,l=Ra;la[l>>3]=e.timeStamp;l>>=2;D[l+2]=e.screenX;D[l+3]=e.screenY;D[l+4]=e.clientX;D[l+5]=e.clientY;D[l+6]=e.ctrlKey;D[l+7]=e.shiftKey;D[l+8]=e.altKey;D[l+9]=e.metaKey;ja[2*l+20]=e.button;ja[2*l+21]=e.buttons;D[l+11]=e.movementX;D[l+12]=e.movementY;k=Wa(k);D[l+13]=e.clientX-k.left;D[l+14]=e.clientY-k.top;G.get(d)(f,Ra,b)&&e.preventDefault()},
Fa:c})}
function Ya(a,b,c,d,f,m){Sa||(Sa=T(1696));a=Ua(a);return Oa({target:a,Ga:"touchstart"==m||"touchend"==m,za:m,Ha:d,Ja:function(e){for(var k,l={},r=e.touches,y=0;y<r.length;++y)k=r[y],k.Na=k.Oa=0,l[k.identifier]=k;for(y=0;y<e.changedTouches.length;++y)k=e.changedTouches[y],k.Na=1,l[k.identifier]=k;for(y=0;y<e.targetTouches.length;++y)l[e.targetTouches[y].identifier].Oa=1;r=Sa;la[r>>3]=e.timeStamp;var t=r>>2;D[t+3]=e.ctrlKey;D[t+4]=e.shiftKey;D[t+5]=e.altKey;D[t+6]=e.metaKey;t+=7;var J=Wa(a),E=0;for(y in l)if(k=
l[y],D[t]=k.identifier,D[t+1]=k.screenX,D[t+2]=k.screenY,D[t+3]=k.clientX,D[t+4]=k.clientY,D[t+5]=k.pageX,D[t+6]=k.pageY,D[t+7]=k.Na,D[t+8]=k.Oa,D[t+9]=k.clientX-J.left,D[t+10]=k.clientY-J.top,t+=13,31<++E)break;D[r+8>>2]=E;G.get(d)(f,r,b)&&e.preventDefault()},Fa:c})}function Za(){this.Aa=[void 0];this.Ma=[]}var U,$a;
function ab(a,b,c,d,f){function m(){var v=0,B=0;p.response&&N&&0===F[a+12>>2]&&(B=p.response.byteLength);0<B&&(v=T(B),A.set(new Uint8Array(p.response),v));F[a+12>>2]=v;V(a+16,B);V(a+24,0);(v=p.response?p.response.byteLength:0)&&V(a+32,v);C[a+40>>1]=p.readyState;C[a+42>>1]=p.status;p.statusText&&R(p.statusText,a+44,64)}var e=F[a+8>>2];if(e){var k=P(e),l=a+112,r=P(l+0);r||(r="GET");var y=F[l+56>>2],t=F[l+68>>2],J=F[l+72>>2];e=F[l+76>>2];var E=F[l+80>>2],S=F[l+84>>2],K=F[l+88>>2],I=F[l+52>>2],N=!!(I&
1),ca=!!(I&2);I=!!(I&64);t=t?P(t):void 0;J=J?P(J):void 0;var p=new XMLHttpRequest;p.withCredentials=!!A[l+60>>0];p.open(r,k,!I,t,J);I||(p.timeout=y);p.ya=k;p.responseType="arraybuffer";E&&(k=P(E),p.overrideMimeType(k));if(e)for(;;){l=F[e>>2];if(!l)break;k=F[e+4>>2];if(!k)break;e+=8;l=P(l);k=P(k);p.setRequestHeader(l,k)}var L=U.Va(p);F[a>>2]=L;e=S&&K?A.slice(S,S+K):null;p.onload=v=>{U.has(L)&&(m(),200<=p.status&&300>p.status?b&&b(a,p,v):c&&c(a,p,v))};p.onerror=v=>{U.has(L)&&(m(),c&&c(a,p,v))};p.ontimeout=
v=>{U.has(L)&&c&&c(a,p,v)};p.onprogress=v=>{if(U.has(L)){var B=N&&ca&&p.response?p.response.byteLength:0,u=0;0<B&&N&&ca&&(u=T(B),A.set(new Uint8Array(p.response),u));F[a+12>>2]=u;V(a+16,B);V(a+24,v.loaded-B);V(a+32,v.total);C[a+40>>1]=p.readyState;3<=p.readyState&&0===p.status&&0<v.loaded&&(p.status=200);C[a+42>>1]=p.status;p.statusText&&R(p.statusText,a+44,64);d&&d(a,p,v);u&&bb(u)}};p.onreadystatechange=v=>{U.has(L)&&(C[a+40>>1]=p.readyState,2<=p.readyState&&(C[a+42>>1]=p.status),f&&f(a,p,v))};try{p.send(e)}catch(v){c&&
c(a,p,v)}}else c(a,0,"no url specified!")}var cb=a=>{a instanceof Ca||"unwind"==a||ba(1,a)},db=a=>{z=a;if(!noExitRuntime){if(g.onExit)g.onExit(a);ha=!0}ba(a,new Ca(a))};function V(a,b){F[a>>2]=b;F[a+4>>2]=(b-F[a>>2])/4294967296}
function eb(a,b,c,d){var f=$a;if(f){var m=F[a+112+64>>2];m||(m=F[a+8>>2]);var e=P(m);try{var k=f.transaction(["FILES"],"readwrite").objectStore("FILES").put(b,e);k.onsuccess=()=>{C[a+40>>1]=4;C[a+42>>1]=200;R("OK",a+44,64);c(a,0,e)};k.onerror=l=>{C[a+40>>1]=4;C[a+42>>1]=413;R("Payload Too Large",a+44,64);d(a,0,l)}}catch(l){d(a,0,l)}}else d(a,0,"IndexedDB not available!")}
function fb(a,b,c){var d=$a;if(d){var f=F[a+112+64>>2];f||(f=F[a+8>>2]);f=P(f);try{var m=d.transaction(["FILES"],"readonly").objectStore("FILES").get(f);m.onsuccess=e=>{if(e.target.result){e=e.target.result;var k=e.byteLength||e.length,l=T(k);A.set(new Uint8Array(e),l);F[a+12>>2]=l;V(a+16,k);V(a+24,0);V(a+32,k);C[a+40>>1]=4;C[a+42>>1]=200;R("OK",a+44,64);b(a,0,e)}else C[a+40>>1]=4,C[a+42>>1]=404,R("Not Found",a+44,64),c(a,0,"no data")};m.onerror=e=>{C[a+40>>1]=4;C[a+42>>1]=404;R("Not Found",a+44,
64);c(a,0,e)}}catch(e){c(a,0,e)}}else c(a,0,"IndexedDB not available!")}function gb(a,b,c){var d=$a;if(d){var f=F[a+112+64>>2];f||(f=F[a+8>>2]);f=P(f);try{var m=d.transaction(["FILES"],"readwrite").objectStore("FILES").delete(f);m.onsuccess=e=>{e=e.target.result;F[a+12>>2]=0;V(a+16,0);V(a+24,0);V(a+32,0);C[a+40>>1]=4;C[a+42>>1]=200;R("OK",a+44,64);b(a,0,e)};m.onerror=e=>{C[a+40>>1]=4;C[a+42>>1]=404;R("Not Found",a+44,64);c(a,0,e)}}catch(e){c(a,0,e)}}else c(a,0,"IndexedDB not available!")}
var hb=1,ib=[],W=[],jb=[],X=[],Y=[],kb=[],lb=[];function mb(a){for(var b=hb++,c=a.length;c<b;c++)a[c]=null;return b}function nb(a,b){a.ya||(a.ya=a.getContext,a.getContext=function(d,f){f=a.ya(d,f);return"webgl"==d==f instanceof WebGLRenderingContext?f:null});var c=a.getContext("webgl2",b);return c?ob(c,b):0}function ob(a,b){var c=mb(lb);b={handle:c,attributes:b,version:b.$a,Ua:a};a.canvas&&(a.canvas.bb=b);lb[c]=b;return c}
var pb,qb=["default","low-power","high-performance"],rb=()=>{if("object"==typeof crypto&&"function"==typeof crypto.getRandomValues)return a=>crypto.getRandomValues(a);w("initRandomDevice")},sb=a=>(sb=rb())(a);function tb(a,b,c,d){for(var f=0;f<a;f++){var m=Z[c](),e=m&&mb(d);m&&(m.name=e,d[e]=m);D[b+4*f>>2]=e}}function ub(a){return"]"==a.slice(-1)&&a.lastIndexOf("[")}function vb(a){var b=Z.Wa,c=b.Ba[a];"number"==typeof c&&(b.Ba[a]=c=Z.getUniformLocation(b,b.Pa[a]+(0<c?"["+c+"]":"")));return c}U=new Za;
ua();(function(a,b){try{var c=indexedDB.open("emscripten_filesystem",1)}catch(d){b(d);return}c.onupgradeneeded=d=>{d=d.target.result;d.objectStoreNames.contains("FILES")&&d.deleteObjectStore("FILES");d.createObjectStore("FILES")};c.onsuccess=d=>a(d.target.result);c.onerror=d=>b(d)})(a=>{$a=a;va()},()=>{$a=!1;va()});Object.assign(Za.prototype,{get(a){return this.Aa[a]},has(a){return void 0!==this.Aa[a]},Va(a){var b=this.Ma.pop()||this.Aa.length;this.Aa[b]=a;return b},Za(a){this.Aa[a]=void 0;this.Ma.push(a)}});
var Z,wb={c:(a,b,c,d)=>{w(`Assertion failed: ${P(a)}, at: `+[b?P(b):"unknown filename",c,d?P(d):"unknown function"])},e:function(a,b,c){(new Fa(a)).ab(b,c);Ga=a;Ha++;throw Ga;},L:function(a){if(U.has(a)){var b=U.get(a);U.Za(a);0<b.readyState&&4>b.readyState&&b.abort()}},J:()=>!0,s:()=>{w("")},I:()=>performance.now(),N:function(){return!0},pa:function(a,b){function c(d){G.get(a)(d,b)&&requestAnimationFrame(c)}return requestAnimationFrame(c)},K:a=>{var b=A.length;a>>>=0;if(2147483648<a)return!1;for(var c=
1;4>=c;c*=2){var d=b*(1+.2/c);d=Math.min(d,a+100663296);var f=Math;d=Math.max(a,d);a:{f=f.min.call(f,2147483648,d+(65536-d%65536)%65536)-fa.buffer.byteLength+65535>>>16;try{fa.grow(f);ma();var m=1;break a}catch(e){}m=void 0}if(m)return!0}return!1},fa:function(a,b,c){a=Ua(a);if(!a)return-4;a.width=b;a.height=c;return 0},oa:function(a,b,c,d){return Va(a,b,c,d,2,"keydown")},na:function(a,b,c,d){return Va(a,b,c,d,3,"keyup")},ma:function(a,b,c,d){return Xa(a,b,c,d,5,"mousedown")},ka:function(a,b,c,d){return Xa(a,
b,c,d,8,"mousemove")},la:function(a,b,c,d){return Xa(a,b,c,d,6,"mouseup")},ga:function(a,b,c,d){return Ya(a,b,c,d,25,"touchcancel")},ha:function(a,b,c,d){return Ya(a,b,c,d,23,"touchend")},ia:function(a,b,c,d){return Ya(a,b,c,d,24,"touchmove")},ja:function(a,b,c,d){return Ya(a,b,c,d,22,"touchstart")},M:function(a,b,c,d,f){function m(u){if(J)u();else if(!ha)try{if(u(),!noExitRuntime)try{z=u=z,db(u)}catch(da){cb(da)}}catch(da){cb(da)}}var e=a+112,k=F[e+36>>2],l=F[e+40>>2],r=F[e+44>>2],y=F[e+48>>2],t=
F[e+52>>2],J=!!(t&64),E=u=>{m(()=>{k?G.get(k)(u):b&&b(u)})},S=u=>{m(()=>{r?G.get(r)(u):d&&d(u)})},K=u=>{m(()=>{l?G.get(l)(u):c&&c(u)})},I=u=>{m(()=>{y?G.get(y)(u):f&&f(u)})},N=u=>{ab(u,E,K,S,I)},ca=(u,da)=>{eb(u,da.response,ea=>{m(()=>{k?G.get(k)(ea):b&&b(ea)})},ea=>{m(()=>{k?G.get(k)(ea):b&&b(ea)})})},p=u=>{ab(u,ca,K,S,I)},L=P(e+0),v=!!(t&16),B=!!(t&4);t=!!(t&32);if("EM_IDB_STORE"===L)N=F[e+84>>2],eb(a,A.slice(N,N+F[e+88>>2]),E,K);else if("EM_IDB_DELETE"===L)gb(a,E,K);else if(v){if(t)return 0;ab(a,
B?ca:E,K,S,I)}else fb(a,E,t?K:B?p:N);return a},da:function(a,b){b>>=2;b={alpha:!!D[b],depth:!!D[b+1],stencil:!!D[b+2],antialias:!!D[b+3],premultipliedAlpha:!!D[b+4],preserveDrawingBuffer:!!D[b+5],powerPreference:qb[D[b+6]],failIfMajorPerformanceCaveat:!!D[b+7],$a:D[b+8],gb:D[b+9],fb:D[b+10],Ya:D[b+11],hb:D[b+12],ib:D[b+13]};a=Ua(a);return!a||b.Ya?0:nb(a,b)},ea:function(a){a>>=2;for(var b=0;14>b;++b)D[a+b]=0;D[a]=D[a+1]=D[a+3]=D[a+4]=D[a+8]=D[a+10]=1},ca:function(a){pb=lb[a];g.eb=Z=pb&&pb.Ua;return!a||
Z?0:-5},H:(a,b)=>{sb(A.subarray(a,a+b));return 0},$:function(a){Z.activeTexture(a)},A:function(a,b){Z.attachShader(W[a],Y[b])},d:function(a,b){35051==a?Z.La=b:35052==a&&(Z.Ia=b);Z.bindBuffer(a,ib[b])},v:function(a,b){Z.bindFramebuffer(a,jb[b])},b:function(a,b){Z.bindTexture(a,X[b])},j:function(a){Z.bindVertexArray(kb[a])},aa:function(a,b){Z.blendFunc(a,b)},k:function(a,b,c,d){c&&b?Z.bufferData(a,A,d,c,b):Z.bufferData(a,b,d)},F:function(a){Z.clear(a)},G:function(a,b,c,d){Z.clearColor(a,b,c,d)},S:function(a){Z.compileShader(Y[a])},
X:function(){var a=mb(W),b=Z.createProgram();b.name=a;b.Ea=b.Ca=b.Da=0;b.Ka=1;W[a]=b;return a},U:function(a){var b=mb(Y);Y[b]=Z.createShader(a);return b},p:function(a,b){for(var c=0;c<a;c++){var d=D[b+4*c>>2],f=ib[d];f&&(Z.deleteBuffer(f),f.name=0,ib[d]=null,d==Z.La&&(Z.La=0),d==Z.Ia&&(Z.Ia=0))}},x:function(a,b){for(var c=0;c<a;++c){var d=D[b+4*c>>2],f=jb[d];f&&(Z.deleteFramebuffer(f),f.name=0,jb[d]=null)}},l:function(a){if(a){var b=W[a];b&&(Z.deleteProgram(b),b.name=0,W[a]=null)}},f:function(a){if(a){var b=
Y[a];b&&(Z.deleteShader(b),Y[a]=null)}},ra:function(a,b){for(var c=0;c<a;c++){var d=D[b+4*c>>2],f=X[d];f&&(Z.deleteTexture(f),f.name=0,X[d]=null)}},E:function(a,b){for(var c=0;c<a;c++){var d=D[b+4*c>>2];Z.deleteVertexArray(kb[d]);kb[d]=null}},q:function(a){Z.depthMask(!!a)},u:function(a,b,c,d){Z.drawArraysInstanced(a,b,c,d)},Z:function(a,b,c,d){Z.drawElements(a,b,c,d)},ba:function(a){Z.enable(a)},g:function(a){Z.enableVertexAttribArray(a)},qa:function(a,b,c,d,f){Z.framebufferTexture2D(a,b,c,X[d],
f)},o:function(a,b){tb(a,b,"createBuffer",ib)},Q:function(a,b){tb(a,b,"createFramebuffer",jb)},m:function(a,b){tb(a,b,"createTexture",X)},B:function(a,b){tb(a,b,"createVertexArray",kb)},i:function(a,b){return Z.getAttribLocation(W[a],P(b))},V:function(a,b,c,d){a=Z.getProgramInfoLog(W[a]);b=0<b&&d?R(a,d,b):0;c&&(D[c>>2]=b)},z:function(a,b,c){if(c&&!(a>=hb))if(a=W[a],35716==b)D[c>>2]=Z.getProgramInfoLog(a).length+1;else if(35719==b){if(!a.Ea)for(b=0;b<Z.getProgramParameter(a,35718);++b)a.Ea=Math.max(a.Ea,
Z.getActiveUniform(a,b).name.length+1);D[c>>2]=a.Ea}else if(35722==b){if(!a.Ca)for(b=0;b<Z.getProgramParameter(a,35721);++b)a.Ca=Math.max(a.Ca,Z.getActiveAttrib(a,b).name.length+1);D[c>>2]=a.Ca}else if(35381==b){if(!a.Da)for(b=0;b<Z.getProgramParameter(a,35382);++b)a.Da=Math.max(a.Da,Z.getActiveUniformBlockName(a,b).length+1);D[c>>2]=a.Da}else D[c>>2]=Z.getProgramParameter(a,b)},R:function(a,b,c,d){a=Z.getShaderInfoLog(Y[a]);b=0<b&&d?R(a,d,b):0;c&&(D[c>>2]=b)},y:function(a,b,c){c&&(35716==b?(a=Z.getShaderInfoLog(Y[a]),
D[c>>2]=a?a.length+1:0):35720==b?(a=Z.getShaderSource(Y[a]),D[c>>2]=a?a.length+1:0):D[c>>2]=Z.getShaderParameter(Y[a],b))},t:function(a,b){b=P(b);if(a=W[a]){var c=a,d=c.Ba,f=c.Qa,m;if(!d)for(c.Ba=d={},c.Pa={},m=0;m<Z.getProgramParameter(c,35718);++m){var e=Z.getActiveUniform(c,m);var k=e.name;e=e.size;var l=ub(k);l=0<l?k.slice(0,l):k;var r=c.Ka;c.Ka+=e;f[l]=[e,r];for(k=0;k<e;++k)d[r]=k,c.Pa[r++]=l}c=a.Ba;d=0;f=b;m=ub(b);0<m&&(d=parseInt(b.slice(m+1))>>>0,f=b.slice(0,m));if((f=a.Qa[f])&&d<f[0]&&(d+=
f[1],c[d]=c[d]||Z.getUniformLocation(a,b)))return d}return-1},W:function(a){a=W[a];Z.linkProgram(a);a.Ba=0;a.Qa={}},T:function(a,b,c,d){for(var f="",m=0;m<b;++m){var e=d?D[d+4*m>>2]:-1;f+=P(D[c+4*m>>2],0>e?void 0:e)}Z.shaderSource(Y[a],f)},w:function(a,b,c,d,f,m,e,k,l){if(Z.Ia)Z.texImage2D(a,b,c,d,f,m,e,k,l);else if(l){var r=k-5120;r=0==r?ia:1==r?A:2==r?ja:4==r?D:6==r?ka:5==r||28922==r||28520==r||30779==r||30782==r?F:C;Z.texImage2D(a,b,c,d,f,m,e,k,r,l>>31-Math.clz32(r.BYTES_PER_ELEMENT))}else Z.texImage2D(a,
b,c,d,f,m,e,k,null)},a:function(a,b,c){Z.texParameteri(a,b,c)},_:function(a,b){Z.uniform1i(vb(a),b)},C:function(a,b,c){Z.uniform2f(vb(a),b,c)},D:function(a){a=W[a];Z.useProgram(a);Z.Wa=a},n:function(a,b){Z.vertexAttribDivisor(a,b)},h:function(a,b,c,d,f,m){Z.vertexAttribPointer(a,b,c,!!d,f,m)},r:function(a,b,c,d){Z.viewport(a,b,c,d)},P:function(a,b,c,d){var f=document.createElement("canvas");f.width=b;f.height=c;b=f.getContext("2d");b.font=a+"px "+P(d);b.textBaseline="middle";b.globalAlpha=1;b.fillStyle=
"white";window["gCanvas"+a]=f;window["gCanvasCtx"+a]=b},O:function(a,b,c,d){var f=new Image;f.onload=()=>{F[c>>2]=f.width;F[d>>2]=f.height;Z.bindTexture(Z.TEXTURE_2D,X[a]);Z.texImage2D(Z.TEXTURE_2D,0,Z.RGBA,Z.RGBA,Z.UNSIGNED_BYTE,f)};f.src=P(b)},Y:function(a,b){var c=window["gCanvas"+a];a=window["gCanvasCtx"+a];a.clearRect(0,0,c.width,c.height);b=P(b);var d=a.measureText(b).width;a.fillText(b,0,c.height/2);Z.texImage2D(Z.TEXTURE_2D,0,Z.RGBA,Z.RGBA,Z.UNSIGNED_BYTE,c);return d}};
(function(){function a(c){x=c=c.exports;fa=x.sa;ma();G=x.wa;oa.unshift(x.ta);va();return c}var b={a:wb};ua();if(g.instantiateWasm)try{return g.instantiateWasm(b,a)}catch(c){return n("Module.instantiateWasm callback failed with error: "+c),!1}Ba(b,function(c){a(c.instance)});return{}})();var bb=a=>(bb=x.ua)(a),T=a=>(T=x.va)(a),xb=g._main=(a,b)=>(xb=g._main=x.xa)(a,b),yb;M=function zb(){yb||Ab();yb||(M=zb)};
function Ab(){function a(){if(!yb&&(yb=!0,g.calledRun=!0,!ha)){Da(oa);Da(pa);if(g.onRuntimeInitialized)g.onRuntimeInitialized();if(Bb){var b=xb;try{var c=b(0,0);z=c;db(c)}catch(d){cb(d)}}if(g.postRun)for("function"==typeof g.postRun&&(g.postRun=[g.postRun]);g.postRun.length;)b=g.postRun.shift(),ra.unshift(b);Da(ra)}}if(!(0<H)){if(g.preRun)for("function"==typeof g.preRun&&(g.preRun=[g.preRun]);g.preRun.length;)sa();Da(na);0<H||(g.setStatus?(g.setStatus("Running..."),setTimeout(function(){setTimeout(function(){g.setStatus("")},
1);a()},1)):a())}}if(g.preInit)for("function"==typeof g.preInit&&(g.preInit=[g.preInit]);0<g.preInit.length;)g.preInit.pop()();var Bb=!0;g.noInitialRun&&(Bb=!1);Ab();
