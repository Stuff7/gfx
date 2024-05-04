#include "tables.h"
#include "types.h"

Result *GlyphParser_new(GlyphParser *self, TableDir *dir) {
  Bitstream bs;

  TRY(TableDir_findTable(dir, TableTag_Head, &bs));
  TRY(HeadTable_parse(&self->head, &bs));

  TRY(TableDir_findTable(dir, TableTag_Maxp, &bs));
  TRY(MaxpTable_parse(&self->maxp, &bs));

  TRY(TableDir_findTable(dir, TableTag_Hhea, &bs));
  TRY(HheaTable_parse(&self->hhea, &bs));

  TRY(TableDir_findTable(dir, TableTag_Loca, &bs));
  TRY(LocaTable_parse(&self->loca, &bs, &self->head, &self->maxp));

  TRY(TableDir_findTable(dir, TableTag_Hmtx, &bs));
  TRY(HmtxTable_parse(&self->hmtx, &bs, &self->hhea, &self->maxp));

  TRY(TableDir_findTable(dir, TableTag_Glyf, &bs));
  Bitstream glyfBs;
  self->glyf = malloc(self->maxp.numGlyphs * sizeof(GlyfTable));
  for (u32 i = 0; i < self->maxp.numGlyphs; i++) {
    u32 offset = self->loca.offsets[i];
    if (self->loca.offsets[i + 1] - offset) {
      TRY(Bitstream_slice(&glyfBs, &bs, offset, bs.size - offset));
      TRY(GlyfTable_parse(&self->glyf[i], &glyfBs));
    }
  }

  return OK;
}

void GlyphParser_free(GlyphParser *self) {
  HmtxTable_free(&self->hmtx);
  LocaTable_free(&self->loca);
  free(self->glyf);
}

Result *GlyfTable_parse(GlyfTable *self, Bitstream *bs) {
  TRY(Bitstream_readI16(bs, &self->numberOfContours));
  TRY(Bitstream_readI16(bs, &self->xMin));
  TRY(Bitstream_readI16(bs, &self->yMin));
  TRY(Bitstream_readI16(bs, &self->xMax));
  TRY(Bitstream_readI16(bs, &self->yMax));

  return OK;
}

Result *HheaTable_parse(HheaTable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->majorVersion));
  TRY(Bitstream_readU16(bs, &self->minorVersion));
  TRY(Bitstream_readI16(bs, &self->ascender));
  TRY(Bitstream_readI16(bs, &self->descender));
  TRY(Bitstream_readI16(bs, &self->lineGap));
  TRY(Bitstream_readU16(bs, &self->advanceWidthMax));
  TRY(Bitstream_readI16(bs, &self->minLeftSideBearing));
  TRY(Bitstream_readI16(bs, &self->minRightSideBearing));
  TRY(Bitstream_readI16(bs, &self->xMaxExtent));
  TRY(Bitstream_readI16(bs, &self->caretSlopeRise));
  TRY(Bitstream_readI16(bs, &self->caretSlopeRun));
  TRY(Bitstream_readI16(bs, &self->caretOffset));
  static const u64 RESERVED = 4 * sizeof(i16);
  Bitstream_skip(bs, RESERVED);
  TRY(Bitstream_readI16(bs, &self->metricDataFormat));
  TRY(Bitstream_readU16(bs, &self->numberOfHMetrics));

  return OK;
}

Result *LocaTable_parse(LocaTable *self, Bitstream *bs, const HeadTable *head, const MaxpTable *maxp) {
  self->size = maxp->numGlyphs + 1;

  if (head->indexToLocFormat == LocFormat_Short) {
    ASSERT_ALLOC(u32, self->size, self->offsets);
    for (u32 i = 0; i < self->size; i++) {
      TRY(Bitstream_readU16(bs, (u16 *)(&self->offsets[i])));
      self->offsets[i] *= 2;
    }
  }
  else {
    ASSERT_ALLOC(u32, self->size, self->offsets);
    for (u32 i = 0; i < self->size; i++) {
      TRY(Bitstream_readU32(bs, &self->offsets[i]));
    }
  }

  return OK;
}

void LocaTable_free(LocaTable *self) { free(self->offsets); }

Result *HmtxTable_parse(HmtxTable *self, Bitstream *bs, const HheaTable *hhea, const MaxpTable *maxp) {
  self->hMetrics = malloc(hhea->numberOfHMetrics * sizeof(LongHorMetric));
  for (int i = 0; i < hhea->numberOfHMetrics; i++) {
    LongHorMetric *hMetric = &self->hMetrics[i];
    TRY(Bitstream_readU16(bs, &hMetric->advanceWidth));
    TRY(Bitstream_readI16(bs, &hMetric->lsb));
  }

  self->leftSideBearingsSize = maxp->numGlyphs - hhea->numberOfHMetrics;
  self->leftSideBearings = malloc(self->leftSideBearingsSize * sizeof(i16));
  for (u64 i = 0; i < self->leftSideBearingsSize; i++) {
    TRY(Bitstream_readI16(bs, &self->leftSideBearings[i]));
  }

  return OK;
}

void HmtxTable_free(HmtxTable *self) {
  free(self->hMetrics);
  free(self->leftSideBearings);
}
