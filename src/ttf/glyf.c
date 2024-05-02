#include "tables.h"
#include "types.h"

Result *GlyphParserNew(GlyphParser *self, TableDir *dir) {
  Bitstream bs;

  TRY(TableDirFindTable(dir, TableTag_Head, &bs));
  TRY(HeadTableParse(&self->head, &bs));

  TRY(TableDirFindTable(dir, TableTag_Maxp, &bs));
  TRY(MaxpTableParse(&self->maxp, &bs));

  TRY(TableDirFindTable(dir, TableTag_Hhea, &bs));
  TRY(HheaTableParse(&self->hhea, &bs));

  TRY(TableDirFindTable(dir, TableTag_Loca, &bs));
  TRY(LocaTableParse(&self->loca, &bs, &self->head, &self->maxp));

  TRY(TableDirFindTable(dir, TableTag_Hmtx, &bs));
  TRY(HmtxTableParse(&self->hmtx, &bs, &self->hhea, &self->maxp));

  TRY(TableDirFindTable(dir, TableTag_Glyf, &bs));
  Bitstream glyfBs;
  self->glyf = malloc(self->maxp.numGlyphs * sizeof(GlyfTable));
  for (u32 i = 0; i < self->maxp.numGlyphs; i++) {
    u32 offset = self->loca.offsets[i];
    if (self->loca.offsets[i + 1] - offset) {
      TRY(BitstreamSlice(&glyfBs, &bs, offset, bs.size - offset));
      TRY(GlyfTableParse(&self->glyf[i], &glyfBs));
    }
  }

  return OK;
}

void GlyphParserDestroy(GlyphParser *self) {
  HmtxTableFree(&self->hmtx);
  LocaTableFree(&self->loca);
  free(self->glyf);
}

Result *GlyfTableParse(GlyfTable *self, Bitstream *bs) {
  TRY(BitstreamReadI16(bs, &self->numberOfContours));
  TRY(BitstreamReadI16(bs, &self->xMin));
  TRY(BitstreamReadI16(bs, &self->yMin));
  TRY(BitstreamReadI16(bs, &self->xMax));
  TRY(BitstreamReadI16(bs, &self->yMax));

  return OK;
}

Result *HheaTableParse(HheaTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->majorVersion));
  TRY(BitstreamReadU16(bs, &self->minorVersion));
  TRY(BitstreamReadI16(bs, &self->ascender));
  TRY(BitstreamReadI16(bs, &self->descender));
  TRY(BitstreamReadI16(bs, &self->lineGap));
  TRY(BitstreamReadU16(bs, &self->advanceWidthMax));
  TRY(BitstreamReadI16(bs, &self->minLeftSideBearing));
  TRY(BitstreamReadI16(bs, &self->minRightSideBearing));
  TRY(BitstreamReadI16(bs, &self->xMaxExtent));
  TRY(BitstreamReadI16(bs, &self->caretSlopeRise));
  TRY(BitstreamReadI16(bs, &self->caretSlopeRun));
  TRY(BitstreamReadI16(bs, &self->caretOffset));
  static const u64 RESERVED = 4 * sizeof(i16);
  BitstreamSkip(bs, RESERVED);
  TRY(BitstreamReadI16(bs, &self->metricDataFormat));
  TRY(BitstreamReadU16(bs, &self->numberOfHMetrics));

  return OK;
}

Result *LocaTableParse(LocaTable *self, Bitstream *bs, const HeadTable *head, const MaxpTable *maxp) {
  self->size = maxp->numGlyphs + 1;

  if (head->indexToLocFormat == LocFormat_Short) {
    ASSERT_ALLOC(u32, self->size, self->offsets);
    for (u32 i = 0; i < self->size; i++) {
      TRY(BitstreamReadU16(bs, (u16 *)(&self->offsets[i])));
      self->offsets[i] *= 2;
    }
  }
  else {
    ASSERT_ALLOC(u32, self->size, self->offsets);
    for (u32 i = 0; i < self->size; i++) {
      TRY(BitstreamReadU32(bs, &self->offsets[i]));
    }
  }

  return OK;
}

void LocaTableFree(LocaTable *self) { free(self->offsets); }

Result *HmtxTableParse(HmtxTable *self, Bitstream *bs, const HheaTable *hhea, const MaxpTable *maxp) {
  self->hMetrics = malloc(hhea->numberOfHMetrics * sizeof(LongHorMetric));
  for (int i = 0; i < hhea->numberOfHMetrics; i++) {
    LongHorMetric *hMetric = &self->hMetrics[i];
    TRY(BitstreamReadU16(bs, &hMetric->advanceWidth));
    TRY(BitstreamReadI16(bs, &hMetric->lsb));
  }

  self->leftSideBearingsSize = maxp->numGlyphs - hhea->numberOfHMetrics;
  self->leftSideBearings = malloc(self->leftSideBearingsSize * sizeof(i16));
  for (u64 i = 0; i < self->leftSideBearingsSize; i++) {
    TRY(BitstreamReadI16(bs, &self->leftSideBearings[i]));
  }

  return OK;
}

void HmtxTableFree(HmtxTable *self) {
  free(self->hMetrics);
  free(self->leftSideBearings);
}
