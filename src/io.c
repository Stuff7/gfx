#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

Result readFile(const char *path, const char *modes, u8 **bytes, u64 *size) {
  FILE *file = fopen(path, modes);
  if (!file) { return Err(System, path); }

  if (fseek(file, 0, SEEK_END)) {
    fclose(file);
    return Err(System, path);
  }

  long filesize = ftell(file);
  if (filesize == -1) {
    fclose(file);
    return Err(System, path);
  }
  rewind(file);

  u8 *buf = malloc(filesize + 1);
  if (!buf) {
    fclose(file);
    return Err(System, path);
  }

  *size = fread(buf, 1, filesize, file);
  if (*size != (u64)filesize) {
    free(buf);
    fclose(file);
    return Err(System, path);
  }

  if (fclose(file)) {
    free(buf);
    return Err(System, path);
  }

  *bytes = buf;

  return Ok;
}

Result readString(const char *path, char **out) {
  u64 len;
  try(readFile(path, "r", (u8 **)out, &len));
  (*out)[len] = '\0';
  return Ok;
}
