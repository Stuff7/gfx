#include "math.h"
#include "renderer.h"

void Camera_new(Camera *camera, Mat4 *matrix, Vec3 position, Vec3 target, Vec3 up) {
  camera->position = position;
  camera->front = Vec3_normalize(Vec3_sub(target, position));
  camera->up = up;
  camera->matrix = matrix;
  camera->right = Vec3_normalize(Vec3_cross(camera->front, camera->up));
  camera->rotation = (Vec3){.x = atan2f(camera->front.z, camera->front.x), .y = asinf(camera->front.y), .z = 0.0f};
}

void Camera_move(Camera *camera, Vec3 direction, float speed) {
  Vec3 offset = Vec3_add(Vec3_scalarMul(camera->front, direction.z), Vec3_scalarMul(camera->right, direction.x));
  offset = Vec3_add(offset, Vec3_scalarMul(camera->up, direction.y));
  camera->position = Vec3_add(camera->position, Vec3_scalarMul(Vec3_normalize(offset), speed));
}

void Camera_rotate(Camera *camera, float yaw, float pitch, float roll) {
  camera->rotation.x += yaw;
  camera->rotation.y += pitch;
  const f64 HALF_PI = (PI / 2.0) - 0.01;
  if (camera->rotation.y >= HALF_PI) { camera->rotation.y = HALF_PI; }
  else if (camera->rotation.y <= -HALF_PI) { camera->rotation.y = -HALF_PI; }
  camera->rotation.z += roll;

  camera->front.x = cosf(camera->rotation.x) * cosf(camera->rotation.y);
  camera->front.y = sinf(camera->rotation.y);
  camera->front.z = sinf(camera->rotation.x) * cosf(camera->rotation.y);
  camera->front = Vec3_normalize(camera->front);

  Vec3 worldUp = {
      {0.0f, 1.0f, 0.0f}
  };
  camera->right = Vec3_normalize(Vec3_cross(camera->front, worldUp));
  camera->up = Vec3_normalize(Vec3_cross(camera->right, camera->front));

  // clang-format off
  Mat4 rollRotation = {
    .m1x1 = cosf(camera->rotation.z), .m2x1 = -sinf(camera->rotation.z), .m3x1 = 0.0f, .m4x1 = 0.0f,
    .m1x2 = sinf(camera->rotation.z), .m2x2 = cosf(camera->rotation.z),  .m3x2 = 0.0f, .m4x2 = 0.0f,
    .m1x3 = 0.0f,                     .m2x3 = 0.0f,                      .m3x3 = 1.0f, .m4x3 = 0.0f,
    .m1x4 = 0.0f,                     .m2x4 = 0.0f,                      .m3x4 = 0.0f, .m4x4 = 1.0f
  };
  // clang-format on
  camera->up = Vec3_mat4Multiply(camera->up, rollRotation);
}

void Camera_updateMatrix(Camera *camera) {
  Vec3 center = Vec3_add(camera->position, camera->front);
  Mat4_lookAt(camera->matrix, camera->position, center, camera->up);
}
