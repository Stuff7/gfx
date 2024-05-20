#include "renderer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define GLFW_LOG(_format, ...) LOG3(_format, 225, "GLFW", ##__VA_ARGS__)

static bool RENDER_WIREFRAME = false;

static void cursor_callback(UNUSED(GLFWwindow *) _1, f64 x, f64 y) { window.cursor(window.context, x, y); }

static void input_callback(UNUSED(GLFWwindow *) _1, int key, UNUSED(int) _2, int action, UNUSED(int) _3) {
  if (action == KeyState_Release && key == Key_F1) {
    if ((RENDER_WIREFRAME = !RENDER_WIREFRAME)) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
  }
  window.input(window.context, key, action);
}

static void error_callback(int error, const char *description) { GLFW_LOG("(%d) %s", error, description); }

static void framebuffer_size_callback(UNUSED(GLFWwindow *) _, int width, int height) {
  glViewport(0, 0, width, height);
  window.width = width;
  window.height = height;
  window.resize(window.context, width, height);
}

void Window_setCursor(f64 x, f64 y) { glfwSetCursorPos(window.hndl, x, y); }

void Window_close() { glfwSetWindowShouldClose(window.hndl, true); }

void Window_run() {
  glEnable(GL_DEPTH_TEST);
  glfwSwapInterval(1);
  f64 deltaFrame = 0;

  while (!glfwWindowShouldClose(window.hndl)) {
    window.now = glfwGetTime();
    window.delta = window.now - deltaFrame;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    window.update(window.context);
    glfwSwapBuffers(window.hndl);
    glfwPollEvents();

    if (window.renderFps) {
      printf("\rFPS: %f", 1.0 / (window.delta));
      fflush(stdout);
    }

    deltaFrame = window.now;
  }
}

Window window;

Result *Window_new(
    void *context,
    const char *title,
    int w,
    int h,
    RenderCallback update,
    InputCallback input,
    ResizeCallback resize,
    CursorCallback cursor
) {
  window.context = context;
  window.update = update;
  window.input = input;
  window.resize = resize;
  window.cursor = cursor;

  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) { return ERR("glfwInit failed"); }

  GLFW_LOG("GLFW Version: %s", glfwGetVersionString());

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window.hndl = glfwCreateWindow(w, h, title, NULL, NULL);
  if (!window.hndl) {
    Window_free();
    return ERR("GLFWwindow creation failed");
  }

  glfwMakeContextCurrent(window.hndl);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    Window_free();
    return ERR("Glad loading failed");
  }

  GLFW_LOG("OpenGL Version: %s", glGetString(GL_VERSION));

  glfwSetFramebufferSizeCallback(window.hndl, framebuffer_size_callback);
  glfwSetKeyCallback(window.hndl, input_callback);
  glfwSetCursorPosCallback(window.hndl, cursor_callback);

  return OK;
}

void Window_captureCursor(bool capture) {
  glfwSetInputMode(window.hndl, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window_free() {
  if (window.hndl) { glfwDestroyWindow(window.hndl); }
  glfwTerminate();
}
