#include "renderer.h"
#include "result.h"
#include "utils.h"
#include <GL/gl.h>
#include <stdlib.h>

static Result *compileFromString(unsigned int *shader, int type, const char *src) {
  *shader = glCreateShader(type);
  glShaderSource(*shader, 1, (const char **)&src, NULL);
  glCompileShader(*shader);

  int success;
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(*shader, 512, NULL, infoLog);
    glDeleteShader(*shader);
    return ERR("Shader compilation failed: %s", infoLog);
  }

  return OK;
}

static Result *compile(unsigned int *shader, int type, const char *path) {
  char *src;
  TRY(readString(path, &src));
  TRY(compileFromString(shader, type, src));
  free(src);

  return OK;
}

Result *createShader(uint *shader, uint vert, uint frag) {
  *shader = glCreateProgram();

  glAttachShader(*shader, vert);
  glAttachShader(*shader, frag);

  glLinkProgram(*shader);
  int success;
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  if (success == 0) {
    char infoLog[512];
    glGetProgramInfoLog(*shader, 512, NULL, infoLog);
    return ERR("Shader linking failed: %s", infoLog);
  }

  glValidateProgram(*shader);
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  if (success == 0) {
    char infoLog[512];
    glGetProgramInfoLog(*shader, 512, NULL, infoLog);
    return ERR("Shader validation failed: %s", infoLog);
  }

  glDeleteShader(vert);
  glDeleteShader(frag);

  return OK;
}

Result *Shader_fromStrings(uint *shader, const char *vertSrc, const char *fragSrc) {
  uint vert, frag;
  TRY(compileFromString(&frag, GL_VERTEX_SHADER, vertSrc));
  TRY(compileFromString(&vert, GL_FRAGMENT_SHADER, fragSrc));
  return createShader(shader, vert, frag);
}

Result *Shader_new(uint *shader, const char *vertPath, const char *fragPath) {
  uint vert, frag;
  TRY(compile(&frag, GL_VERTEX_SHADER, vertPath));
  TRY(compile(&vert, GL_FRAGMENT_SHADER, fragPath));
  return createShader(shader, vert, frag);
}

void Shader_use(uint shader) { glUseProgram(shader); }

void Shader_uniformMat4(uint shader, const char *name, const Mat4 *matrix) {
  glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, (f32 *)matrix->data);
}

void Shader_uniformVec4(uint shader, const char *name, f32 x, f32 y, f32 z, f32 w) {
  glUniform4f(glGetUniformLocation(shader, name), x, y, z, w);
}

void Shader_uniformFloat(uint shader, const char *name, f32 x) { glUniform1f(glGetUniformLocation(shader, name), x); }

void Shader_free(uint shader) { glDeleteProgram(shader); }
