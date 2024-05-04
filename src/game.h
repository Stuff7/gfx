#pragma once

#include "defs.h"
#include "renderer.h"

#define RAND(min, max) (f32) rand() / (f32)RAND_MAX *(max - min) + min
#define WIN_WIDTH 1000.f
#define WIN_HEIGHT 600.f

typedef struct {
  InstancedRenderer renderer;
  uint shader;
  Camera camera;
  Mat4 *proj;
  Transform *model;
  Mat4 *matrices;
  Color *colors;
  uint idx;

  struct {
    f64 x, y;
    bool capture;
  } cursor;
} State;

State State_new();
void State_free(State *state);
Result *State_createScene(State *state, const char *windowTitle);
