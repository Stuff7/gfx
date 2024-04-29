#include "defs.h"

const char *RESULT_STR[] = {"Ok",
                            "glfwInit failed",
                            "GLFWwindow creation failed",
                            "Glad loading failed",
                            "Shader compilation failed",
                            "A system error occurred",
                            "Shader program linking failed",
                            "Shader program validation failed",
                            "Bitstream reached end of file while reading bits",
                            "Assertion failed"};

bool isErr(Result res) {
  if (res.err) {
    fprintf(stderr, "[Unwrap] %s\n\t%s\n\tat %s:%d\n", RESULT_STR[res.err], res.ctx == NULL ? "" : (char *)res.ctx,
            res.fileName, res.line);
    if (res.err == Error_System) { perror("[SystemError]"); }
    if (res.free) { res.free((void *)res.ctx); }
  }
  return res.err;
}
