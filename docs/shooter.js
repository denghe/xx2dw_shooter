var g;g||(g=typeof Module !== 'undefined' ? Module : {});var aa=Object.assign({},g),ba=(a,b)=>{throw b;},h="";"undefined"!=typeof document&&document.currentScript&&(h=document.currentScript.src);h=0!==h.indexOf("blob:")?h.substr(0,h.replace(/[?#].*/,"").lastIndexOf("/")+1):"";var fa=g.print||console.log.bind(console),n=g.printErr||console.error.bind(console);Object.assign(g,aa);aa=null;g.quit&&(ba=g.quit);var r;g.wasmBinary&&(r=g.wasmBinary);var noExitRuntime=g.noExitRuntime||!0;
"object"!=typeof WebAssembly&&x("no native wasm support detected");var ha,y,ia=!1,z,ja,A,ka,C,D,F,la,ma;function na(){var a=ha.buffer;g.HEAP8=ja=new Int8Array(a);g.HEAP16=ka=new Int16Array(a);g.HEAP32=D=new Int32Array(a);g.HEAPU8=A=new Uint8Array(a);g.HEAPU16=C=new Uint16Array(a);g.HEAPU32=F=new Uint32Array(a);g.HEAPF32=la=new Float32Array(a);g.HEAPF64=ma=new Float64Array(a)}var G,oa=[],pa=[],qa=[],ra=[],sa=[];function ta(){var a=g.preRun.shift();oa.unshift(a)}var H=0,ua=null,M=null;
function va(){H++;g.monitorRunDependencies&&g.monitorRunDependencies(H)}function wa(){H--;g.monitorRunDependencies&&g.monitorRunDependencies(H);if(0==H&&(null!==ua&&(clearInterval(ua),ua=null),M)){var a=M;M=null;a()}}function x(a){if(g.onAbort)g.onAbort(a);a="Aborted("+a+")";n(a);ia=!0;z=1;throw new WebAssembly.RuntimeError(a+". Build with -sASSERTIONS for more info.");}function xa(a){return a.startsWith("data:application/octet-stream;base64,")}var O;O="shooter.wasm";
if(!xa(O)){var ya=O;O=g.locateFile?g.locateFile(ya,h):h+ya}function za(a){if(a==O&&r)return new Uint8Array(r);throw"both async and sync fetching of the wasm failed";}function Aa(a){return r||"function"!=typeof fetch?Promise.resolve().then(()=>za(a)):fetch(a,{credentials:"same-origin"}).then(b=>{if(!b.ok)throw"failed to load wasm binary file at '"+a+"'";return b.arrayBuffer()}).catch(()=>za(a))}
function Ba(a,b,c){return Aa(a).then(d=>WebAssembly.instantiate(d,b)).then(d=>d).then(c,d=>{n("failed to asynchronously prepare wasm: "+d);x(d)})}function Ca(a,b){var c=O;r||"function"!=typeof WebAssembly.instantiateStreaming||xa(c)||"function"!=typeof fetch?Ba(c,a,b):fetch(c,{credentials:"same-origin"}).then(d=>WebAssembly.instantiateStreaming(d,a).then(b,function(f){n("wasm streaming compile failed: "+f);n("falling back to ArrayBuffer instantiation");return Ba(c,a,b)}))}
function Da(a){this.name="ExitStatus";this.message=`Program terminated with exit(${a})`;this.status=a}var Ea=a=>{for(;0<a.length;)a.shift()(g)},Fa=new TextDecoder("utf8"),P=(a,b)=>{if(!a)return"";b=a+b;for(var c=a;!(c>=b)&&A[c];)++c;return Fa.decode(A.subarray(a,c))};function Ga(a){this.ya=a-24;this.Ta=function(b){F[this.ya+4>>2]=b};this.Sa=function(b){F[this.ya+8>>2]=b};this.ab=function(b,c){this.Ra();this.Ta(b);this.Sa(c)};this.Ra=function(){F[this.ya+16>>2]=0}}var Ha=0,Ia=0,Ja=0;
function Ka(){for(var a=Q.length-1;0<=a;--a)La(a);Q=[];Ma=[]}var Ma=[];function Na(){if(navigator.userActivation?navigator.userActivation.isActive:Ja&&Oa.Ga)for(var a=0;a<Ma.length;++a){var b=Ma[a];Ma.splice(a,1);--a;b.jb.apply(null,b.cb)}}var Q=[];function La(a){var b=Q[a];b.target.removeEventListener(b.za,b.Xa,b.Fa);Q.splice(a,1)}
function Pa(a){function b(d){++Ja;Oa=a;Na();a.Ja(d);Na();--Ja}if(!a.target)return-4;if(a.Ha)a.Xa=b,a.target.addEventListener(a.za,b,a.Fa),Q.push(a),Qa||(ra.push(Ka),Qa=!0);else for(var c=0;c<Q.length;++c)Q[c].target==a.target&&Q[c].za==a.za&&La(c--);return 0}var Qa,Oa,Ra,Sa,Ta,Ua=[0,document,window];function Va(a){a=2<a?P(a):a;return Ua[a]||document.querySelector(a)}
var R=(a,b,c)=>{var d=A;if(!(0<c))return 0;var f=b;c=b+c-1;for(var m=0;m<a.length;++m){var e=a.charCodeAt(m);if(55296<=e&&57343>=e){var k=a.charCodeAt(++m);e=65536+((e&1023)<<10)|k&1023}if(127>=e){if(b>=c)break;d[b++]=e}else{if(2047>=e){if(b+1>=c)break;d[b++]=192|e>>6}else{if(65535>=e){if(b+2>=c)break;d[b++]=224|e>>12}else{if(b+3>=c)break;d[b++]=240|e>>18;d[b++]=128|e>>12&63}d[b++]=128|e>>6&63}d[b++]=128|e&63}}d[b]=0;return b-f};
function Wa(a,b,c,d,f,m){Ra||(Ra=T(176));a={target:Va(a),Ga:!0,za:m,Ha:d,Ja:function(e){var k=Ra;ma[k>>3]=e.timeStamp;var l=k>>2;D[l+2]=e.location;D[l+3]=e.ctrlKey;D[l+4]=e.shiftKey;D[l+5]=e.altKey;D[l+6]=e.metaKey;D[l+7]=e.repeat;D[l+8]=e.charCode;D[l+9]=e.keyCode;D[l+10]=e.which;R(e.key||"",k+44,32);R(e.code||"",k+76,32);R(e.char||"",k+108,32);R(e.locale||"",k+140,32);G.get(d)(f,k,b)&&e.preventDefault()},Fa:c};return Pa(a)}
function Xa(a){return 0>Ua.indexOf(a)?a.getBoundingClientRect():{left:0,top:0}}
function Ya(a,b,c,d,f,m){Sa||(Sa=T(72));a=Va(a);return Pa({target:a,Ga:"mousemove"!=m&&"mouseenter"!=m&&"mouseleave"!=m,za:m,Ha:d,Ja:function(e=event){var k=a,l=Sa;ma[l>>3]=e.timeStamp;l>>=2;D[l+2]=e.screenX;D[l+3]=e.screenY;D[l+4]=e.clientX;D[l+5]=e.clientY;D[l+6]=e.ctrlKey;D[l+7]=e.shiftKey;D[l+8]=e.altKey;D[l+9]=e.metaKey;ka[2*l+20]=e.button;ka[2*l+21]=e.buttons;D[l+11]=e.movementX;D[l+12]=e.movementY;k=Xa(k);D[l+13]=e.clientX-k.left;D[l+14]=e.clientY-k.top;G.get(d)(f,Sa,b)&&e.preventDefault()},
Fa:c})}
function Za(a,b,c,d,f,m){Ta||(Ta=T(1696));a=Va(a);return Pa({target:a,Ga:"touchstart"==m||"touchend"==m,za:m,Ha:d,Ja:function(e){for(var k,l={},q=e.touches,v=0;v<q.length;++v)k=q[v],k.Na=k.Oa=0,l[k.identifier]=k;for(v=0;v<e.changedTouches.length;++v)k=e.changedTouches[v],k.Na=1,l[k.identifier]=k;for(v=0;v<e.targetTouches.length;++v)l[e.targetTouches[v].identifier].Oa=1;q=Ta;ma[q>>3]=e.timeStamp;var t=q>>2;D[t+3]=e.ctrlKey;D[t+4]=e.shiftKey;D[t+5]=e.altKey;D[t+6]=e.metaKey;t+=7;var J=Xa(a),E=0;for(v in l)if(k=
l[v],D[t]=k.identifier,D[t+1]=k.screenX,D[t+2]=k.screenY,D[t+3]=k.clientX,D[t+4]=k.clientY,D[t+5]=k.pageX,D[t+6]=k.pageY,D[t+7]=k.Na,D[t+8]=k.Oa,D[t+9]=k.clientX-J.left,D[t+10]=k.clientY-J.top,t+=13,31<++E)break;D[q+8>>2]=E;G.get(d)(f,q,b)&&e.preventDefault()},Fa:c})}function $a(){this.Aa=[void 0];this.Ma=[]}var U,ab;
function bb(a,b,c,d,f){function m(){var w=0,B=0;p.response&&N&&0===F[a+12>>2]&&(B=p.response.byteLength);0<B&&(w=T(B),A.set(new Uint8Array(p.response),w));F[a+12>>2]=w;V(a+16,B);V(a+24,0);(w=p.response?p.response.byteLength:0)&&V(a+32,w);C[a+40>>1]=p.readyState;C[a+42>>1]=p.status;p.statusText&&R(p.statusText,a+44,64)}var e=F[a+8>>2];if(e){var k=P(e),l=a+112,q=P(l+0);q||(q="GET");var v=F[l+56>>2],t=F[l+68>>2],J=F[l+72>>2];e=F[l+76>>2];var E=F[l+80>>2],S=F[l+84>>2],K=F[l+88>>2],I=F[l+52>>2],N=!!(I&
1),ca=!!(I&2);I=!!(I&64);t=t?P(t):void 0;J=J?P(J):void 0;var p=new XMLHttpRequest;p.withCredentials=!!A[l+60>>0];p.open(q,k,!I,t,J);I||(p.timeout=v);p.ya=k;p.responseType="arraybuffer";E&&(k=P(E),p.overrideMimeType(k));if(e)for(;;){l=F[e>>2];if(!l)break;k=F[e+4>>2];if(!k)break;e+=8;l=P(l);k=P(k);p.setRequestHeader(l,k)}var L=U.Va(p);F[a>>2]=L;e=S&&K?A.slice(S,S+K):null;p.onload=w=>{U.has(L)&&(m(),200<=p.status&&300>p.status?b&&b(a,p,w):c&&c(a,p,w))};p.onerror=w=>{U.has(L)&&(m(),c&&c(a,p,w))};p.ontimeout=
w=>{U.has(L)&&c&&c(a,p,w)};p.onprogress=w=>{if(U.has(L)){var B=N&&ca&&p.response?p.response.byteLength:0,u=0;0<B&&N&&ca&&(u=T(B),A.set(new Uint8Array(p.response),u));F[a+12>>2]=u;V(a+16,B);V(a+24,w.loaded-B);V(a+32,w.total);C[a+40>>1]=p.readyState;3<=p.readyState&&0===p.status&&0<w.loaded&&(p.status=200);C[a+42>>1]=p.status;p.statusText&&R(p.statusText,a+44,64);d&&d(a,p,w);u&&cb(u)}};p.onreadystatechange=w=>{U.has(L)&&(C[a+40>>1]=p.readyState,2<=p.readyState&&(C[a+42>>1]=p.status),f&&f(a,p,w))};try{p.send(e)}catch(w){c&&
c(a,p,w)}}else c(a,0,"no url specified!")}var db=a=>{a instanceof Da||"unwind"==a||ba(1,a)},eb=a=>{z=a;if(!noExitRuntime){if(g.onExit)g.onExit(a);ia=!0}ba(a,new Da(a))};function V(a,b){F[a>>2]=b;F[a+4>>2]=(b-F[a>>2])/4294967296}
function fb(a,b,c,d){var f=ab;if(f){var m=F[a+112+64>>2];m||(m=F[a+8>>2]);var e=P(m);try{var k=f.transaction(["FILES"],"readwrite").objectStore("FILES").put(b,e);k.onsuccess=()=>{C[a+40>>1]=4;C[a+42>>1]=200;R("OK",a+44,64);c(a,0,e)};k.onerror=l=>{C[a+40>>1]=4;C[a+42>>1]=413;R("Payload Too Large",a+44,64);d(a,0,l)}}catch(l){d(a,0,l)}}else d(a,0,"IndexedDB not available!")}
function gb(a,b,c){var d=ab;if(d){var f=F[a+112+64>>2];f||(f=F[a+8>>2]);f=P(f);try{var m=d.transaction(["FILES"],"readonly").objectStore("FILES").get(f);m.onsuccess=e=>{if(e.target.result){e=e.target.result;var k=e.byteLength||e.length,l=T(k);A.set(new Uint8Array(e),l);F[a+12>>2]=l;V(a+16,k);V(a+24,0);V(a+32,k);C[a+40>>1]=4;C[a+42>>1]=200;R("OK",a+44,64);b(a,0,e)}else C[a+40>>1]=4,C[a+42>>1]=404,R("Not Found",a+44,64),c(a,0,"no data")};m.onerror=e=>{C[a+40>>1]=4;C[a+42>>1]=404;R("Not Found",a+44,
64);c(a,0,e)}}catch(e){c(a,0,e)}}else c(a,0,"IndexedDB not available!")}function hb(a,b,c){var d=ab;if(d){var f=F[a+112+64>>2];f||(f=F[a+8>>2]);f=P(f);try{var m=d.transaction(["FILES"],"readwrite").objectStore("FILES").delete(f);m.onsuccess=e=>{e=e.target.result;F[a+12>>2]=0;V(a+16,0);V(a+24,0);V(a+32,0);C[a+40>>1]=4;C[a+42>>1]=200;R("OK",a+44,64);b(a,0,e)};m.onerror=e=>{C[a+40>>1]=4;C[a+42>>1]=404;R("Not Found",a+44,64);c(a,0,e)}}catch(e){c(a,0,e)}}else c(a,0,"IndexedDB not available!")}
var ib=1,jb=[],W=[],kb=[],X=[],Y=[],lb=[],mb=[];function nb(a){for(var b=ib++,c=a.length;c<b;c++)a[c]=null;return b}function ob(a,b){a.ya||(a.ya=a.getContext,a.getContext=function(d,f){f=a.ya(d,f);return"webgl"==d==f instanceof WebGLRenderingContext?f:null});var c=a.getContext("webgl2",b);return c?pb(c,b):0}function pb(a,b){var c=nb(mb);b={handle:c,attributes:b,version:b.$a,Ua:a};a.canvas&&(a.canvas.bb=b);mb[c]=b;return c}
var qb,rb=["default","low-power","high-performance"],sb=[null,[],[]],tb=()=>{if("object"==typeof crypto&&"function"==typeof crypto.getRandomValues)return a=>crypto.getRandomValues(a);x("initRandomDevice")},ub=a=>(ub=tb())(a);function vb(a,b,c,d){for(var f=0;f<a;f++){var m=Z[c](),e=m&&nb(d);m&&(m.name=e,d[e]=m);D[b+4*f>>2]=e}}function wb(a){return"]"==a.slice(-1)&&a.lastIndexOf("[")}
function xb(a){var b=Z.Wa,c=b.Ba[a];"number"==typeof c&&(b.Ba[a]=c=Z.getUniformLocation(b,b.Pa[a]+(0<c?"["+c+"]":"")));return c}U=new $a;va();(function(a,b){try{var c=indexedDB.open("emscripten_filesystem",1)}catch(d){b(d);return}c.onupgradeneeded=d=>{d=d.target.result;d.objectStoreNames.contains("FILES")&&d.deleteObjectStore("FILES");d.createObjectStore("FILES")};c.onsuccess=d=>a(d.target.result);c.onerror=d=>b(d)})(a=>{ab=a;wa()},()=>{ab=!1;wa()});
Object.assign($a.prototype,{get(a){return this.Aa[a]},has(a){return void 0!==this.Aa[a]},Va(a){var b=this.Ma.pop()||this.Aa.length;this.Aa[b]=a;return b},Za(a){this.Aa[a]=void 0;this.Ma.push(a)}});
var Z,yb={s:(a,b,c,d)=>{x(`Assertion failed: ${P(a)}, at: `+[b?P(b):"unknown filename",c,d?P(d):"unknown function"])},c:function(a,b,c){(new Ga(a)).ab(b,c);Ha=a;Ia++;throw Ha;},O:function(a){if(U.has(a)){var b=U.get(a);U.Za(a);0<b.readyState&&4>b.readyState&&b.abort()}},L:()=>!0,t:()=>{x("")},N:()=>performance.now(),Q:function(){return!0},ra:function(a,b){function c(d){G.get(a)(d,b)&&requestAnimationFrame(c)}return requestAnimationFrame(c)},M:a=>{var b=A.length;a>>>=0;if(2147483648<a)return!1;for(var c=
1;4>=c;c*=2){var d=b*(1+.2/c);d=Math.min(d,a+100663296);var f=Math;d=Math.max(a,d);a:{f=f.min.call(f,2147483648,d+(65536-d%65536)%65536)-ha.buffer.byteLength+65535>>>16;try{ha.grow(f);na();var m=1;break a}catch(e){}m=void 0}if(m)return!0}return!1},ha:function(a,b,c){a=Va(a);if(!a)return-4;a.width=b;a.height=c;return 0},qa:function(a,b,c,d){return Wa(a,b,c,d,2,"keydown")},pa:function(a,b,c,d){return Wa(a,b,c,d,3,"keyup")},oa:function(a,b,c,d){return Ya(a,b,c,d,5,"mousedown")},ma:function(a,b,c,d){return Ya(a,
b,c,d,8,"mousemove")},na:function(a,b,c,d){return Ya(a,b,c,d,6,"mouseup")},ia:function(a,b,c,d){return Za(a,b,c,d,25,"touchcancel")},ja:function(a,b,c,d){return Za(a,b,c,d,23,"touchend")},ka:function(a,b,c,d){return Za(a,b,c,d,24,"touchmove")},la:function(a,b,c,d){return Za(a,b,c,d,22,"touchstart")},P:function(a,b,c,d,f){function m(u){if(J)u();else if(!ia)try{if(u(),!noExitRuntime)try{z=u=z,eb(u)}catch(da){db(da)}}catch(da){db(da)}}var e=a+112,k=F[e+36>>2],l=F[e+40>>2],q=F[e+44>>2],v=F[e+48>>2],t=
F[e+52>>2],J=!!(t&64),E=u=>{m(()=>{k?G.get(k)(u):b&&b(u)})},S=u=>{m(()=>{q?G.get(q)(u):d&&d(u)})},K=u=>{m(()=>{l?G.get(l)(u):c&&c(u)})},I=u=>{m(()=>{v?G.get(v)(u):f&&f(u)})},N=u=>{bb(u,E,K,S,I)},ca=(u,da)=>{fb(u,da.response,ea=>{m(()=>{k?G.get(k)(ea):b&&b(ea)})},ea=>{m(()=>{k?G.get(k)(ea):b&&b(ea)})})},p=u=>{bb(u,ca,K,S,I)},L=P(e+0),w=!!(t&16),B=!!(t&4);t=!!(t&32);if("EM_IDB_STORE"===L)N=F[e+84>>2],fb(a,A.slice(N,N+F[e+88>>2]),E,K);else if("EM_IDB_DELETE"===L)hb(a,E,K);else if(w){if(t)return 0;bb(a,
B?ca:E,K,S,I)}else gb(a,E,t?K:B?p:N);return a},fa:function(a,b){b>>=2;b={alpha:!!D[b],depth:!!D[b+1],stencil:!!D[b+2],antialias:!!D[b+3],premultipliedAlpha:!!D[b+4],preserveDrawingBuffer:!!D[b+5],powerPreference:rb[D[b+6]],failIfMajorPerformanceCaveat:!!D[b+7],$a:D[b+8],gb:D[b+9],fb:D[b+10],Ya:D[b+11],hb:D[b+12],ib:D[b+13]};a=Va(a);return!a||b.Ya?0:ob(a,b)},ga:function(a){a>>=2;for(var b=0;14>b;++b)D[a+b]=0;D[a]=D[a+1]=D[a+3]=D[a+4]=D[a+8]=D[a+10]=1},ea:function(a){qb=mb[a];g.eb=Z=qb&&qb.Ua;return!a||
Z?0:-5},x:(a,b,c,d)=>{for(var f=0,m=0;m<c;m++){var e=F[b>>2],k=F[b+4>>2];b+=8;for(var l=0;l<k;l++){var q=A[e+l],v=sb[a];if(0===q||10===q){for(q=0;v[q]&&!(NaN<=q);)++q;q=Fa.decode(v.buffer?v.subarray(0,q):new Uint8Array(v.slice(0,q)));(1===a?fa:n)(q);v.length=0}else v.push(q)}f+=k}F[d>>2]=f;return 0},K:(a,b)=>{ub(A.subarray(a,a+b));return 0},q:function(a){Z.activeTexture(a)},C:function(a,b){Z.attachShader(W[a],Y[b])},d:function(a,b){35051==a?Z.La=b:35052==a&&(Z.Ia=b);Z.bindBuffer(a,jb[b])},y:function(a,
b){Z.bindFramebuffer(a,kb[b])},b:function(a,b){Z.bindTexture(a,X[b])},f:function(a){Z.bindVertexArray(lb[a])},ca:function(a,b){Z.blendFunc(a,b)},g:function(a,b,c,d){c&&b?Z.bufferData(a,A,d,c,b):Z.bufferData(a,b,d)},H:function(a){Z.clear(a)},I:function(a,b,c,d){Z.clearColor(a,b,c,d)},X:function(a){Z.compileShader(Y[a])},aa:function(){var a=nb(W),b=Z.createProgram();b.name=a;b.Ea=b.Ca=b.Da=0;b.Ka=1;W[a]=b;return a},Z:function(a){var b=nb(Y);Y[b]=Z.createShader(a);return b},z:function(a,b){for(var c=
0;c<a;c++){var d=D[b+4*c>>2],f=jb[d];f&&(Z.deleteBuffer(f),f.name=0,jb[d]=null,d==Z.La&&(Z.La=0),d==Z.Ia&&(Z.Ia=0))}},v:function(a,b){for(var c=0;c<a;++c){var d=D[b+4*c>>2],f=kb[d];f&&(Z.deleteFramebuffer(f),f.name=0,kb[d]=null)}},F:function(a){if(a){var b=W[a];b&&(Z.deleteProgram(b),b.name=0,W[a]=null)}},m:function(a){if(a){var b=Y[a];b&&(Z.deleteShader(b),Y[a]=null)}},U:function(a,b){for(var c=0;c<a;c++){var d=D[b+4*c>>2],f=X[d];f&&(Z.deleteTexture(f),f.name=0,X[d]=null)}},V:function(a,b){for(var c=
0;c<a;c++){var d=D[b+4*c>>2];Z.deleteVertexArray(lb[d]);lb[d]=null}},n:function(a){Z.depthMask(!!a)},k:function(a,b,c,d){Z.drawArraysInstanced(a,b,c,d)},da:function(a){Z.enable(a)},h:function(a){Z.enableVertexAttribArray(a)},S:function(a,b,c,d,f){Z.framebufferTexture2D(a,b,c,X[d],f)},D:function(a,b){vb(a,b,"createBuffer",jb)},J:function(a,b){vb(a,b,"createFramebuffer",kb)},e:function(a,b){vb(a,b,"createTexture",X)},ba:function(a,b){vb(a,b,"createVertexArray",lb)},j:function(a,b){return Z.getAttribLocation(W[a],
P(b))},_:function(a,b,c,d){a=Z.getProgramInfoLog(W[a]);b=0<b&&d?R(a,d,b):0;c&&(D[c>>2]=b)},B:function(a,b,c){if(c&&!(a>=ib))if(a=W[a],35716==b)D[c>>2]=Z.getProgramInfoLog(a).length+1;else if(35719==b){if(!a.Ea)for(b=0;b<Z.getProgramParameter(a,35718);++b)a.Ea=Math.max(a.Ea,Z.getActiveUniform(a,b).name.length+1);D[c>>2]=a.Ea}else if(35722==b){if(!a.Ca)for(b=0;b<Z.getProgramParameter(a,35721);++b)a.Ca=Math.max(a.Ca,Z.getActiveAttrib(a,b).name.length+1);D[c>>2]=a.Ca}else if(35381==b){if(!a.Da)for(b=
0;b<Z.getProgramParameter(a,35382);++b)a.Da=Math.max(a.Da,Z.getActiveUniformBlockName(a,b).length+1);D[c>>2]=a.Da}else D[c>>2]=Z.getProgramParameter(a,b)},W:function(a,b,c,d){a=Z.getShaderInfoLog(Y[a]);b=0<b&&d?R(a,d,b):0;c&&(D[c>>2]=b)},A:function(a,b,c){c&&(35716==b?(a=Z.getShaderInfoLog(Y[a]),D[c>>2]=a?a.length+1:0):35720==b?(a=Z.getShaderSource(Y[a]),D[c>>2]=a?a.length+1:0):D[c>>2]=Z.getShaderParameter(Y[a],b))},E:function(a,b){b=P(b);if(a=W[a]){var c=a,d=c.Ba,f=c.Qa,m;if(!d)for(c.Ba=d={},c.Pa=
{},m=0;m<Z.getProgramParameter(c,35718);++m){var e=Z.getActiveUniform(c,m);var k=e.name;e=e.size;var l=wb(k);l=0<l?k.slice(0,l):k;var q=c.Ka;c.Ka+=e;f[l]=[e,q];for(k=0;k<e;++k)d[q]=k,c.Pa[q++]=l}c=a.Ba;d=0;f=b;m=wb(b);0<m&&(d=parseInt(b.slice(m+1))>>>0,f=b.slice(0,m));if((f=a.Qa[f])&&d<f[0]&&(d+=f[1],c[d]=c[d]||Z.getUniformLocation(a,b)))return d}return-1},$:function(a){a=W[a];Z.linkProgram(a);a.Ba=0;a.Qa={}},Y:function(a,b,c,d){for(var f="",m=0;m<b;++m){var e=d?D[d+4*m>>2]:-1;f+=P(D[c+4*m>>2],0>
e?void 0:e)}Z.shaderSource(Y[a],f)},u:function(a,b,c,d,f,m,e,k,l){if(Z.Ia)Z.texImage2D(a,b,c,d,f,m,e,k,l);else if(l){var q=k-5120;q=0==q?ja:1==q?A:2==q?ka:4==q?D:6==q?la:5==q||28922==q||28520==q||30779==q||30782==q?F:C;Z.texImage2D(a,b,c,d,f,m,e,k,q,l>>31-Math.clz32(q.BYTES_PER_ELEMENT))}else Z.texImage2D(a,b,c,d,f,m,e,k,null)},a:function(a,b,c){Z.texParameteri(a,b,c)},p:function(a,b){Z.uniform1i(xb(a),b)},o:function(a,b,c){Z.uniform2f(xb(a),b,c)},r:function(a){a=W[a];Z.useProgram(a);Z.Wa=a},l:function(a,
b){Z.vertexAttribDivisor(a,b)},i:function(a,b,c,d,f,m){Z.vertexAttribPointer(a,b,c,!!d,f,m)},w:function(a,b,c,d){Z.viewport(a,b,c,d)},G:function(a,b,c){if(void 0===window.gCanvas){var d=document.createElement("canvas");d.width=b;d.height=c;b=d.getContext("2d");b.font=a+"px monospace";b.textBaseline="middle";b.globalAlpha=1;b.fillStyle="white";window["gCanvas"+a]=d;window["gCanvasCtx"+a]=b}},R:function(a,b,c,d){var f=new Image;f.onload=()=>{F[c>>2]=f.width;F[d>>2]=f.height;Z.bindTexture(Z.TEXTURE_2D,
X[a]);Z.texImage2D(Z.TEXTURE_2D,0,Z.RGBA,Z.RGBA,Z.UNSIGNED_BYTE,f)};f.src=P(b)},T:function(a,b){var c=window["gCanvas"+a];a=window["gCanvasCtx"+a];a.clearRect(0,0,c.width,c.height);b=P(b);var d=a.measureText(b).width;a.fillText(b,0,c.height/2);Z.texImage2D(Z.TEXTURE_2D,0,Z.RGBA,Z.RGBA,Z.UNSIGNED_BYTE,c);return d}};
(function(){function a(c){y=c=c.exports;ha=y.sa;na();G=y.xa;pa.unshift(y.ta);wa();return c}var b={a:yb};va();if(g.instantiateWasm)try{return g.instantiateWasm(b,a)}catch(c){return n("Module.instantiateWasm callback failed with error: "+c),!1}Ca(b,function(c){a(c.instance)});return{}})();var cb=a=>(cb=y.ua)(a),T=a=>(T=y.va)(a),zb=g._main=(a,b)=>(zb=g._main=y.wa)(a,b),Ab;M=function Bb(){Ab||Cb();Ab||(M=Bb)};
function Cb(){function a(){if(!Ab&&(Ab=!0,g.calledRun=!0,!ia)){Ea(pa);Ea(qa);if(g.onRuntimeInitialized)g.onRuntimeInitialized();if(Db){var b=zb;try{var c=b(0,0);z=c;eb(c)}catch(d){db(d)}}if(g.postRun)for("function"==typeof g.postRun&&(g.postRun=[g.postRun]);g.postRun.length;)b=g.postRun.shift(),sa.unshift(b);Ea(sa)}}if(!(0<H)){if(g.preRun)for("function"==typeof g.preRun&&(g.preRun=[g.preRun]);g.preRun.length;)ta();Ea(oa);0<H||(g.setStatus?(g.setStatus("Running..."),setTimeout(function(){setTimeout(function(){g.setStatus("")},
1);a()},1)):a())}}if(g.preInit)for("function"==typeof g.preInit&&(g.preInit=[g.preInit]);0<g.preInit.length;)g.preInit.pop()();var Db=!0;g.noInitialRun&&(Db=!1);Cb();
