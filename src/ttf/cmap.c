#include "tables.h"

Result *CmapTableParse(CmapTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->version));
  TRY(BitstreamReadU16(bs, &self->numTables));

  ASSERT_ALLOC(EncodingRecord, self->numTables, self->encodingRecords);
  for (int i = 0; i < self->numTables; i++) {
    EncodingRecordParse(&self->encodingRecords[i], bs);
  }

  return OK;
}

Result *EncodingRecordParse(EncodingRecord *self, Bitstream *bs) {
  ENUM_PARSE(bs, u16, U16, PlatformID, self->platformID);
  ENUM_PARSE(bs, u16, U16, EncodingIDWindows, self->encodingID);
  TRY(BitstreamReadU32(bs, &self->subtableOffset));

  return OK;
}

void CmapTableFree(CmapTable *self) { free(self->encodingRecords); }
