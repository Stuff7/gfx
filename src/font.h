#pragma once

#include "renderer.h"

typedef char Tag[5];

Result BitstreamReadTag(Bitstream *self, Tag buf);

typedef struct {
  Tag tag;
  u32 checksum;
  u32 offset;
  u32 length;
} TableRecord;
Result TableRecordParse(TableRecord *record, Bitstream *bs);

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
Result GDEFTableParse(GDEFTable *self, Bitstream *bs);

typedef struct {
  u16 majorVersion;
  u16 minorVersion;
  u16 scriptListOffset;
  u16 featureListOffset;
  u16 lookupListOffset;
  // Version 1.1
  u32 featureVariationsOffset;
} GPOSTable;
typedef GPOSTable GSUBHeader;
Result GPOSTableParse(GPOSTable *self, Bitstream *bs);

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
} OS2;
Result OS2TableParse(OS2 *self, Bitstream *bs);

typedef struct {
  u16 platformID;
  u16 encodingID;
  u32 subtableOffset;
} EncodingRecord;
Result EncodingRecordParse(EncodingRecord *self, Bitstream *bs);

typedef struct {
  u16 version;
  u16 numTables;
  EncodingRecord *encodingRecords;
} CmapTable;
Result CmapTableParse(CmapTable *self, Bitstream *bs);
void CmapTableFree(CmapTable *self);

typedef struct {
  i16 *instructions;
  u32 size;
} CvtTable;
Result CvtTableParse(CvtTable *self, Bitstream *bs);
void CvtTableFree(CvtTable *self);

typedef struct {
  u8 *instructions;
  u32 size;
} FpgmTable;
Result FpgmTableParse(FpgmTable *self, Bitstream *bs);
void FpgmTableFree(FpgmTable *self);

typedef struct {
  u16 rangeMaxPPEM;
  u16 rangeGaspBehavior;
} GaspRange;
Result GaspRangeParse(GaspRange *self, Bitstream *bs);

typedef struct {
  u16 version;
  u16 numRanges;
  GaspRange *gaspRanges;
} GaspTable;
Result GaspTableParse(GaspTable *self, Bitstream *bs);
void GaspTableFree(GaspTable *self);

typedef struct {
  u32 version;
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

Result MaxpTableParse(MaxpTable *self, Bitstream *bs);

typedef enum {
  LocFormat_Short = 0,
  LocFormat_Long = 1,
} LocFormat;

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

Result HeadTableParse(HeadTable *self, Bitstream *bs);

typedef struct {
  u32 size;
  void *offsets;
} LocaTable;

Result LocaTableParse(LocaTable *self, Bitstream *bs, const HeadTable *head);
void LocaTableFree(LocaTable *self);

typedef struct {
  u16 platformID;
  u16 encodingID;
  u16 languageID;
  u16 nameID;
  u16 length;
  u16 stringOffset;
} NameRecord;

Result NameRecordParse(NameRecord *self, Bitstream *bs);

typedef struct {
  u16 length;
  u16 langTagOffset;
} LangTagRecord;

Result LangTagRecordParse(LangTagRecord *self, Bitstream *bs);

typedef struct {
  u16 version;
  u16 count;
  u16 storageOffset;
  NameRecord *nameRecord;
  u16 langTagCount;
  LangTagRecord *langTagRecord;
  char **strings;
} NameTable;

Result NameTableParse(NameTable *self, Bitstream *bs);

void NameTableFree(NameTable *self);

typedef struct {
  u32 sfntVersion;
  u16 numTables, searchRange, entrySelector, rangeShift;
  TableRecord *tableRecords;
} TableDir;

Result TableDirParse(TableDir *table, Bitstream *bs);
void TableDirDestroy(TableDir *table);
