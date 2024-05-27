#pragma once

#include "defs.h"
#include <math.h>

#define PI M_PI

#define MAX(_x, _y) (((_x) > (_y)) ? (_x) : (_y))
#define MIN(_x, _y) (((_x) < (_y)) ? (_x) : (_y))
#define LBIT_OFF(_n, _c) (_n & (((u8)0xFF << _c) >> _c))
#define RBIT_OFF(_n, _c) (_n & (((u8)0xFF >> _c) << _c))

typedef union {
  struct {
    f32 x, y;
  };

  f32 data[2];
} Vec2;

typedef union {
  struct {
    f32 x, y, z;
  };

  f32 data[3];
  Vec2 xy;
} Vec3;

typedef union {
  struct {
    f32 x, y, z, w;
  };

  f32 data[4];
  Vec2 xy;
  Vec3 xyz;
} Vec4;

typedef struct {
  Vec2 a, b, c;
} Triangle;

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

void Transform_new(Transform *self, Mat4 *matrix, Vec3 position, Vec3 scale, Vec3 angle);
#define QUAD_IDX_LEN 6
void Color_set(Color *c, u8 r, u8 g, u8 b);
Quad Quad_new(f32 w, f32 h);
Quad Quad_box(f32 minX, f32 minY, f32 maxX, f32 maxY);
void Quad_indices(uint *indices);
#define CUBE_IDX_LEN 36
Cube Cube_new(f32 width, f32 height, f32 depth);
void Cube_indices(uint *indices);

void Vec_sub(uint numVertices, const f32 *a, const f32 *b, f32 *out);
bool Vec_eq(uint numVertices, const f32 *a, const f32 *b);

f32 Vec2_cross(Vec2 a, Vec2 b, Vec2 c);
f64 Vec2_distanceSquared(Vec2 a, Vec2 b);
uint Vec2_closest(Vec2 self, const Vec2 *points, uint numPoints);

bool Triangle_isPointInside(const Triangle *self, Vec2 p);
f32 Triangle_area(const Triangle *self);

Vec4 Vec4_new(f32 x, f32 y, f32 z, f32 w);
Vec4 Vec4_normalize(Vec4 v);
Vec4 Vec4_mat4Multiply(Vec4 v, Mat4 m);
f32 Vec4_dot(Vec4 a, Vec4 b);

Vec3 Vec3_normalize(Vec3 v);
Vec3 Vec3_add(Vec3 a, Vec3 b);
Vec3 Vec3_scalarMul(Vec3 v, float scalar);
Vec3 Vec3_sub(Vec3 a, Vec3 b);
Vec3 Vec3_cross(Vec3 a, Vec3 b);
Vec3 Vec3_mat4Multiply(Vec3 v, Mat4 m);
f32 Vec3_dot(Vec3 a, Vec3 b);

Vec4 Quat_new(Vec3 euler_angles);
void Quat_rotateX(Vec4 *quat, f32 angle);
void Quat_rotateY(Vec4 *quat, f32 angle);
void Quat_rotateZ(Vec4 *quat, f32 angle);
Mat4 Quat_toMat4(Vec4 q);

#define DEF_VEC_NORMALIZE(_typ, _size)                                                                                 \
  f32 _typ##_length(_typ _v) {                                                                                         \
    f32 _sum = 0;                                                                                                      \
    for (int _i = 0; _i < _size; _i++) {                                                                               \
      _sum += _v.data[_i] * _v.data[_i];                                                                               \
    }                                                                                                                  \
    return sqrt(_sum);                                                                                                 \
  }                                                                                                                    \
                                                                                                                       \
  _typ _typ##_normalize(_typ _v) {                                                                                     \
    _typ _result;                                                                                                      \
    f32 _length = _typ##_length(_v);                                                                                   \
                                                                                                                       \
    if (_length != 0) {                                                                                                \
      for (int _i = 0; _i < _size; _i++) {                                                                             \
        _result.data[_i] = _v.data[_i] / _length;                                                                      \
      }                                                                                                                \
    }                                                                                                                  \
    else {                                                                                                             \
      for (int _i = 0; _i < _size; _i++) {                                                                             \
        _result.data[_i] = 0;                                                                                          \
      }                                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    return _result;                                                                                                    \
  }

void Mat4_identity(Mat4 *self);
void Mat4_lookAt(Mat4 *self, Vec3 eye, Vec3 center, Vec3 up);
void Mat4_perspective(Mat4 *self, f32 fov, f32 aspect, f32 near, f32 far);
void Mat4_scale(Mat4 *self, Vec3 scale);
void Mat4_rotate(Mat4 *self, Vec4 quat);
void Mat4_translate(Mat4 *self, Vec3 translation);
void Mat4_transform(Mat4 *self, const Transform *t);
void Mat4_multiply(Mat4 *a, const Mat4 *b);
void Mat4_print(const Mat4 *self);

/* clang-format off */
#define MAT4_INIT(\
  _self,\
  _v1x1, _v1x2, _v1x3, _v1x4,\
  _v2x1, _v2x2, _v2x3, _v2x4,\
  _v3x1, _v3x2, _v3x3, _v3x4,\
  _v4x1, _v4x2, _v4x3, _v4x4\
) \
  _self->m1x1 = _v1x1; _self->m1x2 = _v1x2; _self->m1x3 = _v1x3; _self->m1x4 = _v1x4;\
  _self->m2x1 = _v2x1; _self->m2x2 = _v2x2; _self->m2x3 = _v2x3; _self->m2x4 = _v2x4;\
  _self->m3x1 = _v3x1; _self->m3x2 = _v3x2; _self->m3x3 = _v3x3; _self->m3x4 = _v3x4;\
  _self->m4x1 = _v4x1; _self->m4x2 = _v4x2; _self->m4x3 = _v4x3; _self->m4x4 = _v4x4
/* clang-format on */
