#include "font.h"
#include "renderer.h"

#define MAX_ALLOC_SIZE 100000

#define ASSERT_ALLOC(typ, num, table)                                                                                  \
  {                                                                                                                    \
    uint size = num * sizeof(typ);                                                                                     \
    ASSERT(size < MAX_ALLOC_SIZE, #table " is too large");                                                             \
    table = malloc(size);                                                                                              \
  }

Result TableDirParse(TableDir *self, Bitstream *bs) {
  self->bs = bs;
  TRY(BitstreamReadU32(self->bs, &self->sfntVersion));
  TRY(BitstreamReadU16(self->bs, &self->numTables));
  TRY(BitstreamReadU16(self->bs, &self->searchRange));
  TRY(BitstreamReadU16(self->bs, &self->entrySelector));
  TRY(BitstreamReadU16(self->bs, &self->rangeShift));

  for (int i = 0; i < self->numTables; i++) {
    TableTag tag;
    TRY(TableTagParse(&tag, self->bs));
    if (tag == TableTag_Unknown) {
      BitstreamSkip(self->bs, sizeof(TableRecord));
      continue;
    }
    TableRecord *record = &self->tableRecords[tag];
    TRY(TableRecordParse(record, self->bs));
    if (tag == TableTag_Head) {
      Bitstream header;
      TRY(BitstreamSlice(&header, self->bs, record->offset, record->length));
      HeadTableParse(&self->head, &header);
    }
  }

  return OK;
}

Result TableDirGetTable(TableDir *self, TableTag tag, void *table) {
  Bitstream bs;
  TableRecord *record = &self->tableRecords[tag];
  TRY(BitstreamSlice(&bs, self->bs, record->offset, record->length));

  switch (tag) {
    case TableTag_GDEF:
      TRY(GDEFTableParse(table, &bs));
      break;
    case TableTag_GPOS:
      TRY(GPOSTableParse(table, &bs));
      break;
    case TableTag_GSUB:
      TRY(GPOSTableParse(table, &bs));
      break;
    case TableTag_OS2:
      TRY(OS2TableParse(table, &bs));
      break;
    case TableTag_Cmap:
      TRY(CmapTableParse(table, &bs));
      break;
    case TableTag_Cvt:
      TRY(CvtTableParse(table, &bs));
      break;
    case TableTag_Fpgm:
      TRY(FpgmTableParse(table, &bs));
      break;
    case TableTag_Gasp:
      TRY(GaspTableParse(table, &bs));
      break;
    case TableTag_Loca:
      TRY(LocaTableParse(table, &bs, &self->head));
      break;
    case TableTag_Maxp:
      TRY(MaxpTableParse(table, &bs));
      break;
    case TableTag_Name:
      TRY(NameTableParse(table, &bs));
      break;
    default:
      break;
  }

  return OK;
}

Result TableTagParse(TableTag *tableTag, Bitstream *bs) {
  Tag tag;
  TRY(BitstreamReadTag(bs, tag));

  if (streq(tag, "GDEF")) {
    *tableTag = TableTag_GDEF;
  } else if (streq(tag, "GPOS")) {
    *tableTag = TableTag_GPOS;
  } else if (streq(tag, "GSUB")) {
    *tableTag = TableTag_GSUB;
  } else if (streq(tag, "OS/2")) {
    *tableTag = TableTag_OS2;
  } else if (streq(tag, "cmap")) {
    *tableTag = TableTag_Cmap;
  } else if (streq(tag, "cvt ")) {
    *tableTag = TableTag_Cvt;
  } else if (streq(tag, "fpgm")) {
    *tableTag = TableTag_Fpgm;
  } else if (streq(tag, "gasp")) {
    *tableTag = TableTag_Gasp;
  } else if (streq(tag, "head")) {
    *tableTag = TableTag_Head;
  } else if (streq(tag, "loca")) {
    *tableTag = TableTag_Loca;
  } else if (streq(tag, "maxp")) {
    *tableTag = TableTag_Maxp;
  } else if (streq(tag, "name")) {
    *tableTag = TableTag_Name;
  } else {
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

Result GDEFTableParse(GDEFTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->majorVersion));
  TRY(BitstreamReadU16(bs, &self->minorVersion));
  TRY(BitstreamReadU16(bs, &self->glyphClassDefOffset));
  TRY(BitstreamReadU16(bs, &self->attachListOffset));
  TRY(BitstreamReadU16(bs, &self->ligCaretListOffset));
  TRY(BitstreamReadU16(bs, &self->markAttachClassDefOffset));

  switch (self->minorVersion) {
    case 2:
      TRY(BitstreamReadU16(bs, &self->markGlyphSetsDefOffset));
      break;
    case 3:
      TRY(BitstreamReadU16(bs, &self->itemVarStoreOffset));
      break;
  }
  return OK;
}

Result GPOSTableParse(GPOSTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->majorVersion));
  TRY(BitstreamReadU16(bs, &self->minorVersion));
  TRY(BitstreamReadU16(bs, &self->scriptListOffset));
  TRY(BitstreamReadU16(bs, &self->featureListOffset));
  TRY(BitstreamReadU16(bs, &self->lookupListOffset));

  if (self->minorVersion == 1) { TRY(BitstreamReadU32(bs, &self->featureVariationsOffset)); }
  return OK;
}

Result OS2TableParse(OS2Table *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->version));
  TRY(BitstreamReadI16(bs, &self->xAvgCharWidth));
  TRY(BitstreamReadU16(bs, &self->usWeightClass));
  TRY(BitstreamReadU16(bs, &self->usWidthClass));
  TRY(BitstreamReadU16(bs, &self->fsType));
  TRY(BitstreamReadI16(bs, &self->ySubscriptXSize));
  TRY(BitstreamReadI16(bs, &self->ySubscriptYSize));
  TRY(BitstreamReadI16(bs, &self->ySubscriptXOffset));
  TRY(BitstreamReadI16(bs, &self->ySubscriptYOffset));
  TRY(BitstreamReadI16(bs, &self->ySuperscriptXSize));
  TRY(BitstreamReadI16(bs, &self->ySuperscriptYSize));
  TRY(BitstreamReadI16(bs, &self->ySuperscriptXOffset));
  TRY(BitstreamReadI16(bs, &self->ySuperscriptYOffset));
  TRY(BitstreamReadI16(bs, &self->yStrikeoutSize));
  TRY(BitstreamReadI16(bs, &self->yStrikeoutPosition));
  TRY(BitstreamReadI16(bs, &self->sFamilyClass));
  TRY(BitstreamReadBuf(bs, self->panose, 10));
  TRY(BitstreamReadU32(bs, &self->ulUnicodeRange1));
  TRY(BitstreamReadU32(bs, &self->ulUnicodeRange2));
  TRY(BitstreamReadU32(bs, &self->ulUnicodeRange3));
  TRY(BitstreamReadU32(bs, &self->ulUnicodeRange4));
  TRY(BitstreamReadTag(bs, self->achVendID));
  TRY(BitstreamReadU16(bs, &self->fsSelection));
  TRY(BitstreamReadU16(bs, &self->usFirstCharIndex));
  TRY(BitstreamReadU16(bs, &self->usLastCharIndex));
  TRY(BitstreamReadI16(bs, &self->sTypoAscender));
  TRY(BitstreamReadI16(bs, &self->sTypoDescender));
  TRY(BitstreamReadI16(bs, &self->sTypoLineGap));
  TRY(BitstreamReadU16(bs, &self->usWinAscent));
  TRY(BitstreamReadU16(bs, &self->usWinDescent));
  switch (self->version) {
    case 1:
      TRY(BitstreamReadU32(bs, &self->ulCodePageRange1));
      TRY(BitstreamReadU32(bs, &self->ulCodePageRange2));
      break;
    case 2:
    case 3:
    case 4:
      TRY(BitstreamReadI16(bs, &self->sxHeight));
      TRY(BitstreamReadI16(bs, &self->sCapHeight));
      TRY(BitstreamReadU16(bs, &self->usDefaultChar));
      TRY(BitstreamReadU16(bs, &self->usBreakChar));
      TRY(BitstreamReadU16(bs, &self->usMaxContext));
      break;
    case 5:
      TRY(BitstreamReadU16(bs, &self->usLowerOpticalPointSize));
      TRY(BitstreamReadU16(bs, &self->usUpperOpticalPointSize));
      break;
  }

  return OK;
}

Result CmapTableParse(CmapTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->version));
  TRY(BitstreamReadU16(bs, &self->numTables));

  ASSERT_ALLOC(EncodingRecord, self->numTables, self->encodingRecords);
  for (int i = 0; i < self->numTables; i++) {
    EncodingRecordParse(&self->encodingRecords[i], bs);
  }

  return OK;
}

Result EncodingRecordParse(EncodingRecord *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->platformID));
  TRY(BitstreamReadU16(bs, &self->encodingID));
  TRY(BitstreamReadU32(bs, &self->subtableOffset));

  return OK;
}

void CmapTableFree(CmapTable *self) { free(self->encodingRecords); }

Result CvtTableParse(CvtTable *self, Bitstream *bs) {
  self->size = bs->size / sizeof(i16);
  ASSERT_ALLOC(i16, self->size, self->instructions);
  for (u32 i = 0; i < self->size; i++) {
    TRY(BitstreamReadI16(bs, &self->instructions[i]));
  }

  return OK;
}

void CvtTableFree(CvtTable *self) { free(self->instructions); }

Result FpgmTableParse(FpgmTable *self, Bitstream *bs) {
  self->size = bs->size;
  ASSERT_ALLOC(u8, self->size, self->instructions);
  TRY(BitstreamReadBuf(bs, self->instructions, self->size));

  return OK;
}

void FpgmTableFree(FpgmTable *self) { free(self->instructions); }

Result GaspTableParse(GaspTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->version));
  TRY(BitstreamReadU16(bs, &self->numRanges));

  ASSERT_ALLOC(GaspRange, self->numRanges, self->gaspRanges);
  for (int i = 0; i < self->numRanges; i++) {
    GaspRangeParse(&self->gaspRanges[i], bs);
  }

  return OK;
}

Result GaspRangeParse(GaspRange *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->rangeMaxPPEM));
  TRY(BitstreamReadU16(bs, &self->rangeGaspBehavior));

  return OK;
}

void GaspTableFree(GaspTable *self) { free(self->gaspRanges); }

Result MaxpTableParse(MaxpTable *self, Bitstream *bs) {
  TRY(BitstreamReadU32(bs, &self->version));
  TRY(BitstreamReadU16(bs, &self->numGlyphs));

  if (self->version == 0x00010000) {
    TRY(BitstreamReadU16(bs, &self->maxPoints));
    TRY(BitstreamReadU16(bs, &self->maxContours));
    TRY(BitstreamReadU16(bs, &self->maxCompositePoints));
    TRY(BitstreamReadU16(bs, &self->maxCompositeContours));
    TRY(BitstreamReadU16(bs, &self->maxZones));
    TRY(BitstreamReadU16(bs, &self->maxTwilightPoints));
    TRY(BitstreamReadU16(bs, &self->maxStorage));
    TRY(BitstreamReadU16(bs, &self->maxFunctionDefs));
    TRY(BitstreamReadU16(bs, &self->maxInstructionDefs));
    TRY(BitstreamReadU16(bs, &self->maxStackElements));
    TRY(BitstreamReadU16(bs, &self->maxSizeOfInstructions));
    TRY(BitstreamReadU16(bs, &self->maxComponentElements));
    TRY(BitstreamReadU16(bs, &self->maxComponentDepth));
  }

  return OK;
}

Result HeadTableParse(HeadTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->majorVersion));
  TRY(BitstreamReadU16(bs, &self->minorVersion));
  TRY(BitstreamReadI32(bs, &self->fontRevision));
  TRY(BitstreamReadU32(bs, &self->checksumAdjustment));
  TRY(BitstreamReadU32(bs, &self->magicNumber));
  TRY(BitstreamReadU16(bs, &self->flags));
  TRY(BitstreamReadU16(bs, &self->unitsPerEm));
  TRY(BitstreamReadI64(bs, &self->created));
  TRY(BitstreamReadI64(bs, &self->modified));
  TRY(BitstreamReadI16(bs, &self->xMin));
  TRY(BitstreamReadI16(bs, &self->yMin));
  TRY(BitstreamReadI16(bs, &self->xMax));
  TRY(BitstreamReadI16(bs, &self->yMax));
  TRY(BitstreamReadU16(bs, &self->macStyle));
  TRY(BitstreamReadU16(bs, &self->lowestRecPPEM));
  TRY(BitstreamReadI16(bs, &self->fontDirectionHint));
  i16 locFormat;
  TRY(BitstreamReadI16(bs, &locFormat));
  self->indexToLocFormat = (LocFormat)locFormat;
  TRY(BitstreamReadI16(bs, &self->glyphDataFormat));

  return OK;
}

Result LocaTableParse(LocaTable *self, Bitstream *bs, const HeadTable *head) {
  switch (head->indexToLocFormat) {
    case LocFormat_Short: // Offset16
      self->size = bs->size / sizeof(u16);
      ASSERT_ALLOC(u16, self->size, self->offsets);
      for (u32 i = 0; i < self->size; i++) {
        TRY(BitstreamReadU16(bs, &((u16 *)self->offsets)[i]));
      }
      break;
    case LocFormat_Long: // Offset32
      self->size = bs->size / sizeof(u32);
      ASSERT_ALLOC(u32, self->size, self->offsets);
      for (u32 i = 0; i < self->size; i++) {
        TRY(BitstreamReadU32(bs, &((u32 *)self->offsets)[i]));
      }
      break;
  }

  return OK;
}

Result NameRecordParse(NameRecord *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->platformID));
  TRY(BitstreamReadU16(bs, &self->encodingID));
  TRY(BitstreamReadU16(bs, &self->languageID));
  TRY(BitstreamReadU16(bs, &self->nameID));
  TRY(BitstreamReadU16(bs, &self->length));
  TRY(BitstreamReadU16(bs, &self->stringOffset));

  return OK;
}

Result LangTagRecordParse(LangTagRecord *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->length));
  TRY(BitstreamReadU16(bs, &self->langTagOffset));

  return OK;
}

Result NameTableParse(NameTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->version));
  TRY(BitstreamReadU16(bs, &self->count));
  TRY(BitstreamReadU16(bs, &self->storageOffset));

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

  // ASSERT_ALLOC(char *, self->count, self->strings);
  // NameRecord *record;
  // Bitstream data;
  // for (int i = 0; i < self->count; i++) {
  //   record = &self->nameRecord[i];
  //   BitstreamSlice(&data, bs, self->storageOffset + record->stringOffset, record->length);
  //   self->strings[i] = malloc(record->length + 1);
  //   TRY(BitstreamReadStr(&data, self->strings[i], record->length));
  // }

  return OK;
}

void NameTableFree(NameTable *self) {
  free(self->nameRecord);
  if (self->version == 1) { free(self->langTagRecord); }
  // for (int i = 0; i < self->count; i++) {
  //   free(self->strings[i]);
  // }
  // free(self->strings);
}

void LocaTableFree(LocaTable *self) { free(self->offsets); }
