#include "tables.h"
#include "types.h"
#include <string.h>

const u64 MAX_ALLOC_SIZE = 1024 * 1024 * 4;

static const u8 CURVE = 1;
static const u8 U8_X = 1 << 1;
static const u8 U8_Y = 1 << 2;
static const u8 REPEAT = 1 << 3;
static const u8 INSTRUCTION_X = 1 << 4;
static const u8 INSTRUCTION_Y = 1 << 5;
static const u8 OVERLAP_SIMPLE = 1 << 6;
static const u8 RESERVED_BIT = 1 << 7;

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

  TRY(TableDir_findTable(dir, TableTag_Cmap, &bs));
  TRY(CmapTable_parse(&self->cmap, &bs));

  TRY(TableDir_findTable(dir, TableTag_Glyf, &bs));
  Bitstream glyfBs;
  self->glyf = malloc(self->maxp.numGlyphs * sizeof(GlyfTable));

  for (u32 i = 0; i < self->maxp.numGlyphs; i++) {
    u32 offset = self->loca.offsets[i];
    if (self->loca.offsets[i + 1] - offset) {
      TRY(Bitstream_slice(&glyfBs, &bs, offset, bs.size - offset));
      TRY(GlyfTable_parse(&self->glyf[i], &glyfBs));
    }
    else { memset(&self->glyf[i], 0, sizeof(GlyfTable)); }
  }

  ASSERT_ALLOC(Glyph, 0xFFFF, self->glyphs);
  for (u16 c = 0x0000; c < 0xFFFF; c++) {
    u16 idx;
    TRY(CmapSubtable_findGlyphIdFromCharCode(&self->cmap.subtable, c, &idx));
    TRY(Glyph_parse(&self->glyphs[c], &self->glyf[idx], self), "Parsing glyph %u / %u", idx, self->maxp.numGlyphs);
  }

  return OK;
}

void GlyphParser_free(GlyphParser *self) {
  HmtxTable_free(&self->hmtx);
  LocaTable_free(&self->loca);
  CmapTable_free(&self->cmap);
  for (int i = 0; i < 0xFFFF; i++) {
    Glyph_free(&self->glyphs[i]);
  }
  free(self->glyphs);
  free(self->glyf);
}

Result *Glyph_parsePoints(Glyph *self, bool isX) {
  u8 u8Mask = isX ? U8_X : U8_Y;
  u8 instructionMask = isX ? INSTRUCTION_X : INSTRUCTION_Y;
  i16 coord = 0;

  for (u16 i = 0; i < self->numPoints; i++) {
    u8 flag = self->flags[i];

    if (MASK(flag, u8Mask)) {
      u8 offset;
      TRY(Bitstream_readU8(&self->header->glyphStream, &offset));
      coord += MASK(flag, instructionMask) ? offset : -offset;
    }
    else if (!MASK(flag, instructionMask)) {
      i16 delta;
      TRY(Bitstream_readI16(&self->header->glyphStream, &delta));
      coord += delta;
    }

    if (isX) { self->points[i].x = coord; }
    else { self->points[i].y = coord; }

    self->points[i].onCurve = MASK(flag, CURVE);
  }

  return OK;
}

Result *Glyph_parse(Glyph *self, GlyfTable *header, GlyphParser *parser) {
  self->header = header;
  self->needFree = self->header->numberOfContours > 0 && self->header->glyphStream.size > 0;
  if (!self->needFree) { return OK; }

  ASSERT_ALLOC(u16, self->header->numberOfContours, self->endPtsOfContours);

  self->numPoints = 0;
  for (i16 i = 0; i < self->header->numberOfContours; i++) {
    TRY(Bitstream_readU16(&self->header->glyphStream, &self->endPtsOfContours[i]));
    ASSERT(
        self->endPtsOfContours[i] != 0xFFFFu, "Invalid endPtsOfContour [%d / %d]", i, self->header->numberOfContours
    );
    ASSERT(
        self->endPtsOfContours[i] >= self->numPoints,
        "Decreasing endPtsOfContour [%d / %d]",
        i,
        self->header->numberOfContours
    );

    self->numPoints = self->endPtsOfContours[i];
  }
  self->numPoints++;

  TRY(Bitstream_readU16(&self->header->glyphStream, &self->instructionsLength));

  if (parser->maxp.major == 1) {
    if (self->numPoints > parser->maxp.maxPoints) {
      WARN(
          "numPoints exceeds maxp.maxPoints\n\tnumPoints: %u\n\tmaxPoints: %u", self->numPoints, parser->maxp.maxPoints
      );
      parser->maxp.maxPoints = self->numPoints;
    }
    if (parser->maxp.maxSizeOfInstructions < self->instructionsLength) {
      WARN(
          "instructionsLength exceeds maxp.maxSizeOfInstructions\n\tinstructionsLength: %u\n\tmaxSizeOfInstructions: "
          "%u",
          self->instructionsLength,
          parser->maxp.maxSizeOfInstructions
      );
      parser->maxp.maxSizeOfInstructions = self->instructionsLength;
    }
  }

  TRY(Bitstream_skip(&self->header->glyphStream, self->instructionsLength));
  ASSERT_ALLOC(u8, self->numPoints, self->flags);

  u8 flag, repeat;
  for (u16 i = 0; i < self->numPoints; i++) {
    TRY(Bitstream_readU8(&self->header->glyphStream, &flag));

    ASSERT(
        (!MASK(flag, OVERLAP_SIMPLE) || i == 0) && !MASK(flag, RESERVED_BIT),
        "OVERLAP_SIMPLE (bit 6) and RESERVED (bit 7) must be 0 in flag 0b%08B [%u / %u]",
        flag,
        i,
        self->numPoints
    );

    self->flags[i] = flag & ~REPEAT;

    if (MASK(flag, REPEAT)) {
      TRY(Bitstream_readU8(&self->header->glyphStream, &repeat));
      ASSERT(repeat, "Repeat is 0 in flag 0b%08B [%u / %u]", flag, i, self->numPoints);

      ASSERT(
          i + repeat < self->numPoints,
          "Repeat %u exceeds numPoints %u in flag 0b%08B [%u / %u]",
          repeat,
          self->numPoints,
          flag,
          i,
          self->numPoints
      );

      while (repeat--) {
        self->flags[++i] = flag & ~REPEAT;
      }
    }
  }

  ASSERT_ALLOC(GlyphPoint, self->numPoints, self->points);
  memset(self->points, 0, sizeof(GlyphPoint) * self->numPoints);
  TRY(Glyph_parsePoints(self, true));
  TRY(Glyph_parsePoints(self, false));
  header->glyphStream.i = 0;

  return OK;
}

void Glyph_free(Glyph *self) {
  if (self->needFree) {
    free(self->endPtsOfContours);
    free(self->flags);
    free(self->points);
  }
}

Result *GlyfTable_parse(GlyfTable *self, Bitstream *bs) {
  TRY(Bitstream_readI16(bs, &self->numberOfContours));
  TRY(Bitstream_readI16(bs, &self->xMin));
  TRY(Bitstream_readI16(bs, &self->yMin));
  TRY(Bitstream_readI16(bs, &self->xMax));
  TRY(Bitstream_readI16(bs, &self->yMax));
  TRY(Bitstream_slice(&self->glyphStream, bs, bs->i, bs->size - bs->i));

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
  TRY(Bitstream_skip(bs, RESERVED));
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
  if (self->leftSideBearingsSize > 0) {
    self->leftSideBearings = malloc(self->leftSideBearingsSize * sizeof(i16));
    for (u64 i = 0; i < self->leftSideBearingsSize; i++) {
      TRY(Bitstream_readI16(bs, &self->leftSideBearings[i]));
    }
  }

  return OK;
}

void HmtxTable_free(HmtxTable *self) {
  free(self->hMetrics);
  if (self->leftSideBearingsSize > 0) { free(self->leftSideBearings); }
}
