#include "render_glyphs.h"
#include "../renderer.h"
#include "../result.h"
#include "../utils.h"

#define SURF_SIZE 100

static void processInput(RenderGlyphsState *state, Key key, KeyState keyState);
static void draw(RenderGlyphsState *state);
static void resize(RenderGlyphsState *state, int width, int height);
static void mouseInput(RenderGlyphsState *state, f64 width, f64 height);

bool isEar(Vec2 *vertices, int n, int i, uint *indices);
uint *earClippingTriangulation(Vec2 *vertices, uint numVertices, uint *numOutIndices);
Vec2 *polybridge(Vec2 *points, u16 *pNumPoints, u16 *holeIndices, u16 numHoles);

Result *RenderGlyphsState_createScene(RenderGlyphsState *state, const char *ttfPath, const char *windowTitle) {
  Camera_new(&state->camera, &state->view, (Vec3){.z = 3.0}, (Vec3){}, (Vec3){.y = 1.0});
  Transform_new(
      &state->transform,
      &state->model,
      (Vec3){
          {0.f, 0.f, 0.f}
  },
      (Vec3){{1.f, 1.f, 1.f}},
      (Vec3){{0.f, 0.f, 0.f}}
  );

  Result *ret = OK;
  TRY(Window_new(
      state,
      windowTitle,
      WIN_WIDTH,
      WIN_HEIGHT,
      (RenderCallback)draw,
      (InputCallback)processInput,
      (ResizeCallback)resize,
      (CursorCallback)mouseInput
  ));
  OK_OR_GOTO(Window_free, ret, Shader_new(&state->shader, "shaders/glyph_vert.glsl", "shaders/glyph_frag.glsl"));

  Window_setCursor(state->cursor.x, state->cursor.y);
  Window_captureCursor((state->cursor.capture = false));

  OK_OR_GOTO(Window_free, ret, Bitstream_fromFile(&state->bs, ttfPath));
  TableDir table;
  OK_OR_GOTO(Bitstream_free, ret, TableDir_parse(&table, &state->bs));
  OK_OR_GOTO(Bitstream_free, ret, GlyphParser_new(&state->glyphParser, &table));
  OK_OR_GOTO(GlyphParser_free, ret, GlyphParser_getGlyph(&state->glyphParser, 0x0126, &state->glyph));
  OK_OR_GOTO(GlyphParser_free, ret, Glyph_normalize(&state->normalGlyph, &state->glyph, &state->glyphParser.head));
  state->normalGlyph.points = polybridge(
      state->normalGlyph.points,
      &state->normalGlyph.numPoints,
      state->normalGlyph.endPtsOfContours,
      state->normalGlyph.numberOfContours
  );
  uint numIndices;
  uint *indices = earClippingTriangulation(state->normalGlyph.points, state->normalGlyph.numPoints, &numIndices);

  state->renderer = GlyphRenderer_new(
      state->normalGlyph.points,
      state->normalGlyph.numPoints * sizeof(Vec2),
      indices,
      sizeof(uint) * numIndices,
      numIndices
  );

  free(indices);

  Shader_use(state->shader);
  Shader_uniformMat4(state->shader, "model", &state->model);
  Camera_updateMatrix(&state->camera);
  resize(state, WIN_WIDTH, WIN_HEIGHT);
  goto ret;

GlyphParser_free:
  GlyphParser_free(&state->glyphParser);
Bitstream_free:
  Bitstream_free(&state->bs);
Window_free:
  Window_free();
ret:
  return ret;
}

void RenderGlyphsState_free(RenderGlyphsState *state) {
  Shader_free(state->shader);
  GlyphRenderer_free(&state->renderer);
  Window_free();
  NormalizedGlyph_free(&state->normalGlyph);
  GlyphParser_free(&state->glyphParser);
  Bitstream_free(&state->bs);
}

static void draw(RenderGlyphsState *state) {
  Shader_use(state->shader);
  Shader_uniformFloat(state->shader, "fTime", window.now);
  Shader_uniformMat4(state->shader, "view", state->camera.matrix);
  GlyphRenderer_draw(&state->renderer);
}

static void resize(RenderGlyphsState *state, int width, int height) {
  Mat4_perspective(&state->proj, PI * .25f, (f32)width / (f32)height, .1f, 1000.f);
  Shader_uniformMat4(state->shader, "proj", &state->proj);
}

static void mouseInput(RenderGlyphsState *state, f64 x, f64 y) {
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

static void processInput(RenderGlyphsState *state, Key key, KeyState keyState) {
  if (keyState == KeyState_Release) { return; }
  const f32 moveSpeed = 1.0f;

  switch (key) {
    case Key_Q:
      Window_close();
      return;
    case Key_Escape:
      Window_captureCursor((state->cursor.capture = !state->cursor.capture));
      return;
    case Key_Up:
      state->transform.position.y += moveSpeed;
      break;
    case Key_Down:
      state->transform.position.y -= moveSpeed;
      break;
    case Key_Left:
      state->transform.position.x -= moveSpeed;
      break;
    case Key_Right:
      state->transform.position.x += moveSpeed;
      break;
    case Key_X:
      Quat_rotateX(&state->transform.rotation, 0.08f);
      break;
    case Key_Y:
      Quat_rotateY(&state->transform.rotation, 0.08f);
      break;
    case Key_Z:
      Quat_rotateZ(&state->transform.rotation, 0.08f);
      break;
    case Key_Kp_Add:
      state->transform.scale = Vec3_scalarMul(state->transform.scale, 1.05);
      break;
    case Key_Kp_Subtract:
      state->transform.scale = Vec3_scalarMul(state->transform.scale, 0.95);
      break;
    case Key_I:
      state->transform.scale.y += 0.05;
      break;
    case Key_J:
      state->transform.scale.x -= 0.05;
      break;
    case Key_K:
      state->transform.scale.y -= 0.05;
      break;
    case Key_L:
      state->transform.scale.x += 0.05;
      break;
    case Key_U:
      state->transform.scale.z -= 0.05;
      break;
    case Key_O:
      state->transform.scale.z += 0.05;
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
      printf("glyphs[%u] = {\n", state->glyph.numPoints);
      uint endPtsIdx = 0;
      for (u16 i = 0; i < state->glyph.numPoints; i++) {
        printf(
            "[%03u]  { %6d, %6d }\t{ %10f, %10f }\n",
            i,
            state->glyph.points[i].x,
            state->glyph.points[i].y,
            state->normalGlyph.points[i].x,
            state->normalGlyph.points[i].y
        );
        if (endPtsIdx < state->normalGlyph.numberOfContours && i == state->glyph.endPtsOfContours[endPtsIdx]) {
          endPtsIdx++;
          printf("\n");
        }
      }
      printf("}\n");
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

  Mat4_transform(state->transform.matrix, &state->transform);
}
