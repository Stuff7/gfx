#include "renderer.h"
#include <stddef.h>

GlyphRenderer
GlyphRenderer_new(const void *vertices, uint verticesSize, const uint *indices, uint indicesSize, uint indicesLen) {
  GlyphRenderer self;

  VAO_new(&self.vao);
  VAO_bind(self.vao);

  self.vbo = Buffer_new(BufferType_Array);
  Buffer_data(&self.vbo, verticesSize, vertices, false);

  self.ibo = Buffer_new(BufferType_Element);
  self.indicesLen = indicesLen;
  Buffer_data(&self.ibo, indicesSize, indices, false);
  VAO_attrib(self.vao, &self.vbo, 0, 2, AttribType_Float, false, sizeof(Vec2), offsetof(Vec2, x));

  return self;
}

void GlyphRenderer_draw(GlyphRenderer *self) { VAO_drawElements(self->indicesLen); }

void GlyphRenderer_free(GlyphRenderer *self) {
  VAO_free(&self->vao);
  Buffer_free(&self->vbo);
  Buffer_free(&self->ibo);
}
