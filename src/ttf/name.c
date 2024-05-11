#include "tables.h"
#include "types.h"

Result *NameTable_parse(NameTable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->version));
  TRY(Bitstream_readU16(bs, &self->count));
  TRY(Bitstream_readU16(bs, &self->storageOffset));
  TRY(Bitstream_slice(&self->bs, bs, self->storageOffset, bs->size - self->storageOffset));

  ASSERT_ALLOC(NameRecord, self->count, self->nameRecord);
  Result *ret = OK;
  for (int i = 0; i < self->count; i++) {
    OK_OR_GOTO(NameTable_free, ret, NameRecord_parse(&self->nameRecord[i], bs));
  }

  if (self->version == 1) {
    OK_OR_GOTO(NameTable_free, ret, Bitstream_readU16(bs, &self->langTagCount));
    ASSERT_ALLOC_OR_GOTO(NameTable_free, ret, LangTagRecord, self->langTagCount, self->langTagRecord);
    for (int i = 0; i < self->langTagCount; i++) {
      OK_OR_GOTO(NameTable_free, ret, LangTagRecord_parse(&self->langTagRecord[i], bs));
    }
  }
  goto ret;

NameTable_free:
  NameTable_free(self);
ret:
  return ret;
}

Result *NameRecord_parse(NameRecord *self, Bitstream *bs) {
  TRY(EncodingID_parse(&self->platformID, &self->encodingID, bs));
  TRY(Bitstream_readU16(bs, &self->languageID));
  ENUM_PARSE(bs, u16, U16, NameID, self->nameID);
  TRY(Bitstream_readU16(bs, &self->length));
  TRY(Bitstream_readU16(bs, &self->stringOffset));

  return OK;
}

Result *LangTagRecord_parse(LangTagRecord *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->length));
  TRY(Bitstream_readU16(bs, &self->langTagOffset));

  return OK;
}

Result *NameRecord_getString(const NameRecord *record, const NameTable *table, char **buf) {
  Bitstream data;
  TRY(Bitstream_slice(&data, &table->bs, record->stringOffset, record->length));

  u8 *bytes = malloc(record->length);

  if (record->platformID == PlatformID_Windows && record->encodingID.windows == EncodingIDWindows_UnicodeBMP) {
    TRY(Bitstream_readBuf(&data, bytes, record->length));
    *buf = decodeUnicodeBMP(bytes, record->length);
  }
  else if (record->platformID == PlatformID_Macintosh && record->encodingID.mac == EncodingIDMacintosh_Roman) {
    TRY(Bitstream_readBuf(&data, bytes, record->length));
    *buf = decodeMacRoman(bytes, record->length);
  }
  else {
    *buf = malloc(record->length + 1);
    TRY(Bitstream_readStr(&data, *buf, record->length));
  }

  free(bytes);
  return OK;
}

void NameTable_free(NameTable *self) {
  free(self->nameRecord);
  if (self->version == 1) { free(self->langTagRecord); }
}
