#include "tables.h"
#include "types.h"

Result *NameTableParse(NameTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->version));
  TRY(BitstreamReadU16(bs, &self->count));
  TRY(BitstreamReadU16(bs, &self->storageOffset));
  TRY(BitstreamSlice(&self->bs, bs, self->storageOffset, bs->size - self->storageOffset));

  ASSERT_ALLOC(NameRecord, self->count, self->nameRecord);
  for (int i = 0; i < self->count; i++) {
    TRY(NameRecordParse(&self->nameRecord[i], bs));
  }

  if (self->version == 1) {
    TRY(BitstreamReadU16(bs, &self->langTagCount));
    ASSERT_ALLOC(LangTagRecord, self->langTagCount, self->langTagRecord);
    for (int i = 0; i < self->langTagCount; i++) {
      TRY(LangTagRecordParse(&self->langTagRecord[i], bs));
    }
  }

  return OK;
}

Result *NameRecordParse(NameRecord *self, Bitstream *bs) {
  TRY(PlatformEncodingIDParse(&self->platformID, &self->encodingID, bs));
  TRY(BitstreamReadU16(bs, &self->languageID));
  ENUM_PARSE(bs, u16, U16, NameID, self->nameID);
  TRY(BitstreamReadU16(bs, &self->length));
  TRY(BitstreamReadU16(bs, &self->stringOffset));

  return OK;
}

Result *LangTagRecordParse(LangTagRecord *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->length));
  TRY(BitstreamReadU16(bs, &self->langTagOffset));

  return OK;
}

Result *NameRecordGetString(const NameRecord *record, const NameTable *table, char **buf) {
  Bitstream data;
  TRY(BitstreamSlice(&data, &table->bs, record->stringOffset, record->length));

  u8 *bytes = malloc(record->length);

  if (record->platformID == PlatformID_Windows && record->encodingID.windows == EncodingIDWindows_UnicodeBMP) {
    TRY(BitstreamReadBuf(&data, bytes, record->length));
    *buf = decodeUnicodeBMP(bytes, record->length);
  }
  else if (record->platformID == PlatformID_Macintosh && record->encodingID.mac == EncodingIDMacintosh_Roman) {
    TRY(BitstreamReadBuf(&data, bytes, record->length));
    *buf = decodeMacRoman(bytes, record->length);
  }
  else {
    *buf = malloc(record->length + 1);
    TRY(BitstreamReadStr(&data, *buf, record->length));
  }

  free(bytes);
  return OK;
}

void NameTableFree(NameTable *self) {
  free(self->nameRecord);
  if (self->version == 1) { free(self->langTagRecord); }
}
