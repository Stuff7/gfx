#include "utils.h"

#define MAX_UTF8_CHAR_SIZE 4

const char *RESULT_STR[] = {
    "Ok",
    "glfwInit failed",
    "GLFWwindow creation failed",
    "Glad loading failed",
    "Shader compilation failed",
    "A system error occurred",
    "Shader program linking failed",
    "Shader program validation failed",
    "Bitstream reached end of file while reading bits",
    "Assertion failed",
    "Try failed"
};

bool ResultUnwrap(Result res) {
  if (res.err) {
    fprintf(
        stderr,
        "[ERROR] %s\n\t%s%sat %s:%d\n",
        RESULT_STR[res.err],
        res.ctx == NULL ? "" : (char *)res.ctx,
        res.ctx == NULL ? "" : "\n\t",
        res.fileName,
        res.line
    );
    if (res.err == Error_System) { perror("[SystemError]"); }
    if (res.free) { res.free((void *)res.ctx); }
    if (res.cause != NULL) { return ResultUnwrap(*res.cause); }
  }

  bool err = res.err;
  if (res.parent != NULL) { free(res.parent); }
  if (res.self != NULL) { free(res.self); }
  return err;
}

char *decodeUnicodeBMP(const u8 *bytes, u64 length) {
  if (bytes == NULL || length % 2 != 0) { return NULL; }

  u64 string_length = length / 2;
  char *decoded_string = (char *)malloc((string_length + 1) * sizeof(char));
  if (decoded_string == NULL) { return NULL; }

  for (u64 i = 0; i < string_length; ++i) {
    decoded_string[i] = (bytes[2 * i] << 8) | bytes[2 * i + 1];
  }
  decoded_string[string_length] = '\0';

  return decoded_string;
}

char *decodeMacRoman(const u8 *bytes, u64 length) {
  if (bytes == NULL) { return NULL; }

  char *decoded_string = (char *)malloc((length * MAX_UTF8_CHAR_SIZE) + 1);
  if (decoded_string == NULL) { return NULL; }

  size_t decoded_length = 0;
  for (size_t i = 0; i < length; ++i) {
    if (bytes[i] < 0x80) { decoded_string[decoded_length++] = bytes[i]; }
    else {
      if (bytes[i] >= 0x80 && bytes[i] <= 0xBF) {
        decoded_string[decoded_length++] = 0xC0 | (bytes[i] >> 6);
        decoded_string[decoded_length++] = 0x80 | (bytes[i] & 0x3F);
      }
      else {
        decoded_string[decoded_length++] = 0xE0 | (bytes[i] >> 12);
        decoded_string[decoded_length++] = 0x80 | ((bytes[i] >> 6) & 0x3F);
        decoded_string[decoded_length++] = 0x80 | (bytes[i] & 0x3F);
      }
    }
  }
  decoded_string[decoded_length] = '\0';

  return decoded_string;
}
