var g;g||(g=typeof Module !== 'undefined' ? Module : {});var aa=Object.assign({},g),ea=(a,b)=>{throw b;},l="";"undefined"!=typeof document&&document.currentScript&&(l=document.currentScript.src);l=0!==l.indexOf("blob:")?l.substr(0,l.replace(/[?#].*/,"").lastIndexOf("/")+1):"";var fa=g.print||console.log.bind(console),m=g.printErr||console.error.bind(console);Object.assign(g,aa);aa=null;g.quit&&(ea=g.quit);var q;g.wasmBinary&&(q=g.wasmBinary);var noExitRuntime=g.noExitRuntime||!0;
"object"!=typeof WebAssembly&&v("no native wasm support detected");var ha,w,ia=!1,x,ja,y,ka,A,D,E,la,ma;function na(){var a=ha.buffer;g.HEAP8=ja=new Int8Array(a);g.HEAP16=ka=new Int16Array(a);g.HEAP32=D=new Int32Array(a);g.HEAPU8=y=new Uint8Array(a);g.HEAPU16=A=new Uint16Array(a);g.HEAPU32=E=new Uint32Array(a);g.HEAPF32=la=new Float32Array(a);g.HEAPF64=ma=new Float64Array(a)}var F,oa=[],pa=[],qa=[],ra=[],sa=[];function ta(){var a=g.preRun.shift();oa.unshift(a)}var G=0,ua=null,K=null;
function va(){G++;g.monitorRunDependencies&&g.monitorRunDependencies(G)}function wa(){G--;g.monitorRunDependencies&&g.monitorRunDependencies(G);if(0==G&&(null!==ua&&(clearInterval(ua),ua=null),K)){var a=K;K=null;a()}}function v(a){if(g.onAbort)g.onAbort(a);a="Aborted("+a+")";m(a);ia=!0;x=1;throw new WebAssembly.RuntimeError(a+". Build with -sASSERTIONS for more info.");}function xa(a){return a.startsWith("data:application/octet-stream;base64,")}var N;N="s13.wasm";
if(!xa(N)){var ya=N;N=g.locateFile?g.locateFile(ya,l):l+ya}function za(a){if(a==N&&q)return new Uint8Array(q);throw"both async and sync fetching of the wasm failed";}function Aa(a){return q||"function"!=typeof fetch?Promise.resolve().then(()=>za(a)):fetch(a,{credentials:"same-origin"}).then(b=>{if(!b.ok)throw"failed to load wasm binary file at '"+a+"'";return b.arrayBuffer()}).catch(()=>za(a))}
function Ba(a,b,c){return Aa(a).then(d=>WebAssembly.instantiate(d,b)).then(d=>d).then(c,d=>{m("failed to asynchronously prepare wasm: "+d);v(d)})}function Ca(a,b){var c=N;q||"function"!=typeof WebAssembly.instantiateStreaming||xa(c)||"function"!=typeof fetch?Ba(c,a,b):fetch(c,{credentials:"same-origin"}).then(d=>WebAssembly.instantiateStreaming(d,a).then(b,function(f){m("wasm streaming compile failed: "+f);m("falling back to ArrayBuffer instantiation");return Ba(c,a,b)}))}
function Da(a){this.name="ExitStatus";this.message=`Program terminated with exit(${a})`;this.status=a}var Ea=a=>{for(;0<a.length;)a.shift()(g)},Fa=new TextDecoder("utf8"),O=(a,b)=>{if(!a)return"";b=a+b;for(var c=a;!(c>=b)&&y[c];)++c;return Fa.decode(y.subarray(a,c))};function Ga(a){this.va=a-24;this.Wa=function(b){E[this.va+4>>2]=b};this.Va=function(b){E[this.va+8>>2]=b};this.Ta=function(b,c){this.Ua();this.Wa(b);this.Va(c)};this.Ua=function(){E[this.va+16>>2]=0}}var Ha=0,Ia=0,Ja=0;
function Ka(){for(var a=P.length-1;0<=a;--a)La(a);P=[];Ma=[]}var Ma=[];function Na(){if(navigator.userActivation?navigator.userActivation.isActive:Ja&&Oa.Fa)for(var a=0;a<Ma.length;++a){var b=Ma[a];Ma.splice(a,1);--a;b.eb.apply(null,b.Ya)}}var P=[];function La(a){var b=P[a];b.target.removeEventListener(b.xa,b.Pa,b.Ea);P.splice(a,1)}
function Pa(a){function b(d){++Ja;Oa=a;Na();a.Ja(d);Na();--Ja}if(!a.target)return-4;if(a.Ga)a.Pa=b,a.target.addEventListener(a.xa,b,a.Ea),P.push(a),Qa||(ra.push(Ka),Qa=!0);else for(var c=0;c<P.length;++c)P[c].target==a.target&&P[c].xa==a.xa&&La(c--);return 0}var Qa,Oa,Ra,Sa,Ta=[0,document,window];function Ua(a){a=2<a?O(a):a;return Ta[a]||document.querySelector(a)}
var R=(a,b,c)=>{var d=y;if(!(0<c))return 0;var f=b;c=b+c-1;for(var k=0;k<a.length;++k){var e=a.charCodeAt(k);if(55296<=e&&57343>=e){var n=a.charCodeAt(++k);e=65536+((e&1023)<<10)|n&1023}if(127>=e){if(b>=c)break;d[b++]=e}else{if(2047>=e){if(b+1>=c)break;d[b++]=192|e>>6}else{if(65535>=e){if(b+2>=c)break;d[b++]=224|e>>12}else{if(b+3>=c)break;d[b++]=240|e>>18;d[b++]=128|e>>12&63}d[b++]=128|e>>6&63}d[b++]=128|e&63}}d[b]=0;return b-f};
function Va(a,b,c,d,f,k){Ra||(Ra=S(176));a={target:Ua(a),Fa:!0,xa:k,Ga:d,Ja:function(e){var n=Ra;ma[n>>3]=e.timeStamp;var h=n>>2;D[h+2]=e.location;D[h+3]=e.ctrlKey;D[h+4]=e.shiftKey;D[h+5]=e.altKey;D[h+6]=e.metaKey;D[h+7]=e.repeat;D[h+8]=e.charCode;D[h+9]=e.keyCode;D[h+10]=e.which;R(e.key||"",n+44,32);R(e.code||"",n+76,32);R(e.char||"",n+108,32);R(e.locale||"",n+140,32);F.get(d)(f,n,b)&&e.preventDefault()},Ea:c};return Pa(a)}
function Wa(a,b,c,d,f,k){Sa||(Sa=S(72));a=Ua(a);return Pa({target:a,Fa:"mousemove"!=k&&"mouseenter"!=k&&"mouseleave"!=k,xa:k,Ga:d,Ja:function(e=event){var n=a,h=Sa;ma[h>>3]=e.timeStamp;h>>=2;D[h+2]=e.screenX;D[h+3]=e.screenY;D[h+4]=e.clientX;D[h+5]=e.clientY;D[h+6]=e.ctrlKey;D[h+7]=e.shiftKey;D[h+8]=e.altKey;D[h+9]=e.metaKey;ka[2*h+20]=e.button;ka[2*h+21]=e.buttons;D[h+11]=e.movementX;D[h+12]=e.movementY;n=0>Ta.indexOf(n)?n.getBoundingClientRect():{left:0,top:0};D[h+13]=e.clientX-n.left;D[h+14]=e.clientY-
n.top;F.get(d)(f,Sa,b)&&e.preventDefault()},Ea:c})}function Xa(){this.wa=[void 0];this.Ia=[]}var T,Ya;
function Za(a,b,c,d,f){function k(){var u=0,z=0;p.response&&L&&0===E[a+12>>2]&&(z=p.response.byteLength);0<z&&(u=S(z),y.set(new Uint8Array(p.response),u));E[a+12>>2]=u;V(a+16,z);V(a+24,0);(u=p.response?p.response.byteLength:0)&&V(a+32,u);A[a+40>>1]=p.readyState;A[a+42>>1]=p.status;p.statusText&&R(p.statusText,a+44,64)}var e=E[a+8>>2];if(e){var n=O(e),h=a+112,r=O(h+0);r||(r="GET");var C=E[h+56>>2],B=E[h+68>>2],U=E[h+72>>2];e=E[h+76>>2];var M=E[h+80>>2],Q=E[h+84>>2],I=E[h+88>>2],H=E[h+52>>2],L=!!(H&
1),ba=!!(H&2);H=!!(H&64);B=B?O(B):void 0;U=U?O(U):void 0;var p=new XMLHttpRequest;p.withCredentials=!!y[h+60>>0];p.open(r,n,!H,B,U);H||(p.timeout=C);p.va=n;p.responseType="arraybuffer";M&&(n=O(M),p.overrideMimeType(n));if(e)for(;;){h=E[e>>2];if(!h)break;n=E[e+4>>2];if(!n)break;e+=8;h=O(h);n=O(n);p.setRequestHeader(h,n)}var J=T.Na(p);E[a>>2]=J;e=Q&&I?y.slice(Q,Q+I):null;p.onload=u=>{T.has(J)&&(k(),200<=p.status&&300>p.status?b&&b(a,p,u):c&&c(a,p,u))};p.onerror=u=>{T.has(J)&&(k(),c&&c(a,p,u))};p.ontimeout=
u=>{T.has(J)&&c&&c(a,p,u)};p.onprogress=u=>{if(T.has(J)){var z=L&&ba&&p.response?p.response.byteLength:0,t=0;0<z&&L&&ba&&(t=S(z),y.set(new Uint8Array(p.response),t));E[a+12>>2]=t;V(a+16,z);V(a+24,u.loaded-z);V(a+32,u.total);A[a+40>>1]=p.readyState;3<=p.readyState&&0===p.status&&0<u.loaded&&(p.status=200);A[a+42>>1]=p.status;p.statusText&&R(p.statusText,a+44,64);d&&d(a,p,u);t&&$a(t)}};p.onreadystatechange=u=>{T.has(J)&&(A[a+40>>1]=p.readyState,2<=p.readyState&&(A[a+42>>1]=p.status),f&&f(a,p,u))};try{p.send(e)}catch(u){c&&
c(a,p,u)}}else c(a,0,"no url specified!")}var ab=a=>{a instanceof Da||"unwind"==a||ea(1,a)},bb=a=>{x=a;if(!noExitRuntime){if(g.onExit)g.onExit(a);ia=!0}ea(a,new Da(a))};function V(a,b){E[a>>2]=b;E[a+4>>2]=(b-E[a>>2])/4294967296}
function cb(a,b,c,d){var f=Ya;if(f){var k=E[a+112+64>>2];k||(k=E[a+8>>2]);var e=O(k);try{var n=f.transaction(["FILES"],"readwrite").objectStore("FILES").put(b,e);n.onsuccess=()=>{A[a+40>>1]=4;A[a+42>>1]=200;R("OK",a+44,64);c(a,0,e)};n.onerror=h=>{A[a+40>>1]=4;A[a+42>>1]=413;R("Payload Too Large",a+44,64);d(a,0,h)}}catch(h){d(a,0,h)}}else d(a,0,"IndexedDB not available!")}
function db(a,b,c){var d=Ya;if(d){var f=E[a+112+64>>2];f||(f=E[a+8>>2]);f=O(f);try{var k=d.transaction(["FILES"],"readonly").objectStore("FILES").get(f);k.onsuccess=e=>{if(e.target.result){e=e.target.result;var n=e.byteLength||e.length,h=S(n);y.set(new Uint8Array(e),h);E[a+12>>2]=h;V(a+16,n);V(a+24,0);V(a+32,n);A[a+40>>1]=4;A[a+42>>1]=200;R("OK",a+44,64);b(a,0,e)}else A[a+40>>1]=4,A[a+42>>1]=404,R("Not Found",a+44,64),c(a,0,"no data")};k.onerror=e=>{A[a+40>>1]=4;A[a+42>>1]=404;R("Not Found",a+44,
64);c(a,0,e)}}catch(e){c(a,0,e)}}else c(a,0,"IndexedDB not available!")}function eb(a,b,c){var d=Ya;if(d){var f=E[a+112+64>>2];f||(f=E[a+8>>2]);f=O(f);try{var k=d.transaction(["FILES"],"readwrite").objectStore("FILES").delete(f);k.onsuccess=e=>{e=e.target.result;E[a+12>>2]=0;V(a+16,0);V(a+24,0);V(a+32,0);A[a+40>>1]=4;A[a+42>>1]=200;R("OK",a+44,64);b(a,0,e)};k.onerror=e=>{A[a+40>>1]=4;A[a+42>>1]=404;R("Not Found",a+44,64);c(a,0,e)}}catch(e){c(a,0,e)}}else c(a,0,"IndexedDB not available!")}
var fb=1,gb=[],W=[],hb=[],X=[],Y=[],ib=[],jb=[];function kb(a){for(var b=fb++,c=a.length;c<b;c++)a[c]=null;return b}function lb(a,b){a.va||(a.va=a.getContext,a.getContext=function(d,f){f=a.va(d,f);return"webgl"==d==f instanceof WebGLRenderingContext?f:null});var c=a.getContext("webgl2",b);return c?mb(c,b):0}function mb(a,b){var c=kb(jb);b={handle:c,attributes:b,version:b.Sa,Ma:a};a.canvas&&(a.canvas.Xa=b);jb[c]=b;return c}
var nb,ob=["default","low-power","high-performance"],pb=[null,[],[]],qb=()=>{if("object"==typeof crypto&&"function"==typeof crypto.getRandomValues)return a=>crypto.getRandomValues(a);v("initRandomDevice")},rb=a=>(rb=qb())(a);function sb(a,b,c,d){for(var f=0;f<a;f++){var k=Z[c](),e=k&&kb(d);k&&(k.name=e,d[e]=k);D[b+4*f>>2]=e}}function tb(a){return"]"==a.slice(-1)&&a.lastIndexOf("[")}
function ub(a){var b=Z.Oa,c=b.ya[a];"number"==typeof c&&(b.ya[a]=c=Z.getUniformLocation(b,b.Ka[a]+(0<c?"["+c+"]":"")));return c}T=new Xa;va();(function(a,b){try{var c=indexedDB.open("emscripten_filesystem",1)}catch(d){b(d);return}c.onupgradeneeded=d=>{d=d.target.result;d.objectStoreNames.contains("FILES")&&d.deleteObjectStore("FILES");d.createObjectStore("FILES")};c.onsuccess=d=>a(d.target.result);c.onerror=d=>b(d)})(a=>{Ya=a;wa()},()=>{Ya=!1;wa()});
Object.assign(Xa.prototype,{get(a){return this.wa[a]},has(a){return void 0!==this.wa[a]},Na(a){var b=this.Ia.pop()||this.wa.length;this.wa[b]=a;return b},Ra(a){this.wa[a]=void 0;this.Ia.push(a)}});
var Z,vb={e:(a,b,c,d)=>{v(`Assertion failed: ${O(a)}, at: `+[b?O(b):"unknown filename",c,d?O(d):"unknown function"])},I:function(a,b,c){(new Ga(a)).Ta(b,c);Ha=a;Ia++;throw Ha;},N:function(a){if(T.has(a)){var b=T.get(a);T.Ra(a);0<b.readyState&&4>b.readyState&&b.abort()}},K:()=>!0,r:()=>{v("")},M:()=>performance.now(),P:function(){return!0},ma:function(a,b){function c(d){F.get(a)(d,b)&&requestAnimationFrame(c)}return requestAnimationFrame(c)},L:a=>{var b=y.length;a>>>=0;if(2147483648<a)return!1;for(var c=
1;4>=c;c*=2){var d=b*(1+.2/c);d=Math.min(d,a+100663296);var f=Math;d=Math.max(a,d);a:{f=f.min.call(f,2147483648,d+(65536-d%65536)%65536)-ha.buffer.byteLength+65535>>>16;try{ha.grow(f);na();var k=1;break a}catch(e){}k=void 0}if(k)return!0}return!1},ga:function(a,b,c){a=Ua(a);if(!a)return-4;a.width=b;a.height=c;return 0},la:function(a,b,c,d){return Va(a,b,c,d,2,"keydown")},ka:function(a,b,c,d){return Va(a,b,c,d,3,"keyup")},ja:function(a,b,c,d){return Wa(a,b,c,d,5,"mousedown")},ha:function(a,b,c,d){return Wa(a,
b,c,d,8,"mousemove")},ia:function(a,b,c,d){return Wa(a,b,c,d,6,"mouseup")},O:function(a,b,c,d,f){function k(t){if(U)t();else if(!ia)try{if(t(),!noExitRuntime)try{x=t=x,bb(t)}catch(ca){ab(ca)}}catch(ca){ab(ca)}}var e=a+112,n=E[e+36>>2],h=E[e+40>>2],r=E[e+44>>2],C=E[e+48>>2],B=E[e+52>>2],U=!!(B&64),M=t=>{k(()=>{n?F.get(n)(t):b&&b(t)})},Q=t=>{k(()=>{r?F.get(r)(t):d&&d(t)})},I=t=>{k(()=>{h?F.get(h)(t):c&&c(t)})},H=t=>{k(()=>{C?F.get(C)(t):f&&f(t)})},L=t=>{Za(t,M,I,Q,H)},ba=(t,ca)=>{cb(t,ca.response,da=>
{k(()=>{n?F.get(n)(da):b&&b(da)})},da=>{k(()=>{n?F.get(n)(da):b&&b(da)})})},p=t=>{Za(t,ba,I,Q,H)},J=O(e+0),u=!!(B&16),z=!!(B&4);B=!!(B&32);if("EM_IDB_STORE"===J)L=E[e+84>>2],cb(a,y.slice(L,L+E[e+88>>2]),M,I);else if("EM_IDB_DELETE"===J)eb(a,M,I);else if(u){if(B)return 0;Za(a,z?ba:M,I,Q,H)}else db(a,M,B?I:z?p:L);return a},ea:function(a,b){b>>=2;b={alpha:!!D[b],depth:!!D[b+1],stencil:!!D[b+2],antialias:!!D[b+3],premultipliedAlpha:!!D[b+4],preserveDrawingBuffer:!!D[b+5],powerPreference:ob[D[b+6]],failIfMajorPerformanceCaveat:!!D[b+
7],Sa:D[b+8],ab:D[b+9],$a:D[b+10],Qa:D[b+11],bb:D[b+12],cb:D[b+13]};a=Ua(a);return!a||b.Qa?0:lb(a,b)},fa:function(a){a>>=2;for(var b=0;14>b;++b)D[a+b]=0;D[a]=D[a+1]=D[a+3]=D[a+4]=D[a+8]=D[a+10]=1},da:function(a){nb=jb[a];g.Za=Z=nb&&nb.Ma;return!a||Z?0:-5},w:(a,b,c,d)=>{for(var f=0,k=0;k<c;k++){var e=E[b>>2],n=E[b+4>>2];b+=8;for(var h=0;h<n;h++){var r=y[e+h],C=pb[a];if(0===r||10===r){for(r=0;C[r]&&!(NaN<=r);)++r;r=Fa.decode(C.buffer?C.subarray(0,r):new Uint8Array(C.slice(0,r)));(1===a?fa:m)(r);C.length=
0}else C.push(r)}f+=n}E[d>>2]=f;return 0},J:(a,b)=>{rb(y.subarray(a,a+b));return 0},$:function(a){Z.activeTexture(a)},A:function(a,b){Z.attachShader(W[a],Y[b])},d:function(a,b){35051==a?Z.Ha=b:35052==a&&(Z.Ca=b);Z.bindBuffer(a,gb[b])},u:function(a,b){Z.bindFramebuffer(a,hb[b])},c:function(a,b){Z.bindTexture(a,X[b])},j:function(a){Z.bindVertexArray(ib[a])},ba:function(a,b){Z.blendFunc(a,b)},k:function(a,b,c,d){c&&b?Z.bufferData(a,y,d,c,b):Z.bufferData(a,b,d)},G:function(a){Z.clear(a)},H:function(a,
b,c,d){Z.clearColor(a,b,c,d)},T:function(a){Z.compileShader(Y[a])},Y:function(){var a=kb(W),b=Z.createProgram();b.name=a;b.Ba=b.za=b.Aa=0;b.Da=1;W[a]=b;return a},V:function(a){var b=kb(Y);Y[b]=Z.createShader(a);return b},q:function(a,b){for(var c=0;c<a;c++){var d=D[b+4*c>>2],f=gb[d];f&&(Z.deleteBuffer(f),f.name=0,gb[d]=null,d==Z.Ha&&(Z.Ha=0),d==Z.Ca&&(Z.Ca=0))}},x:function(a,b){for(var c=0;c<a;++c){var d=D[b+4*c>>2],f=hb[d];f&&(Z.deleteFramebuffer(f),f.name=0,hb[d]=null)}},l:function(a){if(a){var b=
W[a];b&&(Z.deleteProgram(b),b.name=0,W[a]=null)}},f:function(a){if(a){var b=Y[a];b&&(Z.deleteShader(b),Y[a]=null)}},a:function(a,b){for(var c=0;c<a;c++){var d=D[b+4*c>>2],f=X[d];f&&(Z.deleteTexture(f),f.name=0,X[d]=null)}},E:function(a,b){for(var c=0;c<a;c++){var d=D[b+4*c>>2];Z.deleteVertexArray(ib[d]);ib[d]=null}},F:function(a){Z.disable(a)},t:function(a,b,c,d){Z.drawArraysInstanced(a,b,c,d)},Z:function(a,b,c,d){Z.drawElements(a,b,c,d)},ca:function(a){Z.enable(a)},g:function(a){Z.enableVertexAttribArray(a)},
oa:function(a,b,c,d,f){Z.framebufferTexture2D(a,b,c,X[d],f)},p:function(a,b){sb(a,b,"createBuffer",gb)},R:function(a,b){sb(a,b,"createFramebuffer",hb)},n:function(a,b){sb(a,b,"createTexture",X)},B:function(a,b){sb(a,b,"createVertexArray",ib)},i:function(a,b){return Z.getAttribLocation(W[a],O(b))},W:function(a,b,c,d){a=Z.getProgramInfoLog(W[a]);b=0<b&&d?R(a,d,b):0;c&&(D[c>>2]=b)},z:function(a,b,c){if(c&&!(a>=fb))if(a=W[a],35716==b)D[c>>2]=Z.getProgramInfoLog(a).length+1;else if(35719==b){if(!a.Ba)for(b=
0;b<Z.getProgramParameter(a,35718);++b)a.Ba=Math.max(a.Ba,Z.getActiveUniform(a,b).name.length+1);D[c>>2]=a.Ba}else if(35722==b){if(!a.za)for(b=0;b<Z.getProgramParameter(a,35721);++b)a.za=Math.max(a.za,Z.getActiveAttrib(a,b).name.length+1);D[c>>2]=a.za}else if(35381==b){if(!a.Aa)for(b=0;b<Z.getProgramParameter(a,35382);++b)a.Aa=Math.max(a.Aa,Z.getActiveUniformBlockName(a,b).length+1);D[c>>2]=a.Aa}else D[c>>2]=Z.getProgramParameter(a,b)},S:function(a,b,c,d){a=Z.getShaderInfoLog(Y[a]);b=0<b&&d?R(a,d,
b):0;c&&(D[c>>2]=b)},y:function(a,b,c){c&&(35716==b?(a=Z.getShaderInfoLog(Y[a]),D[c>>2]=a?a.length+1:0):35720==b?(a=Z.getShaderSource(Y[a]),D[c>>2]=a?a.length+1:0):D[c>>2]=Z.getShaderParameter(Y[a],b))},s:function(a,b){b=O(b);if(a=W[a]){var c=a,d=c.ya,f=c.La,k;if(!d)for(c.ya=d={},c.Ka={},k=0;k<Z.getProgramParameter(c,35718);++k){var e=Z.getActiveUniform(c,k);var n=e.name;e=e.size;var h=tb(n);h=0<h?n.slice(0,h):n;var r=c.Da;c.Da+=e;f[h]=[e,r];for(n=0;n<e;++n)d[r]=n,c.Ka[r++]=h}c=a.ya;d=0;f=b;k=tb(b);
0<k&&(d=parseInt(b.slice(k+1))>>>0,f=b.slice(0,k));if((f=a.La[f])&&d<f[0]&&(d+=f[1],c[d]=c[d]||Z.getUniformLocation(a,b)))return d}return-1},X:function(a){a=W[a];Z.linkProgram(a);a.ya=0;a.La={}},U:function(a,b,c,d){for(var f="",k=0;k<b;++k){var e=d?D[d+4*k>>2]:-1;f+=O(D[c+4*k>>2],0>e?void 0:e)}Z.shaderSource(Y[a],f)},v:function(a,b,c,d,f,k,e,n,h){if(Z.Ca)Z.texImage2D(a,b,c,d,f,k,e,n,h);else if(h){var r=n-5120;r=0==r?ja:1==r?y:2==r?ka:4==r?D:6==r?la:5==r||28922==r||28520==r||30779==r||30782==r?E:A;
Z.texImage2D(a,b,c,d,f,k,e,n,r,h>>31-Math.clz32(r.BYTES_PER_ELEMENT))}else Z.texImage2D(a,b,c,d,f,k,e,n,null)},b:function(a,b,c){Z.texParameteri(a,b,c)},_:function(a,b){Z.uniform1i(ub(a),b)},C:function(a,b,c){Z.uniform2f(ub(a),b,c)},D:function(a){a=W[a];Z.useProgram(a);Z.Oa=a},o:function(a,b){Z.vertexAttribDivisor(a,b)},h:function(a,b,c,d,f,k){Z.vertexAttribPointer(a,b,c,!!d,f,k)},m:function(a,b,c,d){Z.viewport(a,b,c,d)},Q:function(a,b,c,d){var f=document.createElement("canvas");f.width=b;f.height=
c;b=f.getContext("2d");b.font=a+"px "+O(d);b.textBaseline="middle";b.globalAlpha=1;b.fillStyle="white";window["gCanvas"+a]=f;window["gCanvasCtx"+a]=b},na:function(a,b,c,d){var f=new Image;f.onload=()=>{E[c>>2]=f.width;E[d>>2]=f.height;Z.bindTexture(Z.TEXTURE_2D,X[a]);Z.texImage2D(Z.TEXTURE_2D,0,Z.RGBA,Z.RGBA,Z.UNSIGNED_BYTE,f)};f.src=O(b)},aa:function(a,b){var c=window["gCanvas"+a];a=window["gCanvasCtx"+a];a.clearRect(0,0,c.width,c.height);b=O(b);var d=a.measureText(b).width;a.fillText(b,0,c.height/
2);Z.texImage2D(Z.TEXTURE_2D,0,Z.RGBA,Z.RGBA,Z.UNSIGNED_BYTE,c);return d}};(function(){function a(c){w=c=c.exports;ha=w.pa;na();F=w.ta;pa.unshift(w.qa);wa();return c}var b={a:vb};va();if(g.instantiateWasm)try{return g.instantiateWasm(b,a)}catch(c){return m("Module.instantiateWasm callback failed with error: "+c),!1}Ca(b,function(c){a(c.instance)});return{}})();var S=a=>(S=w.ra)(a),$a=a=>($a=w.sa)(a),wb=g._main=(a,b)=>(wb=g._main=w.ua)(a,b),xb;K=function yb(){xb||zb();xb||(K=yb)};
function zb(){function a(){if(!xb&&(xb=!0,g.calledRun=!0,!ia)){Ea(pa);Ea(qa);if(g.onRuntimeInitialized)g.onRuntimeInitialized();if(Ab){var b=wb;try{var c=b(0,0);x=c;bb(c)}catch(d){ab(d)}}if(g.postRun)for("function"==typeof g.postRun&&(g.postRun=[g.postRun]);g.postRun.length;)b=g.postRun.shift(),sa.unshift(b);Ea(sa)}}if(!(0<G)){if(g.preRun)for("function"==typeof g.preRun&&(g.preRun=[g.preRun]);g.preRun.length;)ta();Ea(oa);0<G||(g.setStatus?(g.setStatus("Running..."),setTimeout(function(){setTimeout(function(){g.setStatus("")},
1);a()},1)):a())}}if(g.preInit)for("function"==typeof g.preInit&&(g.preInit=[g.preInit]);0<g.preInit.length;)g.preInit.pop()();var Ab=!0;g.noInitialRun&&(Ab=!1);zb();
