#include "renderer.h"
#include <GL/gl.h>
#include <stdlib.h>

static Result compile(unsigned int *shader, int type, const char *path) {
  char *src;
  try(readString(path, &src));

  *shader = glCreateShader(type);
  glShaderSource(*shader, 1, (const char **)&src, NULL);
  glCompileShader(*shader);
  free(src);

  int success;
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    char *infoLog = malloc(512);
    glGetShaderInfoLog(*shader, 512, NULL, infoLog);
    glDeleteShader(*shader);
    return Err(ShaderCompilation, infoLog, free);
  }

  return Ok;
}

Result ShaderNew(uint *shader, const char *vertPath, const char *fragPath) {
  uint vert, frag;
  try(compile(&frag, GL_VERTEX_SHADER, vertPath));
  try(compile(&vert, GL_FRAGMENT_SHADER, fragPath));

  *shader = glCreateProgram();

  glAttachShader(*shader, vert);
  glAttachShader(*shader, frag);

  glLinkProgram(*shader);
  int success;
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  if (success == 0) {
    char *infoLog = malloc(512);
    glGetProgramInfoLog(*shader, 512, NULL, infoLog);
    return Err(ShaderProgramLink, infoLog, free);
  }

  glValidateProgram(*shader);
  glGetProgramiv(*shader, GL_LINK_STATUS, &success);
  if (success == 0) {
    char *infoLog = malloc(512);
    glGetProgramInfoLog(*shader, 512, NULL, infoLog);
    return Err(ShaderProgramValidation, infoLog, free);
  }

  glDeleteShader(vert);
  glDeleteShader(frag);

  return Ok;
}

void ShaderUse(uint shader) { glUseProgram(shader); }

void ShaderUniformMat4(uint shader, const char *name, const Mat4 *matrix) {
  glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, (f32 *)matrix->data);
}

void ShaderUniformVec4(uint shader, const char *name, f32 x, f32 y, f32 z, f32 w) {
  glUniform4f(glGetUniformLocation(shader, name), x, y, z, w);
}

void ShaderUniformFloat(uint shader, const char *name, f32 x) { glUniform1f(glGetUniformLocation(shader, name), x); }

void ShaderDestroy(uint shader) { glDeleteProgram(shader); }
