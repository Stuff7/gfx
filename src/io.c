#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>

Result readFile(const char *path, const char *modes, u8 **bytes, u64 *size) {
  FILE *file = fopen(path, modes);
  if (!file) { return ERR(System, path); }

  if (fseek(file, 0, SEEK_END)) {
    fclose(file);
    return ERR(System, path);
  }

  long filesize = ftell(file);
  if (filesize == -1) {
    fclose(file);
    return ERR(System, path);
  }
  rewind(file);

  u8 *buf = malloc(filesize + 1);
  if (!buf) {
    fclose(file);
    return ERR(System, path);
  }

  *size = fread(buf, 1, filesize, file);
  if (*size != (u64)filesize) {
    free(buf);
    fclose(file);
    return ERR(System, path);
  }

  if (fclose(file)) {
    free(buf);
    return ERR(System, path);
  }

  *bytes = buf;

  return OK;
}

Result readString(const char *path, char **out) {
  u64 len;
  TRY(readFile(path, "r", (u8 **)out, &len));
  (*out)[len] = '\0';
  return OK;
}
