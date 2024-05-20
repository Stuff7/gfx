#include "math.h"

void Vec_sub(uint numVertices, const f32 *a, const f32 *b, f32 *out) {
  for (uint i = 0; i < numVertices; i++) {
    out[i] = a[i] - b[i];
  }
}

f32 Vec2_cross(Vec2 a, Vec2 b) { return (a.x * b.y) - (b.x * a.y); }

f64 Vec2_distanceSquared(Vec2 a, Vec2 b) {
  f64 dx = a.x - b.x;
  f64 dy = a.y - b.y;

  return dx * dx + dy * dy;
}

bool Triangle_isPointInside(const Triangle *self, Vec2 p) {
  Vec2 aV, bV, cV;
  Vec_sub(2, p.data, self->a.data, aV.data);
  Vec_sub(2, p.data, self->b.data, bV.data);
  Vec_sub(2, p.data, self->c.data, cV.data);

  f32 area1 = Vec2_cross(aV, bV);
  f32 area2 = Vec2_cross(bV, cV);
  f32 area3 = Vec2_cross(cV, aV);

  return (area1 > 0 && area2 > 0 && area3 > 0) || (area1 < 0 && area2 < 0 && area3 < 0);
}

f32 Triangle_area(const Triangle *self) {
  Vec2 aV, bV;
  Vec_sub(2, self->b.data, self->a.data, aV.data);
  Vec_sub(2, self->c.data, self->a.data, bV.data);

  return Vec2_cross(aV, bV);
}

Vec4 Vec4_new(f32 x, f32 y, f32 z, f32 w) {
  return (Vec4){
      {x, y, z, w}
  };
}

f32 Vec4_dot(Vec4 a, Vec4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

Vec4 Vec4_mat4Multiply(Vec4 v, Mat4 m) {
  return (Vec4){
      {
       Vec4_dot(m.rows[0], v),
       Vec4_dot(m.rows[1], v),
       Vec4_dot(m.rows[2], v),
       Vec4_dot(m.rows[3], v),
       }
  };
}

Vec3 Vec3_sub(Vec3 a, Vec3 b) { return (Vec3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z}; }

Vec3 Vec3_add(Vec3 a, Vec3 b) { return (Vec3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z}; }

Vec3 Vec3_cross(Vec3 a, Vec3 b) {
  return (Vec3){.x = a.y * b.z - a.z * b.y, .y = a.z * b.x - a.x * b.z, .z = a.x * b.y - a.y * b.x};
}

Vec3 Vec3_scalarMul(Vec3 v, float scalar) { return (Vec3){.x = v.x * scalar, .y = v.y * scalar, .z = v.z * scalar}; }

f32 Vec3_dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 Vec3_mat4Multiply(Vec3 v, Mat4 m) {
  return (Vec3){
      {
       Vec3_dot(m.rows[0].xyz, v),
       Vec3_dot(m.rows[1].xyz, v),
       Vec3_dot(m.rows[2].xyz, v),
       }
  };
}

Vec4 Quat_new(Vec3 euler_angles) {
  f32 cy = cos(euler_angles.z * 0.5);
  f32 sy = sin(euler_angles.z * 0.5);
  f32 cp = cos(euler_angles.y * 0.5);
  f32 sp = sin(euler_angles.y * 0.5);
  f32 cr = cos(euler_angles.x * 0.5);
  f32 sr = sin(euler_angles.x * 0.5);

  Vec4 q;
  q.w = cy * cp * cr + sy * sp * sr;
  q.x = cy * cp * sr - sy * sp * cr;
  q.y = sy * cp * sr + cy * sp * cr;
  q.z = sy * cp * cr - cy * sp * sr;
  return q;
}

Vec4 QuatMultiply(Vec4 q1, Vec4 q2) {
  Vec4 result;
  result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
  result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
  result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
  result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
  return result;
}

void Quat_rotateX(Vec4 *quat, f32 angle) {
  Vec4 rotation = Vec4_normalize((Vec4){
      {sinf(angle / 2), 0.0f, 0.0f, cosf(angle / 2)}
  });
  *quat = QuatMultiply(*quat, rotation);
}

void Quat_rotateY(Vec4 *quat, f32 angle) {
  Vec4 rotation = Vec4_normalize((Vec4){
      {0.0f, sinf(angle / 2), 0.0f, cosf(angle / 2)}
  });
  *quat = QuatMultiply(*quat, rotation);
}

void Quat_rotateZ(Vec4 *quat, f32 angle) {
  Vec4 rotation = Vec4_normalize((Vec4){
      {0.0f, 0.0f, sinf(angle / 2), cosf(angle / 2)}
  });
  *quat = QuatMultiply(*quat, rotation);
}

Mat4 Quat_toMat4(Vec4 q) {
  Mat4 rotation_matrix;
  float x = q.x, y = q.y, z = q.z, w = q.w;
  float xx = x * x, xy = x * y, xz = x * z, xw = x * w;
  float yy = y * y, yz = y * z, yw = y * w;
  float zz = z * z, zw = z * w;

  rotation_matrix.m1x1 = 1.0f - 2.0f * (yy + zz);
  rotation_matrix.m2x1 = 2.0f * (xy - zw);
  rotation_matrix.m3x1 = 2.0f * (xz + yw);
  rotation_matrix.m4x1 = 0.0f;

  rotation_matrix.m1x2 = 2.0f * (xy + zw);
  rotation_matrix.m2x2 = 1.0f - 2.0f * (xx + zz);
  rotation_matrix.m3x2 = 2.0f * (yz - xw);
  rotation_matrix.m4x2 = 0.0f;

  rotation_matrix.m1x3 = 2.0f * (xz - yw);
  rotation_matrix.m2x3 = 2.0f * (yz + xw);
  rotation_matrix.m3x3 = 1.0f - 2.0f * (xx + yy);
  rotation_matrix.m4x3 = 0.0f;

  rotation_matrix.m1x4 = 0.0f;
  rotation_matrix.m2x4 = 0.0f;
  rotation_matrix.m3x4 = 0.0f;
  rotation_matrix.m4x4 = 1.0f;

  return rotation_matrix;
}

// clang-format off
DEF_VEC_NORMALIZE(Vec4, 4)
DEF_VEC_NORMALIZE(Vec3, 3)
