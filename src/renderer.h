#pragma once

#include "GLFW/glfw3.h"
#include "defs.h"
#include "keyboard.h"
#include "math.h"
#include "result.h"
#include "ttf/tables.h"
#include <endian.h>
#include <glad/glad.h>

/* ---------------- Window ---------------- */
typedef void (*RenderCallback)(void *);
typedef void (*InputCallback)(void *, Key, KeyState);
typedef void (*ResizeCallback)(void *, int, int);
typedef void (*CursorCallback)(void *, f64, f64);

typedef struct {
  GLFWwindow *hndl;
  bool renderFps;
  int width;
  int height;
  f64 delta;
  f64 now;
  void *context;
  RenderCallback update;
  ResizeCallback resize;
  InputCallback input;
  CursorCallback cursor;
} Window;

extern Window window;

Result *Window_new(
    void *context,
    const char *title,
    int w,
    int h,
    RenderCallback update,
    InputCallback input,
    ResizeCallback resize,
    CursorCallback cursor
);
void Window_setCursor(f64 x, f64 y);
void Window_run();
void Window_captureCursor(bool capture);
void Window_close();
void Window_free();

/* ---------------- Shader ---------------- */
Result *Shader_new(uint *shader, const char *vert, const char *frag);
void Shader_use(uint shader);
void Shader_free(uint shader);

void Shader_uniformMat4(uint shader, const char *name, const Mat4 *matrix);
void Shader_uniformVec4(uint shader, const char *name, f32 x, f32 y, f32 z, f32 w);
void Shader_uniformFloat(uint shader, const char *name, f32 x);

/* ---------------- Buffer Object ---------------- */
typedef enum {
  BufferType_Array = GL_ARRAY_BUFFER,
  BufferType_Element = GL_ELEMENT_ARRAY_BUFFER,
} BufferType;

typedef enum {
  AttribType_Float = GL_FLOAT,
  AttribType_Byte = GL_UNSIGNED_BYTE,
} AttribType;

typedef struct {
  uint name;
  BufferType type;
} Buffer;

void VAO_new(uint *vao);
void VAO_bind(uint vao);
void VAO_attrib(
    uint vao,
    Buffer *buf,
    int layout,
    int numVert,
    AttribType type,
    int normalized,
    int stride,
    uintptr offset
);
void VAO_attribDivisor(uint vao, Buffer *buf, int layout, int divisor);
void VAO_free(uint *vao);

Buffer Buffer_new(BufferType type);
void Buffer_bind(Buffer *buf);
void Buffer_data(Buffer *buf, int size, const void *data, bool dynamic);
void Buffer_subData(Buffer *buf, int offset, int size, const void *data);
void Buffer_free(Buffer *buf);

void VAO_drawLineElements(int numIndices);
void VAO_drawElements(int numIndex);
void VAO_drawElementsInstanced(int numIndices, int numInstances);

typedef struct {
  Buffer buf;
  uint len, size;
} DataBuffer;

void DataBuffer_set(DataBuffer *dbo, uint len, uint instanceSize, void *data);
void DataBuffer_sliceSet(DataBuffer *dbo, uint offset, uint size, void *data);

/* ---------------- Renderer ---------------- */
typedef struct {
  uint vao;
  Buffer vbo, ibo;
  DataBuffer colors, models;
  uint indicesLen;
} InstancedRenderer;

InstancedRenderer InstancedRenderer_new(
    const void *vertices,
    uint verticesSize,
    const uint *indices,
    uint indicesSize,
    uint indicesLen,
    uint maxInstances,
    bool dynamic
);
void InstancedRenderer_draw(InstancedRenderer *self);
void InstancedRenderer_free(InstancedRenderer *self);

typedef struct {
  NormalizedGlyph glyph;
  uint vao;
  Buffer vbo, ibo;
  uint indicesLen;
} GlyphRenderer;

GlyphRenderer
GlyphRenderer_new(const void *vertices, uint verticesSize, const uint *indices, uint indicesSize, uint indicesLen);
void GlyphRenderer_draw(GlyphRenderer *self);
void GlyphRenderer_free(GlyphRenderer *self);

/* ---------------- Camera ---------------- */
typedef struct {
  Vec3 position;
  Vec3 front;
  Vec3 up;
  Vec3 right;
  Vec3 rotation;
  Mat4 *matrix;
} Camera;

void Camera_new(Camera *camera, Mat4 *matrix, Vec3 position, Vec3 target, Vec3 up);
void Camera_move(Camera *camera, Vec3 direction, float speed);
void Camera_rotate(Camera *camera, float pitch, float yaw, float roll);
void Camera_updateMatrix(Camera *camera);
