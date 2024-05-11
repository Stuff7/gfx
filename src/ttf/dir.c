#include "tables.h"

Result *TableDir_parse(TableDir *self, Bitstream *bs) {
  TRY(Bitstream_slice(&self->bs, bs, 0, bs->size));
  TRY(Bitstream_readU32(&self->bs, &self->sfntVersion));
  ASSERT(
      self->sfntVersion == 0x00010000 || self->sfntVersion == 0x4F54544F,
      "Invalid sfnfVersion\n\tExpected: 0x00010000 | 0x4F54544F\n\tReceived: 0x%08X",
      self->sfntVersion
  );

  TRY(Bitstream_readU16(&self->bs, &self->numTables));
  TRY(Bitstream_readU16(&self->bs, &self->searchRange));
  TRY(Bitstream_readU16(&self->bs, &self->entrySelector));
  TRY(Bitstream_readU16(&self->bs, &self->rangeShift));

  for (int i = 0; i < self->numTables; i++) {
    TableTag tag;
    TRY(TableTag_parse(&tag, &self->bs));
    if (tag == TableTag_Unknown) {
      TRY(Bitstream_skip(&self->bs, sizeof(TableRecord)));
      continue;
    }
    TableRecord *record = &self->tableRecords[tag];
    TRY(TableRecord_parse(record, &self->bs));
  }

  return OK;
}

Result *TableDir_findTable(TableDir *self, TableTag tag, Bitstream *bs) {
  TableRecord *record = &self->tableRecords[tag];
  return Bitstream_slice(bs, &self->bs, record->offset, record->length);
}

Result *TableTag_parse(TableTag *tableTag, Bitstream *bs) {
  Tag tag;
  TRY(Bitstream_readTag(bs, tag));

  if (streq(tag, "GDEF")) { *tableTag = TableTag_GDEF; }
  else if (streq(tag, "GPOS")) { *tableTag = TableTag_GPOS; }
  else if (streq(tag, "GSUB")) { *tableTag = TableTag_GSUB; }
  else if (streq(tag, "OS/2")) { *tableTag = TableTag_OS2; }
  else if (streq(tag, "cmap")) { *tableTag = TableTag_Cmap; }
  else if (streq(tag, "cvt ")) { *tableTag = TableTag_Cvt; }
  else if (streq(tag, "fpgm")) { *tableTag = TableTag_Fpgm; }
  else if (streq(tag, "gasp")) { *tableTag = TableTag_Gasp; }
  else if (streq(tag, "head")) { *tableTag = TableTag_Head; }
  else if (streq(tag, "loca")) { *tableTag = TableTag_Loca; }
  else if (streq(tag, "maxp")) { *tableTag = TableTag_Maxp; }
  else if (streq(tag, "name")) { *tableTag = TableTag_Name; }
  else if (streq(tag, "hmtx")) { *tableTag = TableTag_Hmtx; }
  else if (streq(tag, "hhea")) { *tableTag = TableTag_Hhea; }
  else if (streq(tag, "glyf")) { *tableTag = TableTag_Glyf; }
  else {
    WARN("Unknown table record " BOLD("%s"), tag);
    *tableTag = TableTag_Unknown;
  }

  return OK;
}

Result *TableRecord_parse(TableRecord *record, Bitstream *bs) {
  TRY(Bitstream_readU32(bs, &record->checksum));
  TRY(Bitstream_readU32(bs, &record->offset));
  TRY(Bitstream_readU32(bs, &record->length));
  return OK;
}
