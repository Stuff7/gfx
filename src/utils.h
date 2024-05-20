#pragma once

#include "defs.h"
#include "result.h"
#include <string.h>

#define RAND(_min, _max) (f32) rand() / (f32)RAND_MAX *(_max - _min) + _min
#define MASK(_flag, _mask) ((_flag) & (_mask))

static inline bool streq(const char *a, const char *b) { return strcmp(a, b) == 0; }

char *stringReplace(char *src, const char *pattern, const char *repl);

Result *readFile(const char *path, const char *modes, u8 **bytes, u64 *size);
Result *readString(const char *path, char **out);

char *decodeUnicodeBMP(const u8 *bytes, u64 length);
char *decodeMacRoman(const u8 *bytes, u64 length);

/* ---------------- Logging ---------------- */
char *getFormattedTime(void);

#define LOG3(_format, _clr, _lvl, ...)                                                                                 \
  printf(                                                                                                              \
      FGID(251, "[%s] ") FGID(_clr, BOLD("%-5s | ")) _format " (" FGID(195, "%s") ":" FGID(157, BOLD("%d")) ")\n",     \
      getFormattedTime(),                                                                                              \
      _lvl,                                                                                                            \
      ##__VA_ARGS__,                                                                                                   \
      __FILE__,                                                                                                        \
      __LINE__                                                                                                         \
  )

#define LOG(_format, ...) LOG3(_format, 189, "DEBUG", ##__VA_ARGS__)
#define WARN(_format, ...) LOG3(_format, 222, "WARN", ##__VA_ARGS__)

/* ---------------- Bitstream ---------------- */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define BE_SWAP(_buf, _size) _buf = htobe##_size(_buf)
#elif
#define BE_SWAP(_buf, _size)
#endif

typedef struct {
  u8 *buf;
  u64 size;
  u64 i;
} Bitstream;

Result *Bitstream_fromFile(Bitstream *self, const char *path);
Result *Bitstream_slice(Bitstream *self, const Bitstream *src, u64 offset, u64 size);
Result *Bitstream_skip(Bitstream *self, u64 bytes);
void Bitstream_free(Bitstream *self);

Result *Bitstream_readU64(Bitstream *self, u64 *ret);
Result *Bitstream_readU32(Bitstream *self, u32 *ret);
Result *Bitstream_readU16(Bitstream *self, u16 *ret);
Result *Bitstream_readU8(Bitstream *self, u8 *ret);

Result *Bitstream_readI64(Bitstream *self, i64 *buf);
Result *Bitstream_readI32(Bitstream *self, i32 *buf);
Result *Bitstream_readI16(Bitstream *self, i16 *buf);

Result *Bitstream_readStr(Bitstream *self, char *buf, u64 size);
Result *Bitstream_readBuf(Bitstream *self, u8 *buf, u64 size);
