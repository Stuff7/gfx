#pragma once

#include "../defs.h"
#include "../renderer.h"

#define WIN_WIDTH 1000.f
#define WIN_HEIGHT 600.f

typedef struct {
  GlyphRenderer renderer;
  uint shader;
  Camera camera;
  Mat4 view;
  Mat4 proj;
  Mat4 model;
  Transform transform;
  uint idx;
  Glyph glyph;
  NormalizedGlyph normalGlyph;
  GlyphParser glyphParser;
  Bitstream bs;

  struct {
    f64 x, y;
    bool capture;
  } cursor;
} RenderGlyphsState;

void RenderGlyphsState_free(RenderGlyphsState *state);
Result *RenderGlyphsState_createScene(RenderGlyphsState *state, const char *ttfPath, const char *windowTitle);
