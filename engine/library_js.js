mergeInto(LibraryManager.library, {

  init_gCanvas: function(charSize, canvasWidth, canvasHeight, font) {
    if (window['gCanvas'] === undefined) {
      var canvas = document.createElement('canvas');
      canvas.width = canvasWidth;
      canvas.height = canvasHeight;
      var ctx = canvas.getContext('2d');
        ctx.font = charSize + 'px Arial';// + font;
      ctx.textBaseline = "middle";
      ctx.globalAlpha = 1;
      ctx.fillStyle = 'white';
      window['gCanvas' + charSize] = canvas;
      window['gCanvasCtx' + charSize] = ctx;
    }
  },

  upload_unicode_char_to_texture: function(charSize, utf8Char) {
    var canvas = window['gCanvas' + charSize];
    var ctx = window['gCanvasCtx' + charSize];
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    var str = UTF8ToString(utf8Char);
    var strPixelWidth = ctx.measureText(str).width;
    ctx.fillText(str, 0, canvas.height / 2);
    GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, canvas);
    return strPixelWidth;
  },

  load_texture_from_url: function(glTexture, url, outW, outH) {
    var img = new Image();
    img.onload = () => {
      HEAPU32[outW>>2] = img.width;
      HEAPU32[outH>>2] = img.height;
      GLctx.bindTexture(GLctx.TEXTURE_2D, GL.textures[glTexture]);
      GLctx.texImage2D(GLctx.TEXTURE_2D, 0, GLctx.RGBA, GLctx.RGBA, GLctx.UNSIGNED_BYTE, img);
    };
  img.src = UTF8ToString(url);
  }

});
