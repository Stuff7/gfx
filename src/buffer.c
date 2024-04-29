#include "renderer.h"

void VAONew(uint *vao) { glGenVertexArrays(1, vao); }

void VAOBind(uint vao) { glBindVertexArray(vao); }

void VAOAttrib(uint vao, Buffer *buf, int layout, int numVert, AttribType type, int normalized, int stride,
               uintptr offset) {
  VAOBind(vao);
  BufferBind(buf);
  glVertexAttribPointer(layout, numVert, type, normalized, stride, (void *)offset);
  glEnableVertexAttribArray(layout);
}

void VAOAttribDivisor(uint vao, Buffer *buf, int layout, int divisor) {
  VAOBind(vao);
  BufferBind(buf);
  glVertexAttribDivisor(layout, divisor);
}

void VAODestroy(uint *vao) { glDeleteVertexArrays(1, vao); }

Buffer BufferNew(BufferType type) {
  Buffer buf = {.type = type};
  glGenBuffers(1, &buf.name);
  return buf;
}

void BufferBind(Buffer *buf) { glBindBuffer(buf->type, buf->name); }

void BufferData(Buffer *buf, int size, const void *data, bool dynamic) {
  BufferBind(buf);
  glBufferData(buf->type, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void BufferSubData(Buffer *buf, int offset, int size, const void *data) {
  BufferBind(buf);
  glBufferSubData(buf->type, offset, size, data);
}

void BufferDestroy(Buffer *buf) { glDeleteBuffers(1, &buf->name); }

void DrawElements(int numIndices) { glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL); }

void DrawElementsInstanced(int numIndices, int numInstances) {
  glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL, numInstances);
}
