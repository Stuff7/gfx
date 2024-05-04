#include "tables.h"

Result *GaspTable_parse(GaspTable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->version));
  TRY(Bitstream_readU16(bs, &self->numRanges));

  ASSERT_ALLOC(GaspRange, self->numRanges, self->gaspRanges);
  for (int i = 0; i < self->numRanges; i++) {
    GaspRange_parse(&self->gaspRanges[i], bs);
  }

  return OK;
}

Result *GaspRange_parse(GaspRange *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->rangeMaxPPEM));
  TRY(Bitstream_readU16(bs, &self->rangeGaspBehavior));

  return OK;
}

void GaspTable_free(GaspTable *self) { free(self->gaspRanges); }
