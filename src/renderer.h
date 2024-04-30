#pragma once

#include "GLFW/glfw3.h"
#include "defs.h"
#include "keyboard.h"
#include "math.h"
#include <endian.h>
#include <glad/glad.h>
#include <string.h>

static inline bool streq(const char *a, const char *b) { return strcmp(a, b) == 0; }

Result readFile(const char *path, const char *modes, u8 **bytes, u64 *size);
Result readString(const char *path, char **out);

/* ---------------- Bitstream ---------------- */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define BE_SWAP(buf, size) buf = htobe##size(buf)
#elif
#define BE_SWAP(buf, size)
#endif

typedef struct {
  u8 *buf;
  u64 size;
  u64 i;
} Bitstream;

Result BitstreamFromFile(Bitstream *self, const char *path);
Result BitstreamSlice(Bitstream *self, Bitstream *src, u64 offset, u64 size);
void BitstreamSkip(Bitstream *self, u64 bytes);
void BitstreamDestroy(Bitstream *self);

Result BitstreamReadU64(Bitstream *self, u64 *ret);
Result BitstreamReadU32(Bitstream *self, u32 *ret);
Result BitstreamReadU16(Bitstream *self, u16 *ret);
Result BitstreamReadU8(Bitstream *self, u8 *ret);

Result BitstreamReadI64(Bitstream *self, i64 *buf);
Result BitstreamReadI32(Bitstream *self, i32 *buf);
Result BitstreamReadI16(Bitstream *self, i16 *buf);

Result BitstreamReadStr(Bitstream *self, char *buf, u64 size);
Result BitstreamReadBuf(Bitstream *self, u8 *buf, u64 size);

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

Result WindowNew(
    void *context,
    const char *title,
    int w,
    int h,
    RenderCallback update,
    InputCallback input,
    ResizeCallback resize,
    CursorCallback cursor
);
void WindowSetCursor(f64 x, f64 y);
void WindowRun();
void WindowCaptureCursor(bool capture);
void WindowClose();
void WindowDestroy();

/* ---------------- Shader ---------------- */
Result ShaderNew(uint *shader, const char *vert, const char *frag);
void ShaderUse(uint shader);
void ShaderDestroy(uint shader);

void ShaderUniformMat4(uint shader, const char *name, const Mat4 *matrix);
void ShaderUniformVec4(uint shader, const char *name, f32 x, f32 y, f32 z, f32 w);
void ShaderUniformFloat(uint shader, const char *name, f32 x);

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

void VAONew(uint *vao);
void VAOBind(uint vao);
void VAOAttrib(
    uint vao,
    Buffer *buf,
    int layout,
    int numVert,
    AttribType type,
    int normalized,
    int stride,
    uintptr offset
);
void VAOAttribDivisor(uint vao, Buffer *buf, int layout, int divisor);
void VAODestroy(uint *vao);

Buffer BufferNew(BufferType type);
void BufferBind(Buffer *buf);
void BufferData(Buffer *buf, int size, const void *data, bool dynamic);
void BufferSubData(Buffer *buf, int offset, int size, const void *data);
void BufferDestroy(Buffer *buf);

void DrawElements(int numIndex);
void DrawElementsInstanced(int numIndices, int numInstances);

typedef struct {
  Buffer buf;
  uint len, size;
} DataBuffer;

typedef struct {
  uint vao;
  Buffer vbo, ibo;
  DataBuffer colors, models;
  uint indicesLen;
} InstancedRenderer;

void DataBufferSet(DataBuffer *dbo, uint len, uint instanceSize, void *data);
void DataBufferSliceSet(DataBuffer *dbo, uint offset, uint size, void *data);

/* ---------------- Renderer ---------------- */
InstancedRenderer InstancedRendererNew(
    const void *vertices,
    uint verticesSize,
    const uint *indices,
    uint indicesSize,
    uint indicesLen,
    uint maxInstances,
    bool dynamic
);
void InstancedRendererDraw(InstancedRenderer *self);
void InstancedRendererDestroy(InstancedRenderer *self);

/* ---------------- Camera ---------------- */
typedef struct {
  Vec3 position;
  Vec3 front;
  Vec3 up;
  Vec3 right;
  Vec3 rotation;
  Mat4 *matrix;
} Camera;

void CameraNew(Camera *camera, Mat4 *matrix, Vec3 position, Vec3 target, Vec3 up);
void CameraMove(Camera *camera, Vec3 direction, float speed);
void CameraRotate(Camera *camera, float pitch, float yaw, float roll);
void CameraUpdateMatrix(Camera *camera);
