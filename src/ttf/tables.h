#pragma once

#include "types.h"
#include "utils.h"

extern const u64 MAX_ALLOC_SIZE;

#define ASSERT_ALLOC(_typ, _num, _table)                                                                               \
  {                                                                                                                    \
    uint _size = _num * sizeof(_typ);                                                                                  \
    ASSERT(_size < MAX_ALLOC_SIZE, "%s is too large\n\tsize: %u", #_table, _size);                                     \
    _table = malloc(_size);                                                                                            \
  }

#define ASSERT_ALLOC_OR_GOTO(_label, _ret, _typ, _num, _table)                                                         \
  {                                                                                                                    \
    uint _size = _num * sizeof(_typ);                                                                                  \
    ASSERT_OR_GOTO(_label, _ret, _size < MAX_ALLOC_SIZE, "%s is too large\n\tsize: %u", #_table, _size);               \
    _table = malloc(_size);                                                                                            \
  }

#define FILL_BUF(_label, _ret, _typ, _fnPostfix, _bs, _size, _buf)                                                     \
  {                                                                                                                    \
    ASSERT_ALLOC(_typ, _size, _buf);                                                                                   \
    for (int i = 0; i < _size; i++) {                                                                                  \
      OK_OR_GOTO(_label, _ret, Bitstream_read##_fnPostfix(_bs, &_buf[i]));                                             \
    }                                                                                                                  \
  }

typedef struct {
  i16 numberOfContours;
  i16 xMin;
  i16 yMin;
  i16 xMax;
  i16 yMax;
  Bitstream glyphStream;
} GlyfTable;

Result *GlyfTable_parse(GlyfTable *self, Bitstream *bs);

typedef struct {
  u8 major, minor;
  u16 numGlyphs;
  // Version 1.0
  u16 maxPoints;
  u16 maxContours;
  u16 maxCompositePoints;
  u16 maxCompositeContours;
  u16 maxZones;
  u16 maxTwilightPoints;
  u16 maxStorage;
  u16 maxFunctionDefs;
  u16 maxInstructionDefs;
  u16 maxStackElements;
  u16 maxSizeOfInstructions;
  u16 maxComponentElements;
  u16 maxComponentDepth;
} MaxpTable;

Result *MaxpTable_parse(MaxpTable *self, Bitstream *bs);

typedef struct {
  u16 majorVersion;
  u16 minorVersion;
  i16 ascender;
  i16 descender;
  i16 lineGap;
  u16 advanceWidthMax;
  i16 minLeftSideBearing;
  i16 minRightSideBearing;
  i16 xMaxExtent;
  i16 caretSlopeRise;
  i16 caretSlopeRun;
  i16 caretOffset;
  // i16 (reserved) set to 0
  // i16 (reserved) set to 0
  // i16 (reserved) set to 0
  // i16 (reserved) set to 0
  i16 metricDataFormat;
  u16 numberOfHMetrics;
} HheaTable;

Result *HheaTable_parse(HheaTable *self, Bitstream *bs);

typedef struct {
  LongHorMetric *hMetrics;
  u64 leftSideBearingsSize;
  i16 *leftSideBearings;
} HmtxTable;

Result *HmtxTable_parse(HmtxTable *self, Bitstream *bs, const HheaTable *hhea, const MaxpTable *maxp);
void HmtxTable_free(HmtxTable *self);

typedef struct {
  u16 majorVersion;
  u16 minorVersion;
  i32 fontRevision;
  u32 checksumAdjustment;
  u32 magicNumber;
  u16 flags;
  u16 unitsPerEm;
  i64 created;
  i64 modified;
  i16 xMin;
  i16 yMin;
  i16 xMax;
  i16 yMax;
  u16 macStyle;
  u16 lowestRecPPEM;
  i16 fontDirectionHint;
  LocFormat indexToLocFormat;
  i16 glyphDataFormat;
} HeadTable;

Result *HeadTable_parse(HeadTable *self, Bitstream *bs);

typedef union {
  u16 *u16;
  u32 *u32;
} LocaOffsets;

typedef struct {
  u32 size;
  u32 *offsets;
} LocaTable;

Result *LocaTable_parse(LocaTable *self, Bitstream *bs, const HeadTable *head, const MaxpTable *maxp);
void LocaTable_free(LocaTable *self);

// Format 4
typedef struct {
  u16 format;
  u16 length;
  u16 language;
  u16 segCount;
  u16 searchRange;
  u16 entrySelector;
  u16 rangeShift;
  u16 *endCode;
  u16 reservedPad;
  u16 *startCode;
  i16 *idDelta;
  u16 *idRangeOffsets;
  Bitstream glyphIdStream;
} CmapSubtable;

Result *CmapSubtable_findGlyphIdFromCharCode(CmapSubtable *self, u16 numGlyphs, u16 c, u16 *glyphId);

typedef struct {
  u16 version;
  u16 numTables;
  EncodingRecord *encodingRecords;
  CmapSubtable subtable;
} CmapTable;

Result *CmapTable_parse(CmapTable *self, Bitstream *bs);
void CmapTable_free(CmapTable *self);

typedef struct {
  u32 sfntVersion;
  u16 numTables, searchRange, entrySelector, rangeShift;
  TableRecord tableRecords[TableTag_Unknown];
  Bitstream bs;
} TableDir;

Result *TableDir_parse(TableDir *table, Bitstream *bs);
Result *TableDir_findTable(TableDir *self, TableTag tag, Bitstream *bs);

typedef struct {
  i16 x;
  i16 y;
  bool onCurve;
} GlyphPoint;

typedef struct {
  u16 glyphId;
  i32 x, y;
} CompoundGlyph;

typedef struct {
  GlyfTable header;
  bool needFree;

  union {
    struct {
      u16 numPoints;
      GlyphPoint *points;
      u16 *endPtsOfContours;
      u8 *flags;
    };

    struct {
      CompoundGlyph *components;
      u16 numComponents;
    };
  };
} Glyph;

typedef struct {
  f32 x, y;
  // bool onCurve;
} NormalizedGlyphPoint;

typedef struct {
  u16 *endPtsOfContours;
  u16 numPoints;
  u16 numberOfContours;

  NormalizedGlyphPoint *points;
} NormalizedGlyph;

Result *Glyph_normalize(NormalizedGlyph *self, Glyph *glyph, const HeadTable *header);
void NormalizedGlyph_free(NormalizedGlyph *self);

typedef struct {
  HeadTable head;
  MaxpTable maxp;
  HheaTable hhea;
  LocaTable loca;
  HmtxTable hmtx;
  CmapTable cmap;
  Glyph *glyphs;
} GlyphParser;

Result *GlyphParser_new(GlyphParser *self, TableDir *dir);
Result *GlyphParser_getGlyph(GlyphParser *self, u16 c, Glyph *glyph);
Result *GlyphParser_mapGlyphs(GlyphParser *self, Glyph map[0xFFFF]);
void GlyphParser_free(GlyphParser *self);

Result *Glyph_parse(GlyphParser *self, u64 glyphId);
Result *Glyph_parseSimplePoints(Glyph *self, bool isX);
Result *Glyph_parseSimple(Glyph *self, MaxpTable *maxp);
Result *Glyph_parseCompound(GlyphParser *parser, u64 selfId);
void Glyph_free(Glyph *self);

typedef struct {
  u16 majorVersion;
  u16 minorVersion;
  u16 glyphClassDefOffset;
  u16 attachListOffset;
  u16 ligCaretListOffset;
  u16 markAttachClassDefOffset;
  // Version 1.2
  u16 markGlyphSetsDefOffset;
  // Version 1.3
  u16 itemVarStoreOffset;
} GDEFTable;

Result *GDEFTable_parse(GDEFTable *self, Bitstream *bs);

typedef struct {
  u16 majorVersion;
  u16 minorVersion;
  u16 scriptListOffset;
  u16 featureListOffset;
  u16 lookupListOffset;
  // Version 1.1
  u32 featureVariationsOffset;
} GPOSTable;

typedef GPOSTable GSUBTable;
Result *GPOSTable_parse(GPOSTable *self, Bitstream *bs);

typedef struct {
  u16 version;
  i16 xAvgCharWidth;
  u16 usWeightClass;
  u16 usWidthClass;
  u16 fsType;
  i16 ySubscriptXSize;
  i16 ySubscriptYSize;
  i16 ySubscriptXOffset;
  i16 ySubscriptYOffset;
  i16 ySuperscriptXSize;
  i16 ySuperscriptYSize;
  i16 ySuperscriptXOffset;
  i16 ySuperscriptYOffset;
  i16 yStrikeoutSize;
  i16 yStrikeoutPosition;
  i16 sFamilyClass;
  u8 panose[10];
  u32 ulUnicodeRange1;
  u32 ulUnicodeRange2;
  u32 ulUnicodeRange3;
  u32 ulUnicodeRange4;
  Tag achVendID;
  u16 fsSelection;
  u16 usFirstCharIndex;
  u16 usLastCharIndex;
  i16 sTypoAscender;
  i16 sTypoDescender;
  i16 sTypoLineGap;
  u16 usWinAscent;
  u16 usWinDescent;
  // Version 1
  u32 ulCodePageRange1;
  u32 ulCodePageRange2;
  // Version 2 - 4;
  i16 sxHeight;
  i16 sCapHeight;
  u16 usDefaultChar;
  u16 usBreakChar;
  u16 usMaxContext;
  // Version 5;
  u16 usLowerOpticalPointSize;
  u16 usUpperOpticalPointSize;
} OS2Table;

Result *OS2Table_parse(OS2Table *self, Bitstream *bs);

typedef struct {
  i16 *instructions;
  u32 size;
} CvtTable;

Result *CvtTable_parse(CvtTable *self, Bitstream *bs);
void CvtTable_free(CvtTable *self);

typedef struct {
  u8 *instructions;
  u32 size;
} FpgmTable;

Result *FpgmTable_parse(FpgmTable *self, Bitstream *bs);
void FpgmTable_free(FpgmTable *self);

typedef struct {
  u16 version;
  u16 numRanges;
  GaspRange *gaspRanges;
} GaspTable;

Result *GaspTable_parse(GaspTable *self, Bitstream *bs);
void GaspTable_free(GaspTable *self);

typedef struct {
  u16 version;
  u16 count;
  u16 storageOffset;
  NameRecord *nameRecord;
  u16 langTagCount;
  LangTagRecord *langTagRecord;
  Bitstream bs;
} NameTable;

Result *NameTable_parse(NameTable *self, Bitstream *bs);
Result *NameRecord_getString(const NameRecord *record, const NameTable *table, char **buf);

void NameTable_free(NameTable *self);
