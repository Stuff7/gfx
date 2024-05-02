#include "tables.h"

Result *GaspTableParse(GaspTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->version));
  TRY(BitstreamReadU16(bs, &self->numRanges));

  ASSERT_ALLOC(GaspRange, self->numRanges, self->gaspRanges);
  for (int i = 0; i < self->numRanges; i++) {
    GaspRangeParse(&self->gaspRanges[i], bs);
  }

  return OK;
}

Result *GaspRangeParse(GaspRange *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->rangeMaxPPEM));
  TRY(BitstreamReadU16(bs, &self->rangeGaspBehavior));

  return OK;
}

void GaspTableFree(GaspTable *self) { free(self->gaspRanges); }
