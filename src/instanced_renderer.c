#include "math.h"
#include "renderer.h"
#include <stddef.h>
#include <stdlib.h>

InstancedRenderer InstancedRenderer_new(
    const void *vertices,
    uint verticesSize,
    const uint *indices,
    uint indicesSize,
    uint indicesLen,
    uint maxInstances,
    bool dynamic
) {
  InstancedRenderer self;

  VAO_new(&self.vao);
  VAO_bind(self.vao);

  self.vbo = Buffer_new(BufferType_Array);
  Buffer_data(&self.vbo, verticesSize, vertices, false);

  self.ibo = Buffer_new(BufferType_Element);
  self.indicesLen = indicesLen;
  Buffer_data(&self.ibo, indicesSize, indices, false);
  VAO_attrib(self.vao, &self.vbo, 0, 3, AttribType_Float, false, sizeof(Vec3), offsetof(Vec3, x));

  self.colors.buf = Buffer_new(BufferType_Array);
  Buffer_data(&self.colors.buf, maxInstances * sizeof(Color), NULL, dynamic);
  VAO_attrib(self.vao, &self.colors.buf, 1, 3, AttribType_Byte, true, sizeof(Color), offsetof(Color, r));
  VAO_attribDivisor(self.vao, &self.colors.buf, 1, 1);

  self.models.buf = Buffer_new(BufferType_Array);
  Buffer_data(&self.models.buf, maxInstances * sizeof(Mat4), NULL, dynamic);
  VAO_attrib(self.vao, &self.models.buf, 2, 4, AttribType_Float, false, sizeof(Mat4), offsetof(Mat4, m1x1));
  VAO_attrib(self.vao, &self.models.buf, 3, 4, AttribType_Float, false, sizeof(Mat4), offsetof(Mat4, m1x2));
  VAO_attrib(self.vao, &self.models.buf, 4, 4, AttribType_Float, false, sizeof(Mat4), offsetof(Mat4, m1x3));
  VAO_attrib(self.vao, &self.models.buf, 5, 4, AttribType_Float, false, sizeof(Mat4), offsetof(Mat4, m1x4));
  VAO_attribDivisor(self.vao, &self.models.buf, 2, 1);
  VAO_attribDivisor(self.vao, &self.models.buf, 3, 1);
  VAO_attribDivisor(self.vao, &self.models.buf, 4, 1);
  VAO_attribDivisor(self.vao, &self.models.buf, 5, 1);

  return self;
}

void DataBuffer_set(DataBuffer *dbo, uint len, uint instanceSize, void *data) {
  if (dbo->len != len) {
    dbo->len = len;
    dbo->size = dbo->len * instanceSize;
  }

  Buffer_subData(&dbo->buf, 0, dbo->size, data);
}

void DataBuffer_sliceSet(DataBuffer *dbo, uint offset, uint size, void *data) {
  Buffer_subData(&dbo->buf, offset, size, data + offset);
}

void InstancedRenderer_draw(InstancedRenderer *self) { VAO_drawElementsInstanced(self->indicesLen, self->models.len); }

void InstancedRenderer_free(InstancedRenderer *self) {
  VAO_free(&self->vao);
  Buffer_free(&self->vbo);
  Buffer_free(&self->ibo);
  Buffer_free(&self->models.buf);
  Buffer_free(&self->colors.buf);
}
