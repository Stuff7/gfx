#pragma once

#include "defs.h"
#include "keyboard.h"
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

State StateNew();
void StateDestroy(State *state);
Result StateCreateScene(State *state, const char *windowTitle);

void processInput(State *state, Key key, KeyState keyState);
void draw(State *state);
void resize(State *state, int width, int height);
void mouseInput(State *state, f64 width, f64 height);
