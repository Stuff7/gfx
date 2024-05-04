#include "renderer.h"

void VAO_new(uint *vao) { glGenVertexArrays(1, vao); }

void VAO_bind(uint vao) { glBindVertexArray(vao); }

void VAO_attrib(
    uint vao,
    Buffer *buf,
    int layout,
    int numVert,
    AttribType type,
    int normalized,
    int stride,
    uintptr offset
) {
  VAO_bind(vao);
  Buffer_bind(buf);
  glVertexAttribPointer(layout, numVert, type, normalized, stride, (void *)offset);
  glEnableVertexAttribArray(layout);
}

void VAO_attribDivisor(uint vao, Buffer *buf, int layout, int divisor) {
  VAO_bind(vao);
  Buffer_bind(buf);
  glVertexAttribDivisor(layout, divisor);
}

void VAO_free(uint *vao) { glDeleteVertexArrays(1, vao); }

Buffer Buffer_new(BufferType type) {
  Buffer buf = {.type = type};
  glGenBuffers(1, &buf.name);
  return buf;
}

void Buffer_bind(Buffer *buf) { glBindBuffer(buf->type, buf->name); }

void Buffer_data(Buffer *buf, int size, const void *data, bool dynamic) {
  Buffer_bind(buf);
  glBufferData(buf->type, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void Buffer_subData(Buffer *buf, int offset, int size, const void *data) {
  Buffer_bind(buf);
  glBufferSubData(buf->type, offset, size, data);
}

void Buffer_free(Buffer *buf) { glDeleteBuffers(1, &buf->name); }

void VAO_drawElements(int numIndices) { glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL); }

void VAO_drawElementsInstanced(int numIndices, int numInstances) {
  glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, NULL, numInstances);
}
