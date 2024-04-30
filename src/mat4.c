#include "math.h"

void Mat4Identity(Mat4 *self) {
  // clang-format off
  MAT4_INIT(self,
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
  );
  // clang-format on
}

void Mat4LookAt(Mat4 *self, Vec3 eye, Vec3 center, Vec3 up) {
  Vec3 f = Vec3Normalize(Vec3Sub(center, eye));
  Vec3 s = Vec3Normalize(Vec3Cross(f, up));
  Vec3 u = Vec3Cross(s, f);

  // clang-format off
    MAT4_INIT(self,
       s.x,  s.y,  s.z, -Vec3Dot(s, eye),
       u.x,  u.y,  u.z, -Vec3Dot(u, eye),
      -f.x, -f.y, -f.z,  Vec3Dot(f, eye),
       0.0f, 0.0f, 0.0f, 1.0f
    );
  // clang-format on
}

void Mat4Perspective(Mat4 *self, f32 fov, f32 aspect, f32 near, f32 far) {
  f32 t = tan(fov * 0.5);
  f32 range = far - near;

  // clang-format off
  MAT4_INIT(self,
    1.0f / (aspect * t),  0.0f,     0.0f,                  0.0f,
    0.0f,                 1.0f / t, 0.0f,                  0.0f,
    0.0f,                 0.0f,     -(far + near) / range, -2.0f * far * near / range,
    0.0f,                 0.0f,     -1.0f,                  0.0f
  );
  // clang-format on
}

void Mat4Scale(Mat4 *self, Vec3 scale) {
  // clang-format off
  MAT4_INIT(self,
    scale.x, 0.f,     0.f,     0.f,
    0.f,     scale.y, 0.f,     0.f,
    0.f,     0.f,     scale.z, 0.f,
    0.f,     0.f,     0.f,     1.f 
  );
  // clang-format on
}

void Mat4Rotate(Mat4 *self, Vec4 quat) {
  Mat4 m = QuatToMat4(quat);
  Mat4Multiply(self, &m);
}

void Mat4Translate(Mat4 *self, Vec3 translation) {
  self->m1x4 = translation.x;
  self->m2x4 = translation.y;
  self->m3x4 = translation.z;
}

void Mat4Multiply(Mat4 *a, const Mat4 *b) {
  for (int i = 0; i < 4; ++i) {
    f32 a_row[4] = {a->rows[i].x, a->rows[i].y, a->rows[i].z, a->rows[i].w};
    for (int j = 0; j < 4; ++j) {
      a->data[i * 4 + j] =
          a_row[0] * b->data[j] + a_row[1] * b->data[j + 4] + a_row[2] * b->data[j + 8] + a_row[3] * b->data[j + 12];
    }
  }
}

void Mat4Transform(Mat4 *self, const Transform *t) {
  Mat4Scale(self, t->scale);
  Mat4Rotate(self, t->rotation);
  Mat4Translate(self, t->position);
}

void TransformNew(Transform *self, Mat4 *matrix, Vec3 position, Vec3 scale, Vec3 angle) {
  self->scale = scale;
  self->rotation = QuatNew(angle);
  self->position = position;
  self->matrix = matrix;
  Mat4Transform(self->matrix, self);
}

void Mat4Print(const Mat4 *self) {
  for (int i = 0; i < 4; i++) {
    printf("%.2f\t%.2f\t%.2f\t%.2f\n", self->data2d[i][0], self->data2d[i][1], self->data2d[i][2], self->data2d[i][3]);
  }
}
