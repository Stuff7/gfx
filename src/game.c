#include "game.h"
#include "renderer.h"
#include "result.h"
#include "utils.h"

#define SURF_SIZE 100
static const uint CUBE_LEN = SURF_SIZE * SURF_SIZE;

static void processInput(State *state, Key key, KeyState keyState);
static void draw(State *state);
static void resize(State *state, int width, int height);
static void mouseInput(State *state, f64 width, f64 height);

State State_new() {
  State state;
  state.model = malloc(CUBE_LEN * sizeof(Transform));
  state.matrices = malloc((CUBE_LEN + 2) * sizeof(Mat4));
  state.colors = malloc(CUBE_LEN * sizeof(Color));
  Camera_new(&state.camera, &state.matrices[CUBE_LEN], (Vec3){.z = 30.0}, (Vec3){}, (Vec3){.y = 1.0});
  state.proj = &state.matrices[CUBE_LEN + 1];

  return state;
}

Result *State_createScene(State *state, const char *windowTitle) {
  int i = 0;
  for (int y = 0; y < SURF_SIZE; y += 1) {
    for (int x = 0; x < SURF_SIZE; x += 1) {
      Color_set(&state->colors[i], RAND(0, 255), RAND(0, 255), RAND(0, 255));
      Transform_new(
          &state->model[i],
          &state->matrices[i],
          (Vec3){
              {x, y, 0.f}
      },
          (Vec3){{1.f, 1.f, 1.f}},
          (Vec3){{0.f, 0.f, 0.f}}
      );
      i++;
    }
  }

  Result *ret = OK;
  OK_OR_GOTO(
      model_free,
      ret,
      Window_new(
          state,
          windowTitle,
          WIN_WIDTH,
          WIN_HEIGHT,
          (RenderCallback)draw,
          (InputCallback)processInput,
          (ResizeCallback)resize,
          (CursorCallback)mouseInput
      )
  );
  OK_OR_GOTO(Window_free, ret, Shader_new(&state->shader, "shaders/vert.glsl", "shaders/frag.glsl"));

  Window_setCursor(state->cursor.x, state->cursor.y);
  Window_captureCursor((state->cursor.capture = true));

  Cube cube = Cube_new(1.f, 1.f, 1.f);
  uint indices[CUBE_IDX_LEN];
  Cube_indices(indices);
  state->renderer = InstancedRenderer_new(&cube, sizeof(cube), indices, sizeof(indices), CUBE_IDX_LEN, CUBE_LEN, true);

  Shader_use(state->shader);
  Camera_updateMatrix(&state->camera);
  resize(state, WIN_WIDTH, WIN_HEIGHT);
  DataBuffer_set(&state->renderer.models, CUBE_LEN, sizeof(Mat4), state->matrices);
  DataBuffer_set(&state->renderer.colors, CUBE_LEN, sizeof(Color), state->colors);
  goto ret;

Window_free:
  Window_free();
model_free:
  free(state->model);
  free(state->matrices);
  free(state->colors);
ret:
  return OK;
}

void State_free(State *state) {
  free(state->model);
  free(state->matrices);
  free(state->colors);
  Shader_free(state->shader);
  InstancedRenderer_free(&state->renderer);
  Window_free();
}

static void draw(State *state) {
  Shader_use(state->shader);
  Shader_uniformFloat(state->shader, "fTime", window.now);
  Shader_uniformMat4(state->shader, "view", state->camera.matrix);
  InstancedRenderer_draw(&state->renderer);
}

static void resize(State *state, int width, int height) {
  Mat4_perspective(state->proj, PI * .25f, (f32)width / (f32)height, .1f, 1000.f);
  Shader_uniformMat4(state->shader, "proj", state->proj);
}

static void mouseInput(State *state, f64 x, f64 y) {
  float xoffset = x - state->cursor.x;
  float yoffset = state->cursor.y - y;
  state->cursor.x = x;
  state->cursor.y = y;

  if (!state->cursor.capture) { return; }

  const f32 sensitivity = 0.003f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  Camera_rotate(&state->camera, xoffset, yoffset, 0.0f);
  Camera_updateMatrix(&state->camera);
}

static void processInput(State *state, Key key, KeyState keyState) {
  if (keyState == KeyState_Release) { return; }
  Transform *cube = &state->model[state->idx];
  const f32 moveSpeed = 3.0f;

  switch (key) {
    case Key_Q:
      Window_close();
      return;
    case Key_Escape:
      Window_captureCursor((state->cursor.capture = !state->cursor.capture));
      return;
    case Key_Up:
      cube->position.y += moveSpeed;
      break;
    case Key_Down:
      cube->position.y -= moveSpeed;
      break;
    case Key_Left:
      cube->position.x -= moveSpeed;
      break;
    case Key_Right:
      cube->position.x += moveSpeed;
      break;
    case Key_X:
      Quat_rotateX(&cube->rotation, 0.08f);
      break;
    case Key_Y:
      Quat_rotateY(&cube->rotation, 0.08f);
      break;
    case Key_Z:
      Quat_rotateZ(&cube->rotation, 0.08f);
      break;
    case Key_Kp_Add:
      cube->scale = Vec3_scalarMul(cube->scale, 1.05);
      break;
    case Key_Kp_Subtract:
      cube->scale = Vec3_scalarMul(cube->scale, 0.95);
      break;
    case Key_I:
      cube->scale.y += 0.05;
      break;
    case Key_J:
      cube->scale.x -= 0.05;
      break;
    case Key_K:
      cube->scale.y -= 0.05;
      break;
    case Key_L:
      cube->scale.x += 0.05;
      break;
    case Key_U:
      cube->scale.z -= 0.05;
      break;
    case Key_O:
      cube->scale.z += 0.05;
      break;
    case Key_W:
      Camera_move(&state->camera, (Vec3){.z = 1.f}, moveSpeed);
      Camera_updateMatrix(&state->camera);
      return;
    case Key_A:
      Camera_move(&state->camera, (Vec3){.x = -1.f}, moveSpeed);
      Camera_updateMatrix(&state->camera);
      return;
    case Key_S:
      Camera_move(&state->camera, (Vec3){.z = -1.f}, moveSpeed);
      Camera_updateMatrix(&state->camera);
      return;
    case Key_D:
      Camera_move(&state->camera, (Vec3){.x = 1.f}, moveSpeed);
      Camera_updateMatrix(&state->camera);
      return;
    case Key_P:
      LOG("CURSOR: %.2f\t%.2f\n", state->cursor.x, state->cursor.y);
      return;
    case Key_Kp_0:
      state->idx = 0;
      return;
    case Key_Kp_1:
      state->idx = 1;
      return;
    case Key_Kp_2:
      state->idx = 2;
      return;
    case Key_Kp_3:
      state->idx = 3;
      return;
    case Key_Kp_4:
      state->idx = 4;
      return;
    case Key_Kp_5:
      state->idx = 5;
      return;
    case Key_Kp_6:
      state->idx = 6;
      return;
    case Key_Kp_7:
      state->idx = 7;
      return;
    case Key_Kp_8:
      state->idx = 8;
      return;
    case Key_Kp_9:
      state->idx = 9;
      return;
    default:
      return;
  }

  Mat4_transform(cube->matrix, cube);
  DataBuffer_sliceSet(&state->renderer.models, state->idx * sizeof(Mat4), sizeof(Mat4), state->matrices);
}
