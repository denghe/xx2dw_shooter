var e;e||(e=typeof Module !== 'undefined' ? Module : {});var aa=Object.assign({},e),h=(a,b)=>{throw b;},k="";"undefined"!=typeof document&&document.currentScript&&(k=document.currentScript.src);k=0!==k.indexOf("blob:")?k.substr(0,k.replace(/[?#].*/,"").lastIndexOf("/")+1):"";var ba=e.print||console.log.bind(console),q=e.printErr||console.error.bind(console);Object.assign(e,aa);aa=null;e.quit&&(h=e.quit);var r;e.wasmBinary&&(r=e.wasmBinary);var noExitRuntime=e.noExitRuntime||!0;
"object"!=typeof WebAssembly&&t("no native wasm support detected");var u,v,w=!1,ca,x,y,da,z,A,ea,B;function fa(){var a=u.buffer;e.HEAP8=ca=new Int8Array(a);e.HEAP16=y=new Int16Array(a);e.HEAP32=z=new Int32Array(a);e.HEAPU8=x=new Uint8Array(a);e.HEAPU16=da=new Uint16Array(a);e.HEAPU32=A=new Uint32Array(a);e.HEAPF32=ea=new Float32Array(a);e.HEAPF64=B=new Float64Array(a)}var C,ha=[],ia=[],ja=[],ka=[],la=[];function ma(){var a=e.preRun.shift();ha.unshift(a)}var D=0,F=null,G=null;
function t(a){if(e.onAbort)e.onAbort(a);a="Aborted("+a+")";q(a);w=!0;throw new WebAssembly.RuntimeError(a+". Build with -sASSERTIONS for more info.");}function na(a){return a.startsWith("data:application/octet-stream;base64,")}var H;H="s11.wasm";if(!na(H)){var oa=H;H=e.locateFile?e.locateFile(oa,k):k+oa}function pa(a){if(a==H&&r)return new Uint8Array(r);throw"both async and sync fetching of the wasm failed";}
function qa(a){return r||"function"!=typeof fetch?Promise.resolve().then(()=>pa(a)):fetch(a,{credentials:"same-origin"}).then(b=>{if(!b.ok)throw"failed to load wasm binary file at '"+a+"'";return b.arrayBuffer()}).catch(()=>pa(a))}function ra(a,b,c){return qa(a).then(d=>WebAssembly.instantiate(d,b)).then(d=>d).then(c,d=>{q("failed to asynchronously prepare wasm: "+d);t(d)})}
function sa(a,b){var c=H;r||"function"!=typeof WebAssembly.instantiateStreaming||na(c)||"function"!=typeof fetch?ra(c,a,b):fetch(c,{credentials:"same-origin"}).then(d=>WebAssembly.instantiateStreaming(d,a).then(b,function(g){q("wasm streaming compile failed: "+g);q("falling back to ArrayBuffer instantiation");return ra(c,a,b)}))}function ta(a){this.name="ExitStatus";this.message=`Program terminated with exit(${a})`;this.status=a}
var I=a=>{for(;0<a.length;)a.shift()(e)},ua=new TextDecoder("utf8"),J=(a,b)=>{if(!a)return"";b=a+b;for(var c=a;!(c>=b)&&x[c];)++c;return ua.decode(x.subarray(a,c))};function va(a){this.ra=a-24;this.Oa=function(b){A[this.ra+4>>2]=b};this.Na=function(b){A[this.ra+8>>2]=b};this.La=function(b,c){this.Ma();this.Oa(b);this.Na(c)};this.Ma=function(){A[this.ra+16>>2]=0}}var wa=0,xa=0,K=0;function ya(){for(var a=L.length-1;0<=a;--a)za(a);L=[];M=[]}var M=[];
function Aa(){if(navigator.userActivation?navigator.userActivation.isActive:K&&Ba.Aa)for(var a=0;a<M.length;++a){var b=M[a];M.splice(a,1);--a;b.Wa.apply(null,b.Qa)}}var L=[];function za(a){var b=L[a];b.target.removeEventListener(b.sa,b.Ia,b.za);L.splice(a,1)}
function Ca(a){function b(d){++K;Ba=a;Aa();a.Da(d);Aa();--K}if(!a.target)return-4;if(a.Ba)a.Ia=b,a.target.addEventListener(a.sa,b,a.za),L.push(a),Da||(ka.push(ya),Da=!0);else for(var c=0;c<L.length;++c)L[c].target==a.target&&L[c].sa==a.sa&&za(c--);return 0}var Da,Ba,Ea,N,Fa=[0,document,window];function O(a){a=2<a?J(a):a;return Fa[a]||document.querySelector(a)}
var P=(a,b,c)=>{var d=x;if(!(0<c))return 0;var g=b;c=b+c-1;for(var l=0;l<a.length;++l){var f=a.charCodeAt(l);if(55296<=f&&57343>=f){var n=a.charCodeAt(++l);f=65536+((f&1023)<<10)|n&1023}if(127>=f){if(b>=c)break;d[b++]=f}else{if(2047>=f){if(b+1>=c)break;d[b++]=192|f>>6}else{if(65535>=f){if(b+2>=c)break;d[b++]=224|f>>12}else{if(b+3>=c)break;d[b++]=240|f>>18;d[b++]=128|f>>12&63}d[b++]=128|f>>6&63}d[b++]=128|f&63}}d[b]=0;return b-g};
function Ga(a,b,c,d,g,l){Ea||(Ea=Ha(176));a={target:O(a),Aa:!0,sa:l,Ba:d,Da:function(f){var n=Ea;B[n>>3]=f.timeStamp;var m=n>>2;z[m+2]=f.location;z[m+3]=f.ctrlKey;z[m+4]=f.shiftKey;z[m+5]=f.altKey;z[m+6]=f.metaKey;z[m+7]=f.repeat;z[m+8]=f.charCode;z[m+9]=f.keyCode;z[m+10]=f.which;P(f.key||"",n+44,32);P(f.code||"",n+76,32);P(f.char||"",n+108,32);P(f.locale||"",n+140,32);C.get(d)(g,n,b)&&f.preventDefault()},za:c};return Ca(a)}
function Ia(a,b,c,d,g,l){N||(N=Ha(72));a=O(a);return Ca({target:a,Aa:"mousemove"!=l&&"mouseenter"!=l&&"mouseleave"!=l,sa:l,Ba:d,Da:function(f=event){var n=a,m=N;B[m>>3]=f.timeStamp;m>>=2;z[m+2]=f.screenX;z[m+3]=f.screenY;z[m+4]=f.clientX;z[m+5]=f.clientY;z[m+6]=f.ctrlKey;z[m+7]=f.shiftKey;z[m+8]=f.altKey;z[m+9]=f.metaKey;y[2*m+20]=f.button;y[2*m+21]=f.buttons;z[m+11]=f.movementX;z[m+12]=f.movementY;n=0>Fa.indexOf(n)?n.getBoundingClientRect():{left:0,top:0};z[m+13]=f.clientX-n.left;z[m+14]=f.clientY-
n.top;C.get(d)(g,N,b)&&f.preventDefault()},za:c})}var Ja=1,Q=[],R=[],S=[],T=[],U=[],V=[],Ka=[];function W(a){for(var b=Ja++,c=a.length;c<b;c++)a[c]=null;return b}function La(a,b){a.ra||(a.ra=a.getContext,a.getContext=function(d,g){g=a.ra(d,g);return"webgl"==d==g instanceof WebGLRenderingContext?g:null});var c=a.getContext("webgl2",b);return c?Ma(c,b):0}function Ma(a,b){var c=W(Ka);b={handle:c,attributes:b,version:b.Ka,Ga:a};a.canvas&&(a.canvas.Pa=b);Ka[c]=b;return c}
var Na,Oa=["default","low-power","high-performance"],Pa=[null,[],[]],Qa=()=>{if("object"==typeof crypto&&"function"==typeof crypto.getRandomValues)return a=>crypto.getRandomValues(a);t("initRandomDevice")},Ra=a=>(Ra=Qa())(a);function X(a,b,c,d){for(var g=0;g<a;g++){var l=Y[c](),f=l&&W(d);l&&(l.name=f,d[f]=l);z[b+4*g>>2]=f}}function Sa(a){return"]"==a.slice(-1)&&a.lastIndexOf("[")}
function Ta(a){var b=Y.Ha,c=b.ta[a];"number"==typeof c&&(b.ta[a]=c=Y.getUniformLocation(b,b.Ea[a]+(0<c?"["+c+"]":"")));return c}
var Y,Ua={e:(a,b,c,d)=>{t(`Assertion failed: ${J(a)}, at: `+[b?J(b):"unknown filename",c,d?J(d):"unknown function"])},s:function(a,b,c){(new va(a)).La(b,c);wa=a;xa++;throw wa;},K:()=>!0,q:()=>{t("")},M:()=>performance.now(),la:function(a,b){function c(d){C.get(a)(d,b)&&requestAnimationFrame(c)}return requestAnimationFrame(c)},L:a=>{var b=x.length;a>>>=0;if(2147483648<a)return!1;for(var c=1;4>=c;c*=2){var d=b*(1+.2/c);d=Math.min(d,a+100663296);var g=Math;d=Math.max(a,d);a:{g=g.min.call(g,2147483648,
d+(65536-d%65536)%65536)-u.buffer.byteLength+65535>>>16;try{u.grow(g);fa();var l=1;break a}catch(f){}l=void 0}if(l)return!0}return!1},da:function(a,b,c){a=O(a);if(!a)return-4;a.width=b;a.height=c;return 0},ia:function(a,b,c,d){return Ga(a,b,c,d,2,"keydown")},ha:function(a,b,c,d){return Ga(a,b,c,d,3,"keyup")},ga:function(a,b,c,d){return Ia(a,b,c,d,5,"mousedown")},ea:function(a,b,c,d){return Ia(a,b,c,d,8,"mousemove")},fa:function(a,b,c,d){return Ia(a,b,c,d,6,"mouseup")},ba:function(a,b){b>>=2;b={alpha:!!z[b],
depth:!!z[b+1],stencil:!!z[b+2],antialias:!!z[b+3],premultipliedAlpha:!!z[b+4],preserveDrawingBuffer:!!z[b+5],powerPreference:Oa[z[b+6]],failIfMajorPerformanceCaveat:!!z[b+7],Ka:z[b+8],Ta:z[b+9],Sa:z[b+10],Ja:z[b+11],Ua:z[b+12],Va:z[b+13]};a=O(a);return!a||b.Ja?0:La(a,b)},ca:function(a){a>>=2;for(var b=0;14>b;++b)z[a+b]=0;z[a]=z[a+1]=z[a+3]=z[a+4]=z[a+8]=z[a+10]=1},aa:function(a){Na=Ka[a];e.Ra=Y=Na&&Na.Ga;return!a||Y?0:-5},u:(a,b,c,d)=>{for(var g=0,l=0;l<c;l++){var f=A[b>>2],n=A[b+4>>2];b+=8;for(var m=
0;m<n;m++){var p=x[f+m],E=Pa[a];if(0===p||10===p){for(p=0;E[p]&&!(NaN<=p);)++p;p=ua.decode(E.buffer?E.subarray(0,p):new Uint8Array(E.slice(0,p)));(1===a?ba:q)(p);E.length=0}else E.push(p)}g+=n}A[d>>2]=g;return 0},J:(a,b)=>{Ra(x.subarray(a,a+b));return 0},Z:function(a){Y.activeTexture(a)},y:function(a,b){Y.attachShader(R[a],U[b])},c:function(a,b){35051==a?Y.Ca=b:35052==a&&(Y.xa=b);Y.bindBuffer(a,Q[b])},G:function(a,b){Y.bindFramebuffer(a,S[b])},b:function(a,b){Y.bindTexture(a,T[b])},i:function(a){Y.bindVertexArray(V[a])},
_:function(a,b){Y.blendFunc(a,b)},j:function(a,b,c,d){c&&b?Y.bufferData(a,x,d,c,b):Y.bufferData(a,b,d)},A:function(a){Y.clear(a)},E:function(a,b,c,d){Y.clearColor(a,b,c,d)},R:function(a){Y.compileShader(U[a])},W:function(){var a=W(R),b=Y.createProgram();b.name=a;b.wa=b.ua=b.va=0;b.ya=1;R[a]=b;return a},T:function(a){var b=W(U);U[b]=Y.createShader(a);return b},n:function(a,b){for(var c=0;c<a;c++){var d=z[b+4*c>>2],g=Q[d];g&&(Y.deleteBuffer(g),g.name=0,Q[d]=null,d==Y.Ca&&(Y.Ca=0),d==Y.xa&&(Y.xa=0))}},
v:function(a,b){for(var c=0;c<a;++c){var d=z[b+4*c>>2],g=S[d];g&&(Y.deleteFramebuffer(g),g.name=0,S[d]=null)}},k:function(a){if(a){var b=R[a];b&&(Y.deleteProgram(b),b.name=0,R[a]=null)}},d:function(a){if(a){var b=U[a];b&&(Y.deleteShader(b),U[a]=null)}},ka:function(a,b){for(var c=0;c<a;c++){var d=z[b+4*c>>2],g=T[d];g&&(Y.deleteTexture(g),g.name=0,T[d]=null)}},D:function(a,b){for(var c=0;c<a;c++){var d=z[b+4*c>>2];Y.deleteVertexArray(V[d]);V[d]=null}},F:function(a){Y.disable(a)},t:function(a,b,c,d){Y.drawArraysInstanced(a,
b,c,d)},X:function(a,b,c,d){Y.drawElements(a,b,c,d)},$:function(a){Y.enable(a)},f:function(a){Y.enableVertexAttribArray(a)},ja:function(a,b,c,d,g){Y.framebufferTexture2D(a,b,c,T[d],g)},m:function(a,b){X(a,b,"createBuffer",Q)},P:function(a,b){X(a,b,"createFramebuffer",S)},p:function(a,b){X(a,b,"createTexture",T)},z:function(a,b){X(a,b,"createVertexArray",V)},h:function(a,b){return Y.getAttribLocation(R[a],J(b))},U:function(a,b,c,d){a=Y.getProgramInfoLog(R[a]);b=0<b&&d?P(a,d,b):0;c&&(z[c>>2]=b)},x:function(a,
b,c){if(c&&!(a>=Ja))if(a=R[a],35716==b)z[c>>2]=Y.getProgramInfoLog(a).length+1;else if(35719==b){if(!a.wa)for(b=0;b<Y.getProgramParameter(a,35718);++b)a.wa=Math.max(a.wa,Y.getActiveUniform(a,b).name.length+1);z[c>>2]=a.wa}else if(35722==b){if(!a.ua)for(b=0;b<Y.getProgramParameter(a,35721);++b)a.ua=Math.max(a.ua,Y.getActiveAttrib(a,b).name.length+1);z[c>>2]=a.ua}else if(35381==b){if(!a.va)for(b=0;b<Y.getProgramParameter(a,35382);++b)a.va=Math.max(a.va,Y.getActiveUniformBlockName(a,b).length+1);z[c>>
2]=a.va}else z[c>>2]=Y.getProgramParameter(a,b)},Q:function(a,b,c,d){a=Y.getShaderInfoLog(U[a]);b=0<b&&d?P(a,d,b):0;c&&(z[c>>2]=b)},w:function(a,b,c){c&&(35716==b?(a=Y.getShaderInfoLog(U[a]),z[c>>2]=a?a.length+1:0):35720==b?(a=Y.getShaderSource(U[a]),z[c>>2]=a?a.length+1:0):z[c>>2]=Y.getShaderParameter(U[a],b))},r:function(a,b){b=J(b);if(a=R[a]){var c=a,d=c.ta,g=c.Fa,l;if(!d)for(c.ta=d={},c.Ea={},l=0;l<Y.getProgramParameter(c,35718);++l){var f=Y.getActiveUniform(c,l);var n=f.name;f=f.size;var m=Sa(n);
m=0<m?n.slice(0,m):n;var p=c.ya;c.ya+=f;g[m]=[f,p];for(n=0;n<f;++n)d[p]=n,c.Ea[p++]=m}c=a.ta;d=0;g=b;l=Sa(b);0<l&&(d=parseInt(b.slice(l+1))>>>0,g=b.slice(0,l));if((g=a.Fa[g])&&d<g[0]&&(d+=g[1],c[d]=c[d]||Y.getUniformLocation(a,b)))return d}return-1},V:function(a){a=R[a];Y.linkProgram(a);a.ta=0;a.Fa={}},S:function(a,b,c,d){for(var g="",l=0;l<b;++l){var f=d?z[d+4*l>>2]:-1;g+=J(z[c+4*l>>2],0>f?void 0:f)}Y.shaderSource(U[a],g)},H:function(a,b,c,d,g,l,f,n,m){if(Y.xa)Y.texImage2D(a,b,c,d,g,l,f,n,m);else if(m){var p=
n-5120;p=0==p?ca:1==p?x:2==p?y:4==p?z:6==p?ea:5==p||28922==p||28520==p||30779==p||30782==p?A:da;Y.texImage2D(a,b,c,d,g,l,f,n,p,m>>31-Math.clz32(p.BYTES_PER_ELEMENT))}else Y.texImage2D(a,b,c,d,g,l,f,n,null)},a:function(a,b,c){Y.texParameteri(a,b,c)},Y:function(a,b){Y.uniform1i(Ta(a),b)},B:function(a,b,c){Y.uniform2f(Ta(a),b,c)},C:function(a){a=R[a];Y.useProgram(a);Y.Ha=a},l:function(a,b){Y.vertexAttribDivisor(a,b)},g:function(a,b,c,d,g,l){Y.vertexAttribPointer(a,b,c,!!d,g,l)},o:function(a,b,c,d){Y.viewport(a,
b,c,d)},O:function(a,b,c,d){var g=document.createElement("canvas");g.width=b;g.height=c;b=g.getContext("2d");b.font=a+"px "+J(d);b.textBaseline="middle";b.globalAlpha=1;b.fillStyle="white";window["gCanvas"+a]=g;window["gCanvasCtx"+a]=b},N:function(a,b,c,d){var g=new Image;g.onload=()=>{A[c>>2]=g.width;A[d>>2]=g.height;Y.bindTexture(Y.TEXTURE_2D,T[a]);Y.texImage2D(Y.TEXTURE_2D,0,Y.RGBA,Y.RGBA,Y.UNSIGNED_BYTE,g)};g.src=J(b)},I:function(a,b){var c=window["gCanvas"+a];a=window["gCanvasCtx"+a];a.clearRect(0,
0,c.width,c.height);b=J(b);var d=a.measureText(b).width;a.fillText(b,0,c.height/2);Y.texImage2D(Y.TEXTURE_2D,0,Y.RGBA,Y.RGBA,Y.UNSIGNED_BYTE,c);return d}};
(function(){function a(c){v=c=c.exports;u=v.ma;fa();C=v.qa;ia.unshift(v.na);D--;e.monitorRunDependencies&&e.monitorRunDependencies(D);if(0==D&&(null!==F&&(clearInterval(F),F=null),G)){var d=G;G=null;d()}return c}var b={a:Ua};D++;e.monitorRunDependencies&&e.monitorRunDependencies(D);if(e.instantiateWasm)try{return e.instantiateWasm(b,a)}catch(c){return q("Module.instantiateWasm callback failed with error: "+c),!1}sa(b,function(c){a(c.instance)});return{}})();
var Ha=a=>(Ha=v.oa)(a),Va=e._main=(a,b)=>(Va=e._main=v.pa)(a,b),Z;G=function Wa(){Z||Xa();Z||(G=Wa)};
function Xa(){function a(){if(!Z&&(Z=!0,e.calledRun=!0,!w)){I(ia);I(ja);if(e.onRuntimeInitialized)e.onRuntimeInitialized();if(Ya){var b=Va;try{var c=b(0,0);if(!noExitRuntime){if(e.onExit)e.onExit(c);w=!0}h(c,new ta(c))}catch(d){d instanceof ta||"unwind"==d||h(1,d)}}if(e.postRun)for("function"==typeof e.postRun&&(e.postRun=[e.postRun]);e.postRun.length;)b=e.postRun.shift(),la.unshift(b);I(la)}}if(!(0<D)){if(e.preRun)for("function"==typeof e.preRun&&(e.preRun=[e.preRun]);e.preRun.length;)ma();I(ha);
0<D||(e.setStatus?(e.setStatus("Running..."),setTimeout(function(){setTimeout(function(){e.setStatus("")},1);a()},1)):a())}}if(e.preInit)for("function"==typeof e.preInit&&(e.preInit=[e.preInit]);0<e.preInit.length;)e.preInit.pop()();var Ya=!0;e.noInitialRun&&(Ya=!1);Xa();
