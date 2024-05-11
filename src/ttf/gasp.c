#include "tables.h"

Result *GaspTable_parse(GaspTable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->version));
  TRY(Bitstream_readU16(bs, &self->numRanges));

  ASSERT_ALLOC(GaspRange, self->numRanges, self->gaspRanges);
  Result *ret = OK;
  for (int i = 0; i < self->numRanges; i++) {
    OK_OR_GOTO(GaspTable_free, ret, GaspRange_parse(&self->gaspRanges[i], bs));
  }
  goto ret;

GaspTable_free:
  GaspTable_free(self);
ret:
  return ret;
}

Result *GaspRange_parse(GaspRange *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->rangeMaxPPEM));
  TRY(Bitstream_readU16(bs, &self->rangeGaspBehavior));

  return OK;
}

void GaspTable_free(GaspTable *self) { free(self->gaspRanges); }
