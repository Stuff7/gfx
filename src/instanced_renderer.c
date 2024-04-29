#include "math.h"
#include "renderer.h"
#include <stddef.h>
#include <stdlib.h>

InstancedRenderer InstancedRendererNew(const void *vertices, uint verticesSize, const uint *indices, uint indicesSize,
                                       uint indicesLen, uint maxInstances, bool dynamic) {
  InstancedRenderer self;

  VAONew(&self.vao);
  VAOBind(self.vao);

  self.vbo = BufferNew(BufferType_Array);
  BufferData(&self.vbo, verticesSize, vertices, false);

  self.ibo = BufferNew(BufferType_Element);
  self.indicesLen = indicesLen;
  BufferData(&self.ibo, indicesSize, indices, false);
  VAOAttrib(self.vao, &self.vbo, 0, 3, AttribType_Float, false, sizeof(Vec3), offsetof(Vec3, x));

  self.colors.buf = BufferNew(BufferType_Array);
  BufferData(&self.colors.buf, maxInstances * sizeof(Color), NULL, dynamic);
  VAOAttrib(self.vao, &self.colors.buf, 1, 3, AttribType_Byte, true, sizeof(Color), offsetof(Color, r));
  VAOAttribDivisor(self.vao, &self.colors.buf, 1, 1);

  self.models.buf = BufferNew(BufferType_Array);
  BufferData(&self.models.buf, maxInstances * sizeof(Mat4), NULL, dynamic);
  VAOAttrib(self.vao, &self.models.buf, 2, 4, AttribType_Float, false, sizeof(Mat4), offsetof(Mat4, m1x1));
  VAOAttrib(self.vao, &self.models.buf, 3, 4, AttribType_Float, false, sizeof(Mat4), offsetof(Mat4, m1x2));
  VAOAttrib(self.vao, &self.models.buf, 4, 4, AttribType_Float, false, sizeof(Mat4), offsetof(Mat4, m1x3));
  VAOAttrib(self.vao, &self.models.buf, 5, 4, AttribType_Float, false, sizeof(Mat4), offsetof(Mat4, m1x4));
  VAOAttribDivisor(self.vao, &self.models.buf, 2, 1);
  VAOAttribDivisor(self.vao, &self.models.buf, 3, 1);
  VAOAttribDivisor(self.vao, &self.models.buf, 4, 1);
  VAOAttribDivisor(self.vao, &self.models.buf, 5, 1);

  return self;
}

void DataBufferSet(DataBuffer *dbo, uint len, uint instanceSize, void *data) {
  if (dbo->len != len) {
    dbo->len = len;
    dbo->size = dbo->len * instanceSize;
  }

  BufferSubData(&dbo->buf, 0, dbo->size, data);
}

void DataBufferSliceSet(DataBuffer *dbo, uint offset, uint size, void *data) {
  BufferSubData(&dbo->buf, offset, size, data + offset);
}

void InstancedRendererDraw(InstancedRenderer *self) { DrawElementsInstanced(self->indicesLen, self->models.len); }

void InstancedRendererDestroy(InstancedRenderer *self) {
  VAODestroy(&self->vao);
  BufferDestroy(&self->vbo);
  BufferDestroy(&self->ibo);
  BufferDestroy(&self->models.buf);
  BufferDestroy(&self->colors.buf);
}
