#include "utils.h"

#define MAX_UTF8_CHAR_SIZE 4

Result RESULT_OK = {.err = Error_Ok};

bool ResultUnwrap(Result *res) {
  bool isErr = res->err;
  if (isErr) {
    if (res->reason) {
      fprintf(stderr, "%s\n", res->reason);
      free(res->reason);
    }
    if (res->err == Error_System) { perror("\x1b[1m\x1b[38;5;225mSystem\x1b[0m"); }
    Result *parent = res->parent;
    free(res);
    if (parent) { ResultUnwrap(parent); }
  }

  return isErr;
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
