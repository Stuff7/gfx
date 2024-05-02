#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

void cursor_callback(UNUSED(GLFWwindow *) _1, f64 x, f64 y) { window.cursor(window.context, x, y); }

void input_callback(UNUSED(GLFWwindow *) _1, int key, UNUSED(int) _2, int action, UNUSED(int) _3) {
  window.input(window.context, key, action);
}

void error_callback(int error, const char *description) { fprintf(stderr, "[GLFW] (%d) %s\n", error, description); }

void framebuffer_size_callback(UNUSED(GLFWwindow *) _, int width, int height) {
  glViewport(0, 0, width, height);
  window.width = width;
  window.height = height;
  window.resize(window.context, width, height);
}

void WindowSetCursor(f64 x, f64 y) { glfwSetCursorPos(window.hndl, x, y); }

void WindowClose() { glfwSetWindowShouldClose(window.hndl, true); }

void WindowRun() {
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

Result *WindowNew(
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

  printf("GLFW Version: %s\n", glfwGetVersionString());

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window.hndl = glfwCreateWindow(w, h, title, NULL, NULL);
  if (!window.hndl) {
    WindowDestroy();
    return ERR("GLFWwindow creation failed");
  }

  glfwMakeContextCurrent(window.hndl);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "gladLoadGLLoader failed");
    WindowDestroy();
    return ERR("Glad loading failed");
  }

  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

  glfwSetFramebufferSizeCallback(window.hndl, framebuffer_size_callback);
  glfwSetKeyCallback(window.hndl, input_callback);
  glfwSetCursorPosCallback(window.hndl, cursor_callback);

  return OK;
}

void WindowCaptureCursor(bool capture) {
  glfwSetInputMode(window.hndl, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void WindowDestroy() {
  if (window.hndl) { glfwDestroyWindow(window.hndl); }
  glfwTerminate();
}
