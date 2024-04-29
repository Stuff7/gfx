#pragma once

#include "defs.h"
#include <math.h>

#define PI M_PI

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define LBIT_OFF(n, c) (n & (((u8)0xFF << c) >> c))
#define RBIT_OFF(n, c) (n & (((u8)0xFF >> c) << c))

typedef union {
  struct {
    f32 x, y, z;
  };
  f32 data[3];
} Vec3;

typedef union {
  struct {
    f32 x, y, z, w;
  };
  f32 data[4];
  Vec3 xyz;
} Vec4;

typedef union {
  struct {
    u8 r, g, b;
  };
  u8 rgb[3];
} Color;

typedef struct {
  Vec3 nw, ne, se, sw;
} Quad;

typedef struct {
  Vec3 ftl; // Front top left
  Vec3 ftr; // Front top right
  Vec3 fbl; // Front bottom left
  Vec3 fbr; // Front bottom right
  Vec3 btl; // Back top left
  Vec3 btr; // Back top right
  Vec3 bbl; // Back bottom left
  Vec3 bbr; // Back bottom right
} Cube;

typedef union {
  struct {
    f32 m1x1, m2x1, m3x1, m4x1;
    f32 m1x2, m2x2, m3x2, m4x2;
    f32 m1x3, m2x3, m3x3, m4x3;
    f32 m1x4, m2x4, m3x4, m4x4;
  };
  f32 data[4 * 4];
  f32 data2d[4][4];
  Vec4 rows[4];
} Mat4;

typedef struct {
  Vec3 position, scale;
  Vec4 rotation;
  Mat4 *matrix;
} Transform;

void TransformNew(Transform *self, Mat4 *matrix, Vec3 position, Vec3 scale, Vec3 angle);
#define QUAD_IDX_LEN 6
void ColorSet(Color *c, u8 r, u8 g, u8 b);
Quad QuadNew(f32 w, f32 h);
void QuadIndices(uint *indices);
#define CUBE_IDX_LEN 36
Cube CubeNew(f32 width, f32 height, f32 depth);
void CubeIndices(uint *indices);

Vec4 Vec4New(f32 x, f32 y, f32 z, f32 w);
Vec4 Vec4Normalize(Vec4 v);
Vec4 Vec4Mat4Multiply(Vec4 v, Mat4 m);
f32 Vec4Dot(Vec4 a, Vec4 b);

Vec3 Vec3Normalize(Vec3 v);
Vec3 Vec3Add(Vec3 a, Vec3 b);
Vec3 Vec3ScalarMul(Vec3 v, float scalar);
Vec3 Vec3Sub(Vec3 a, Vec3 b);
Vec3 Vec3Cross(Vec3 a, Vec3 b);
Vec3 Vec3Mat4Multiply(Vec3 v, Mat4 m);
f32 Vec3Dot(Vec3 a, Vec3 b);

Vec4 QuatNew(Vec3 euler_angles);
void QuatRotateX(Vec4 *quat, f32 angle);
void QuatRotateY(Vec4 *quat, f32 angle);
void QuatRotateZ(Vec4 *quat, f32 angle);
Mat4 QuatToMat4(Vec4 q);

#define DefineVecXNormalize(typ, size)                                                                                 \
  f32 typ##Length(typ v) {                                                                                             \
    f32 sum = 0;                                                                                                       \
    for (int i = 0; i < size; i++) {                                                                                   \
      sum += v.data[i] * v.data[i];                                                                                    \
    }                                                                                                                  \
    return sqrt(sum);                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  typ typ##Normalize(typ v) {                                                                                          \
    typ result;                                                                                                        \
    f32 length = typ##Length(v);                                                                                       \
                                                                                                                       \
    if (length != 0) {                                                                                                 \
      for (int i = 0; i < size; i++) {                                                                                 \
        result.data[i] = v.data[i] / length;                                                                           \
      }                                                                                                                \
    } else {                                                                                                           \
      for (int i = 0; i < size; i++) {                                                                                 \
        result.data[i] = 0;                                                                                            \
      }                                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    return result;                                                                                                     \
  }

void Mat4Identity(Mat4 *self);
void Mat4LookAt(Mat4 *self, Vec3 eye, Vec3 center, Vec3 up);
void Mat4Perspective(Mat4 *self, f32 fov, f32 aspect, f32 near, f32 far);
void Mat4Scale(Mat4 *self, Vec3 scale);
void Mat4Rotate(Mat4 *self, Vec4 quat);
void Mat4Translate(Mat4 *self, Vec3 translation);
void Mat4Transform(Mat4 *self, const Transform *t);
void Mat4Multiply(Mat4 *a, const Mat4 *b);
void Mat4Print(const Mat4 *self);

/* clang-format off */
#define Mat4Init(\
  self,\
  v1x1, v1x2, v1x3, v1x4,\
  v2x1, v2x2, v2x3, v2x4,\
  v3x1, v3x2, v3x3, v3x4,\
  v4x1, v4x2, v4x3, v4x4\
) \
  self->m1x1 = v1x1; self->m1x2 = v1x2; self->m1x3 = v1x3; self->m1x4 = v1x4;\
  self->m2x1 = v2x1; self->m2x2 = v2x2; self->m2x3 = v2x3; self->m2x4 = v2x4;\
  self->m3x1 = v3x1; self->m3x2 = v3x2; self->m3x3 = v3x3; self->m3x4 = v3x4;\
  self->m4x1 = v4x1; self->m4x2 = v4x2; self->m4x3 = v4x3; self->m4x4 = v4x4
/* clang-format on */
