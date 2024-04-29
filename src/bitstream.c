#include "font.h"
#include "renderer.h"
#include <string.h>

Result BitstreamFromFile(Bitstream *self, const char *path) {
  self->i = 0;
  return readFile(path, "rb", &self->buf, &self->size);
}

Result BitstreamSlice(Bitstream *self, Bitstream *src, u64 offset, u64 size) {
  if (offset + size > src->size) { return Err(BitstreamEoF); }
  self->i = 0;
  self->size = size;
  self->buf = src->buf + offset;
  return Ok;
}

Result BitstreamReadU8(Bitstream *self, u8 *buf) {
  if (self->i++ == self->size) { return Err(BitstreamEoF); }
  *buf = self->buf[self->i];
  return Ok;
}

#define READ_BYTES(s)                                                                                                  \
  u64 start = self->i;                                                                                                 \
  self->i += s;                                                                                                        \
  if (self->i > self->size) { return Err(BitstreamEoF); }                                                              \
  memcpy(buf, &self->buf[start], s)

Result BitstreamReadU16(Bitstream *self, u16 *buf) {
  READ_BYTES(sizeof(u16));
  beswap(*buf, 16);
  return Ok;
}

Result BitstreamReadU32(Bitstream *self, u32 *buf) {
  READ_BYTES(sizeof(u32));
  beswap(*buf, 32);
  return Ok;
}

Result BitstreamReadU64(Bitstream *self, u64 *buf) {
  READ_BYTES(sizeof(u64));
  beswap(*buf, 64);
  return Ok;
}

Result BitstreamReadI32(Bitstream *self, i32 *buf) {
  READ_BYTES(sizeof(i32));
  beswap(*buf, 32);
  return Ok;
}

Result BitstreamReadI64(Bitstream *self, i64 *buf) {
  READ_BYTES(sizeof(i64));
  beswap(*buf, 64);
  return Ok;
}

Result BitstreamReadI16(Bitstream *self, i16 *buf) {
  READ_BYTES(sizeof(i16));
  beswap(*buf, 16);
  return Ok;
}

Result BitstreamReadTag(Bitstream *self, Tag buf) {
  READ_BYTES(4);
  buf[4] = '\0';
  return Ok;
}

Result BitstreamReadStr(Bitstream *self, char *buf, u64 size) {
  READ_BYTES(size);
  buf[size] = '\0';
  return Ok;
}

Result BitstreamReadBuf(Bitstream *self, u8 *buf, u64 size) {
  READ_BYTES(size);
  return Ok;
}

void BitstreamDestroy(Bitstream *self) { free(self->buf); }
