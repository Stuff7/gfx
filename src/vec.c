#include "math.h"

Vec4 Vec4New(f32 x, f32 y, f32 z, f32 w) { return (Vec4){{x, y, z, w}}; }

Vec3 Vec3Sub(Vec3 a, Vec3 b) { return (Vec3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z}; }

Vec3 Vec3Add(Vec3 a, Vec3 b) { return (Vec3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z}; }

Vec3 Vec3Cross(Vec3 a, Vec3 b) {
  return (Vec3){.x = a.y * b.z - a.z * b.y, .y = a.z * b.x - a.x * b.z, .z = a.x * b.y - a.y * b.x};
}

Vec3 Vec3ScalarMul(Vec3 v, float scalar) { return (Vec3){.x = v.x * scalar, .y = v.y * scalar, .z = v.z * scalar}; }

f32 Vec4Dot(Vec4 a, Vec4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

Vec4 Vec4Mat4Multiply(Vec4 v, Mat4 m) {
  return (Vec4){{
      Vec4Dot(m.rows[0], v),
      Vec4Dot(m.rows[1], v),
      Vec4Dot(m.rows[2], v),
      Vec4Dot(m.rows[3], v),
  }};
}

f32 Vec3Dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 Vec3Mat4Multiply(Vec3 v, Mat4 m) {
  return (Vec3){{
      Vec3Dot(m.rows[0].xyz, v),
      Vec3Dot(m.rows[1].xyz, v),
      Vec3Dot(m.rows[2].xyz, v),
  }};
}

Vec4 QuatNew(Vec3 euler_angles) {
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

void QuatRotateX(Vec4 *quat, f32 angle) {
  Vec4 rotation = Vec4Normalize((Vec4){{sinf(angle / 2), 0.0f, 0.0f, cosf(angle / 2)}});
  *quat = QuatMultiply(*quat, rotation);
}

void QuatRotateY(Vec4 *quat, f32 angle) {
  Vec4 rotation = Vec4Normalize((Vec4){{0.0f, sinf(angle / 2), 0.0f, cosf(angle / 2)}});
  *quat = QuatMultiply(*quat, rotation);
}

void QuatRotateZ(Vec4 *quat, f32 angle) {
  Vec4 rotation = Vec4Normalize((Vec4){{0.0f, 0.0f, sinf(angle / 2), cosf(angle / 2)}});
  *quat = QuatMultiply(*quat, rotation);
}

Mat4 QuatToMat4(Vec4 q) {
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
DefineVecXNormalize(Vec4, 4)
DefineVecXNormalize(Vec3, 3)
