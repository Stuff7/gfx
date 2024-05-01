#pragma once

#include "defs.h"
#include <string.h>

static inline bool streq(const char *a, const char *b) { return strcmp(a, b) == 0; }

Result readFile(const char *path, const char *modes, u8 **bytes, u64 *size);
Result readString(const char *path, char **out);

char *decodeUnicodeBMP(const u8 *bytes, u64 length);
char *decodeMacRoman(const u8 *bytes, u64 length);

/* ---------------- Bitstream ---------------- */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define BE_SWAP(buf, size) buf = htobe##size(buf)
#elif
#define BE_SWAP(buf, size)
#endif

typedef struct {
  u8 *buf;
  u64 size;
  u64 i;
} Bitstream;

Result BitstreamFromFile(Bitstream *self, const char *path);
Result BitstreamSlice(Bitstream *self, const Bitstream *src, u64 offset, u64 size);
void BitstreamSkip(Bitstream *self, u64 bytes);
void BitstreamDestroy(Bitstream *self);

Result BitstreamReadU64(Bitstream *self, u64 *ret);
Result BitstreamReadU32(Bitstream *self, u32 *ret);
Result BitstreamReadU16(Bitstream *self, u16 *ret);
Result BitstreamReadU8(Bitstream *self, u8 *ret);

Result BitstreamReadI64(Bitstream *self, i64 *buf);
Result BitstreamReadI32(Bitstream *self, i32 *buf);
Result BitstreamReadI16(Bitstream *self, i16 *buf);

Result BitstreamReadStr(Bitstream *self, char *buf, u64 size);
Result BitstreamReadBuf(Bitstream *self, u8 *buf, u64 size);
