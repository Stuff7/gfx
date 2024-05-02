#include "ttf/types.h"

Result *BitstreamFromFile(Bitstream *self, const char *path) {
  self->i = 0;
  return readFile(path, "rb", &self->buf, &self->size);
}

Result *BitstreamSlice(Bitstream *self, const Bitstream *src, u64 offset, u64 size) {
  if (offset + size > src->size) {
    return ERRF("Bitstream slice is out of bounds\n\toffset: %lu\n\tsize: %lu\n\t", offset, src->size);
  }
  self->i = 0;
  self->size = size;
  self->buf = src->buf + offset;
  return OK;
}

void BitstreamSkip(Bitstream *self, u64 bytes) { self->i += bytes; }

Result *BitstreamReadU8(Bitstream *self, u8 *buf) {
  if (self->i++ == self->size) {
    return ERRF("Encountered Eof while reading bitstream\n\toffset: %lu\n\tsize: %lu", self->i, self->size);
  }
  *buf = self->buf[self->i];
  return OK;
}

#define READ_BYTES(s)                                                                                                  \
  u64 start = self->i;                                                                                                 \
  self->i += s;                                                                                                        \
  if (self->i > self->size) {                                                                                          \
    return ERRF("Encountered Eof while reading bitstream\n\toffset: %lu\n\tsize: %lu", self->i, self->size);           \
  }                                                                                                                    \
  memcpy(buf, &self->buf[start], s)

Result *BitstreamReadU16(Bitstream *self, u16 *buf) {
  READ_BYTES(sizeof(u16));
  BE_SWAP(*buf, 16);
  return OK;
}

Result *BitstreamReadU32(Bitstream *self, u32 *buf) {
  READ_BYTES(sizeof(u32));
  BE_SWAP(*buf, 32);
  return OK;
}

Result *BitstreamReadU64(Bitstream *self, u64 *buf) {
  READ_BYTES(sizeof(u64));
  BE_SWAP(*buf, 64);
  return OK;
}

Result *BitstreamReadI32(Bitstream *self, i32 *buf) {
  READ_BYTES(sizeof(i32));
  BE_SWAP(*buf, 32);
  return OK;
}

Result *BitstreamReadI64(Bitstream *self, i64 *buf) {
  READ_BYTES(sizeof(i64));
  BE_SWAP(*buf, 64);
  return OK;
}

Result *BitstreamReadI16(Bitstream *self, i16 *buf) {
  READ_BYTES(sizeof(i16));
  BE_SWAP(*buf, 16);
  return OK;
}

Result *BitstreamReadTag(Bitstream *self, Tag buf) {
  READ_BYTES(4);
  buf[4] = '\0';
  return OK;
}

Result *BitstreamReadStr(Bitstream *self, char *buf, u64 size) {
  READ_BYTES(size);
  buf[size] = '\0';
  return OK;
}

Result *BitstreamReadBuf(Bitstream *self, u8 *buf, u64 size) {
  READ_BYTES(size);
  return OK;
}

void BitstreamDestroy(Bitstream *self) { free(self->buf); }
