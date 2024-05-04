#include "ttf/types.h"

Result *Bitstream_fromFile(Bitstream *self, const char *path) {
  self->i = 0;
  return readFile(path, "rb", &self->buf, &self->size);
}

Result *Bitstream_slice(Bitstream *self, const Bitstream *src, u64 offset, u64 size) {
  if (offset + size > src->size) {
    return ERRF("Bitstream slice is out of bounds\n\toffset: %lu\n\tsize: %lu\n\t", offset, src->size);
  }
  self->i = 0;
  self->size = size;
  self->buf = src->buf + offset;
  return OK;
}

void Bitstream_skip(Bitstream *self, u64 bytes) { self->i += bytes; }

Result *Bitstream_readU8(Bitstream *self, u8 *buf) {
  if (self->i++ == self->size) {
    return ERRF("Encountered Eof while reading bitstream\n\toffset: %lu\n\tsize: %lu", self->i, self->size);
  }
  *buf = self->buf[self->i];
  return OK;
}

#define READ_BYTES(_s)                                                                                                 \
  u64 _start = self->i;                                                                                                \
  self->i += _s;                                                                                                       \
  if (self->i > self->size) {                                                                                          \
    return ERRF("Encountered Eof while reading bitstream\n\toffset: %lu\n\tsize: %lu", self->i, self->size);           \
  }                                                                                                                    \
  memcpy(buf, &self->buf[_start], _s)

Result *Bitstream_readU16(Bitstream *self, u16 *buf) {
  READ_BYTES(sizeof(u16));
  BE_SWAP(*buf, 16);
  return OK;
}

Result *Bitstream_readU32(Bitstream *self, u32 *buf) {
  READ_BYTES(sizeof(u32));
  BE_SWAP(*buf, 32);
  return OK;
}

Result *Bitstream_readU64(Bitstream *self, u64 *buf) {
  READ_BYTES(sizeof(u64));
  BE_SWAP(*buf, 64);
  return OK;
}

Result *Bitstream_readI32(Bitstream *self, i32 *buf) {
  READ_BYTES(sizeof(i32));
  BE_SWAP(*buf, 32);
  return OK;
}

Result *Bitstream_readI64(Bitstream *self, i64 *buf) {
  READ_BYTES(sizeof(i64));
  BE_SWAP(*buf, 64);
  return OK;
}

Result *Bitstream_readI16(Bitstream *self, i16 *buf) {
  READ_BYTES(sizeof(i16));
  BE_SWAP(*buf, 16);
  return OK;
}

Result *Bitstream_readTag(Bitstream *self, Tag buf) {
  READ_BYTES(4);
  buf[4] = '\0';
  return OK;
}

Result *Bitstream_readStr(Bitstream *self, char *buf, u64 size) {
  READ_BYTES(size);
  buf[size] = '\0';
  return OK;
}

Result *Bitstream_readBuf(Bitstream *self, u8 *buf, u64 size) {
  READ_BYTES(size);
  return OK;
}

void Bitstream_free(Bitstream *self) { free(self->buf); }
