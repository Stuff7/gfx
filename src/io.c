#include "defs.h"
#include "result.h"
#include <stdio.h>
#include <stdlib.h>

Result *readFile(const char *path, const char *modes, u8 **bytes, u64 *size) {
  FILE *file = fopen(path, modes);
  if (!file) { return ERR_SYS("Trying to open file %s", path); }

  if (fseek(file, 0, SEEK_END)) {
    fclose(file);
    return ERR_SYS("Trying to seek file %s", path);
    ;
  }

  long filesize = ftell(file);
  if (filesize == -1) {
    fclose(file);
    return ERR_SYS("Trying to find file size for %s", path);
    ;
  }
  rewind(file);

  u8 *buf = malloc(filesize + 1);
  if (!buf) {
    fclose(file);
    return ERR_SYS("Trying to allocate memory for %s", path);
    ;
  }

  *size = fread(buf, 1, filesize, file);
  if (*size != (u64)filesize) {
    free(buf);
    fclose(file);
    return ERR_SYS("Trying to read file %s", path);
    ;
  }

  if (fclose(file)) {
    free(buf);
    return ERR_SYS("Trying to close file %s", path);
    ;
  }

  *bytes = buf;

  return OK;
}

Result *readString(const char *path, char **out) {
  u64 len;
  TRY(readFile(path, "r", (u8 **)out, &len));
  (*out)[len] = '\0';
  return OK;
}
