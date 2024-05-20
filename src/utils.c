#include "utils.h"
#include <string.h>
#include <time.h>

#define MAX_UTF8_CHAR_SIZE 4

Result RESULT_OK = {.err = Error_Ok};

bool Result_unwrap(Result *res) {
  bool isErr = res->err;
  if (isErr) {
    if (res->reason) {
      fprintf(stderr, "%s\n", res->reason);
      free(res->reason);
    }
    if (res->err == Error_System) { perror(FGID(225, BOLD("System"))); }
    Result *parent = res->parent;
    free(res);
    if (parent) { Result_unwrap(parent); }
  }

  return isErr;
}

char *stringReplace(char *src, const char *pattern, const char *repl) {
  u64 i = 0;
  u64 srcLen = strlen(src);
  u64 srcCap = srcLen;
  u64 replLen = strlen(repl);

  while (src[i] != '\0') {
    if (src[i] != pattern[0]) {
      i++;
      continue;
    }

    int j = 1;
    bool isMatch = false;
    int diff = 0;

    while (src[i + j] != '\0') {
      if (src[i + j] == pattern[j]) {
        j++;
        continue;
      }
      else if (pattern[j] == '\0') {
        isMatch = true;
        diff = replLen - j;
        break;
      }
      else { break; }
    }

    i += j;

    if (!isMatch) { continue; }

    if (diff < 0) {
      u64 k = 0;
      for (k = i; src[k] != '\0'; k++) {
        src[k + diff] = src[k];
      }
      srcLen = k + diff + 1;
    }
    else if (diff > 0) {
      if (srcLen + replLen > srcCap) {
        srcCap += replLen * 16;
        srcLen += replLen;
        src = realloc(src, srcCap);
      }
      for (u64 k = srcLen - 1; k > i; k--) {
        src[k] = src[k - diff];
      }
    }

    src[srcLen - 1] = '\0';
    memcpy(src + i - j, repl, replLen);
  }

  if (srcLen < srcCap) { src = realloc(src, srcLen); }

  return src;
}

char *getFormattedTime(void) {
  time_t timer;
  struct tm *timeinfo;

  time(&timer);
  timeinfo = localtime(&timer);

  static char timestr[12];
  strftime(timestr, sizeof(timestr), "%I:%M:%S %p", timeinfo);

  return timestr;
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
