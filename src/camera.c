#include "math.h"
#include "renderer.h"

void CameraNew(Camera *camera, Mat4 *matrix, Vec3 position, Vec3 target, Vec3 up) {
  camera->position = position;
  camera->front = Vec3Normalize(Vec3Sub(target, position));
  camera->up = up;
  camera->matrix = matrix;
  camera->right = Vec3Normalize(Vec3Cross(camera->front, camera->up));
  camera->rotation = (Vec3){.x = atan2f(camera->front.z, camera->front.x), .y = asinf(camera->front.y), .z = 0.0f};
}

void CameraMove(Camera *camera, Vec3 direction, float speed) {
  Vec3 offset = Vec3Add(Vec3ScalarMul(camera->front, direction.z), Vec3ScalarMul(camera->right, direction.x));
  offset = Vec3Add(offset, Vec3ScalarMul(camera->up, direction.y));
  camera->position = Vec3Add(camera->position, Vec3ScalarMul(Vec3Normalize(offset), speed));
}

void CameraRotate(Camera *camera, float yaw, float pitch, float roll) {
  camera->rotation.x += yaw;
  camera->rotation.y += pitch;
  const f64 HALF_PI = (PI / 2.0) - 0.01;
  if (camera->rotation.y >= HALF_PI) { camera->rotation.y = HALF_PI; }
  else if (camera->rotation.y <= -HALF_PI) { camera->rotation.y = -HALF_PI; }
  camera->rotation.z += roll;

  camera->front.x = cosf(camera->rotation.x) * cosf(camera->rotation.y);
  camera->front.y = sinf(camera->rotation.y);
  camera->front.z = sinf(camera->rotation.x) * cosf(camera->rotation.y);
  camera->front = Vec3Normalize(camera->front);

  Vec3 worldUp = {
      {0.0f, 1.0f, 0.0f}
  };
  camera->right = Vec3Normalize(Vec3Cross(camera->front, worldUp));
  camera->up = Vec3Normalize(Vec3Cross(camera->right, camera->front));

  // clang-format off
  Mat4 rollRotation = {
    .m1x1 = cosf(camera->rotation.z), .m2x1 = -sinf(camera->rotation.z), .m3x1 = 0.0f, .m4x1 = 0.0f,
    .m1x2 = sinf(camera->rotation.z), .m2x2 = cosf(camera->rotation.z),  .m3x2 = 0.0f, .m4x2 = 0.0f,
    .m1x3 = 0.0f,                     .m2x3 = 0.0f,                      .m3x3 = 1.0f, .m4x3 = 0.0f,
    .m1x4 = 0.0f,                     .m2x4 = 0.0f,                      .m3x4 = 0.0f, .m4x4 = 1.0f
  };
  // clang-format on
  camera->up = Vec3Mat4Multiply(camera->up, rollRotation);
}

void CameraUpdateMatrix(Camera *camera) {
  Vec3 center = Vec3Add(camera->position, camera->front);
  Mat4LookAt(camera->matrix, camera->position, center, camera->up);
}
