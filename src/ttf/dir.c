#include "tables.h"

Result TableDirParse(TableDir *self, Bitstream *bs) {
  TRY(BitstreamSlice(&self->bs, bs, 0, bs->size));
  TRY(BitstreamReadU32(&self->bs, &self->sfntVersion));
  TRY(BitstreamReadU16(&self->bs, &self->numTables));
  TRY(BitstreamReadU16(&self->bs, &self->searchRange));
  TRY(BitstreamReadU16(&self->bs, &self->entrySelector));
  TRY(BitstreamReadU16(&self->bs, &self->rangeShift));

  for (int i = 0; i < self->numTables; i++) {
    TableTag tag;
    TRY(TableTagParse(&tag, &self->bs));
    if (tag == TableTag_Unknown) {
      BitstreamSkip(&self->bs, sizeof(TableRecord));
      continue;
    }
    TableRecord *record = &self->tableRecords[tag];
    TRY(TableRecordParse(record, &self->bs));
  }

  return OK;
}

Result TableDirFindTable(TableDir *self, TableTag tag, Bitstream *bs) {
  TableRecord *record = &self->tableRecords[tag];
  return BitstreamSlice(bs, &self->bs, record->offset, record->length);
}

Result TableTagParse(TableTag *tableTag, Bitstream *bs) {
  Tag tag;
  TRY(BitstreamReadTag(bs, tag));

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
    printf("\x1b[38;5;190mUnknown table record %s\x1b[0m\n", tag);
    *tableTag = TableTag_Unknown;
  }

  return OK;
}

Result TableRecordParse(TableRecord *record, Bitstream *bs) {
  TRY(BitstreamReadU32(bs, &record->checksum));
  TRY(BitstreamReadU32(bs, &record->offset));
  TRY(BitstreamReadU32(bs, &record->length));
  return OK;
}
