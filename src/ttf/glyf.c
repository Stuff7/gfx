#include "tables.h"
#include "types.h"
#include <stdlib.h>
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

  Result *ret = OK;
  OK_OR_GOTO(LocaTable_free, ret, TableDir_findTable(dir, TableTag_Hmtx, &bs));
  OK_OR_GOTO(LocaTable_free, ret, HmtxTable_parse(&self->hmtx, &bs, &self->hhea, &self->maxp));

  OK_OR_GOTO(HmtxTable_free, ret, TableDir_findTable(dir, TableTag_Cmap, &bs));
  OK_OR_GOTO(HmtxTable_free, ret, CmapTable_parse(&self->cmap, &bs));

  OK_OR_GOTO(CmapTable_free, ret, TableDir_findTable(dir, TableTag_Glyf, &bs));
  ASSERT_ALLOC_OR_GOTO(CmapTable_free, ret, Glyph, self->maxp.numGlyphs, self->glyphs);

  Bitstream glyfBs;
  for (u32 i = 0; i < self->maxp.numGlyphs; i++) {
    u32 offset = self->loca.offsets[i];
    if (self->loca.offsets[i + 1] - offset) {
      OK_OR_GOTO(Glyph_free, ret, Bitstream_slice(&glyfBs, &bs, offset, bs.size - offset));
      OK_OR_GOTO(Glyph_free, ret, GlyfTable_parse(&self->glyphs[i].header, &glyfBs));
    }
    else { memset(&self->glyphs[i].header, 0, sizeof(GlyfTable)); }
    OK_OR_GOTO(Glyph_free, ret, Glyph_parse(self, i), "Parsing glyph %u / %u", i, self->maxp.numGlyphs);
  }
  goto ret;

Glyph_free:
  for (int i = 0; i < self->maxp.numGlyphs; i++) {
    Glyph_free(&self->glyphs[i]);
  }
  free(self->glyphs);
CmapTable_free:
  CmapTable_free(&self->cmap);
HmtxTable_free:
  HmtxTable_free(&self->hmtx);
LocaTable_free:
  LocaTable_free(&self->loca);
ret:
  return ret;
}

Result *GlyphParser_getGlyph(GlyphParser *self, u16 c, Glyph *glyph) {
  u16 idx;
  TRY(CmapSubtable_findGlyphIdFromCharCode(&self->cmap.subtable, self->maxp.numGlyphs, c, &idx));
  *glyph = self->glyphs[idx];

  return OK;
}

void GlyphParser_free(GlyphParser *self) {
  HmtxTable_free(&self->hmtx);
  LocaTable_free(&self->loca);
  CmapTable_free(&self->cmap);
  for (int i = 0; i < self->maxp.numGlyphs; i++) {
    Glyph_free(&self->glyphs[i]);
  }
  free(self->glyphs);
}

Result *Glyph_parseSimplePoints(Glyph *self, bool isX) {
  u8 u8Mask = isX ? U8_X : U8_Y;
  u8 instructionMask = isX ? INSTRUCTION_X : INSTRUCTION_Y;
  i16 coord = 0;

  for (u16 i = 0; i < self->numPoints; i++) {
    u8 flag = self->flags[i];

    if (MASK(flag, u8Mask)) {
      u8 offset;
      TRY(Bitstream_readU8(&self->header.glyphStream, &offset));
      coord += MASK(flag, instructionMask) ? offset : -offset;
    }
    else if (!MASK(flag, instructionMask)) {
      i16 delta;
      TRY(Bitstream_readI16(&self->header.glyphStream, &delta));
      coord += delta;
    }

    if (isX) { self->points[i].x = coord; }
    else { self->points[i].y = coord; }

    self->points[i].onCurve = MASK(flag, CURVE);
  }

  return OK;
}

Result *Glyph_parse(GlyphParser *self, u64 glyphId) {
  Glyph *glyph = &self->glyphs[glyphId];
  glyph->needFree = glyph->header.numberOfContours != 0 && glyph->header.glyphStream.size > 0;

  if (!glyph->needFree) { return OK; }

  if (glyph->header.numberOfContours >= 0) { TRY(Glyph_parseSimple(glyph, &self->maxp)); }
  else { TRY(Glyph_parseCompound(self, glyphId)); }

  return OK;
}

Result *Glyph_parseSimple(Glyph *self, MaxpTable *maxp) {
  ASSERT_ALLOC(u16, self->header.numberOfContours, self->endPtsOfContours);

  self->numPoints = 0;
  for (i16 i = 0; i < self->header.numberOfContours; i++) {
    TRY(Bitstream_readU16(&self->header.glyphStream, &self->endPtsOfContours[i]));
    ASSERT(self->endPtsOfContours[i] != 0xFFFFu, "Invalid endPtsOfContour [%d / %d]", i, self->header.numberOfContours);
    ASSERT(
        self->endPtsOfContours[i] >= self->numPoints,
        "Decreasing endPtsOfContour [%d / %d]",
        i,
        self->header.numberOfContours
    );

    self->numPoints = self->endPtsOfContours[i];
  }
  self->numPoints++;

  u16 instructionsLength;
  TRY(Bitstream_readU16(&self->header.glyphStream, &instructionsLength));

  if (maxp->major == 1) {
    if (self->numPoints > maxp->maxPoints) {
      WARN("numPoints exceeds maxp.maxPoints\n\tnumPoints: %u\n\tmaxPoints: %u", self->numPoints, maxp->maxPoints);
      maxp->maxPoints = self->numPoints;
    }
    if (maxp->maxSizeOfInstructions < instructionsLength) {
      WARN(
          "instructionsLength exceeds maxp.maxSizeOfInstructions\n\tinstructionsLength: %u\n\tmaxSizeOfInstructions: "
          "%u",
          instructionsLength,
          maxp->maxSizeOfInstructions
      );
      maxp->maxSizeOfInstructions = instructionsLength;
    }
  }

  TRY(Bitstream_skip(&self->header.glyphStream, instructionsLength));
  ASSERT_ALLOC(u8, self->numPoints, self->flags);

  u8 flag, repeat;
  for (u16 i = 0; i < self->numPoints; i++) {
    TRY(Bitstream_readU8(&self->header.glyphStream, &flag));

    ASSERT(
        (!MASK(flag, OVERLAP_SIMPLE) || i == 0) && !MASK(flag, RESERVED_BIT),
        "OVERLAP_SIMPLE (bit 6) and RESERVED (bit 7) must be 0 in flag 0b%08B [%u / %u]",
        flag,
        i,
        self->numPoints
    );

    self->flags[i] = flag & ~REPEAT;

    if (MASK(flag, REPEAT)) {
      TRY(Bitstream_readU8(&self->header.glyphStream, &repeat));
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
  TRY(Glyph_parseSimplePoints(self, true));
  TRY(Glyph_parseSimplePoints(self, false));
  self->header.glyphStream.i = 0;

  return OK;
}

static const u16 ARG_1_AND_2_ARE_WORDS = 1;
static const u16 WE_HAVE_A_SCALE = 1 << 3;
static const u16 MORE_COMPONENTS = 1 << 5;
static const u16 WE_HAVE_AN_X_AND_Y_SCALE = 1 << 6;
static const u16 WE_HAVE_A_TWO_BY_TWO = 1 << 7;
static const u16 WE_HAVE_INSTRUCTIONS = 1 << 8;

Result *Glyph_parseCompound(GlyphParser *parser, u64 selfId) {
  Glyph *self = &parser->glyphs[selfId];
  MaxpTable *maxp = &parser->maxp;

  u16 flags = 0;
  u16 glyphId = 0;

  self->components = malloc(maxp->maxComponentElements * sizeof(CompoundGlyph));
  self->numComponents = 0;

  do {
    TRY(Bitstream_readU16(&self->header.glyphStream, &flags));
    TRY(Bitstream_readU16(&self->header.glyphStream, &glyphId));
    ASSERT(glyphId < maxp->numGlyphs, "Invalid glyph id\n\tExpected: < %u\n\tReceived: %u", maxp->numGlyphs, glyphId);
    ASSERT(
        self->numComponents < maxp->maxComponentElements,
        "numComponents exceeds maxp.maxComponentElements %u",
        maxp->maxComponentElements
    );
    CompoundGlyph *glyph = &self->components[self->numComponents++];
    glyph->glyphId = glyphId;

    if (flags & ARG_1_AND_2_ARE_WORDS) {
      i16 argument1;
      i16 argument2;
      TRY(Bitstream_readI16(&self->header.glyphStream, &argument1));
      TRY(Bitstream_readI16(&self->header.glyphStream, &argument2));
      glyph->x = argument1;
      glyph->y = argument2;
    }
    else {
      u8 argument1;
      u8 argument2;
      TRY(Bitstream_readU8(&self->header.glyphStream, &argument1));
      TRY(Bitstream_readU8(&self->header.glyphStream, &argument2));
      glyph->x = argument1;
      glyph->y = argument2;
    }

    if (flags & WE_HAVE_A_SCALE) {
      i16 scale;
      TRY(Bitstream_readI16(&self->header.glyphStream, &scale));
    }
    else if (flags & WE_HAVE_AN_X_AND_Y_SCALE) {
      i16 xscale;
      i16 yscale;
      TRY(Bitstream_readI16(&self->header.glyphStream, &xscale));
      TRY(Bitstream_readI16(&self->header.glyphStream, &yscale));
    }
    else if (flags & WE_HAVE_A_TWO_BY_TWO) {
      i16 xscale;
      i16 scale01;
      i16 scale10;
      i16 yscale;
      TRY(Bitstream_readI16(&self->header.glyphStream, &xscale));
      TRY(Bitstream_readI16(&self->header.glyphStream, &scale10));
      TRY(Bitstream_readI16(&self->header.glyphStream, &scale01));
      TRY(Bitstream_readI16(&self->header.glyphStream, &yscale));
    }
  }
  while (flags & MORE_COMPONENTS);

  if (flags & WE_HAVE_INSTRUCTIONS) {
    u16 instructionsLength;
    TRY(Bitstream_readU16(&self->header.glyphStream, &instructionsLength));

    if (maxp->major == 1 && maxp->maxSizeOfInstructions < instructionsLength) {
      WARN(
          "Instructions length exceeds maxp.maxSizeOfInstructions "
          "%d: %d",
          maxp->maxSizeOfInstructions,
          instructionsLength
      );
      maxp->maxSizeOfInstructions = instructionsLength;
    }

    TRY(Bitstream_skip(&self->header.glyphStream, instructionsLength));
  }

  self->components = realloc(self->components, self->numComponents * sizeof(CompoundGlyph));

  return OK;
}

void Glyph_free(Glyph *self) {
  if (!self->needFree) { return; }

  if (self->header.numberOfContours < 0) { free(self->components); }
  else {
    free(self->endPtsOfContours);
    free(self->flags);
    free(self->points);
  }
}
