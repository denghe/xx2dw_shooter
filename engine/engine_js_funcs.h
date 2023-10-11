#pragma once
#include <engine_includes.h>

// code at library_js.js
extern "C" {
void init_gCanvas(int charSize, int width, int height, const char* font);
double upload_unicode_char_to_texture(int charSize, const char *unicodeChar);
void load_texture_from_url(GLuint texture, const char *url, int *outWidth, int *outHeight);
}
