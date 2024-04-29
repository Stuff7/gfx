#include "font.h"
#include "renderer.h"

#define MAX_ALLOC_SIZE 100000

Result TableDirParse(TableDir *table, Bitstream *bs) {
  try(BitstreamReadU32(bs, &table->sfntVersion));
  try(BitstreamReadU16(bs, &table->numTables));
  try(BitstreamReadU16(bs, &table->searchRange));
  try(BitstreamReadU16(bs, &table->entrySelector));
  try(BitstreamReadU16(bs, &table->rangeShift));

  uint size = table->numTables * sizeof(TableRecord);
  Assert(size < MAX_ALLOC_SIZE, "Cmap numTables is too large");
  table->tableRecords = malloc(size);
  HeadTable head;
  for (int i = 0; i < table->numTables; i++) {
    TableRecord *record = &table->tableRecords[i];
    try(TableRecordParse(record, bs));
    Bitstream header;
    try(BitstreamSlice(&header, bs, record->offset, record->length));

    if (streq(record->tag, "GDEF")) {
      GDEFTable gdef;
      try(GDEFTableParse(&gdef, &header));
    } else if (streq(record->tag, "GPOS")) {
      GPOSTable gpos;
      try(GPOSTableParse(&gpos, &header));
    } else if (streq(record->tag, "GSUB")) {
      GSUBHeader gsub;
      try(GPOSTableParse(&gsub, &header));
    } else if (streq(record->tag, "OS/2")) {
      OS2 os2;
      try(OS2TableParse(&os2, &header));
    } else if (streq(record->tag, "cmap")) {
      CmapTable cmap;
      try(CmapTableParse(&cmap, &header));
      CmapTableFree(&cmap);
    } else if (streq(record->tag, "cvt ")) {
      CvtTable cvt;
      try(CvtTableParse(&cvt, &header));
      CvtTableFree(&cvt);
    } else if (streq(record->tag, "fpgm")) {
      FpgmTable fpgm;
      try(FpgmTableParse(&fpgm, &header));
      FpgmTableFree(&fpgm);
    } else if (streq(record->tag, "gasp")) {
      GaspTable gasp;
      try(GaspTableParse(&gasp, &header));
      GaspTableFree(&gasp);
    } else if (streq(record->tag, "maxp")) {
      MaxpTable maxp;
      try(MaxpTableParse(&maxp, &header));
    } else if (streq(record->tag, "head")) {
      try(HeadTableParse(&head, &header));
    } else if (streq(record->tag, "loca")) {
      LocaTable loca;
      try(LocaTableParse(&loca, &header, &head));
      LocaTableFree(&loca);
    } else if (streq(record->tag, "name")) {
      NameTable name;
      try(NameTableParse(&name, &header));
      NameTableFree(&name);
    }
  }

  return Ok;
}

Result TableRecordParse(TableRecord *record, Bitstream *bs) {
  try(BitstreamReadTag(bs, record->tag));
  try(BitstreamReadU32(bs, &record->checksum));
  try(BitstreamReadU32(bs, &record->offset));
  try(BitstreamReadU32(bs, &record->length));
  return Ok;
}

void TableDirDestroy(TableDir *table) { free(table->tableRecords); }

Result GDEFTableParse(GDEFTable *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->majorVersion));
  try(BitstreamReadU16(bs, &self->minorVersion));
  try(BitstreamReadU16(bs, &self->glyphClassDefOffset));
  try(BitstreamReadU16(bs, &self->attachListOffset));
  try(BitstreamReadU16(bs, &self->ligCaretListOffset));
  try(BitstreamReadU16(bs, &self->markAttachClassDefOffset));

  switch (self->minorVersion) {
    case 2:
      try(BitstreamReadU16(bs, &self->markGlyphSetsDefOffset));
      break;
    case 3:
      try(BitstreamReadU16(bs, &self->itemVarStoreOffset));
      break;
  }
  return Ok;
}

Result GPOSTableParse(GPOSTable *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->majorVersion));
  try(BitstreamReadU16(bs, &self->minorVersion));
  try(BitstreamReadU16(bs, &self->scriptListOffset));
  try(BitstreamReadU16(bs, &self->featureListOffset));
  try(BitstreamReadU16(bs, &self->lookupListOffset));

  if (self->minorVersion == 1) { try(BitstreamReadU32(bs, &self->featureVariationsOffset)); }
  return Ok;
}

Result OS2TableParse(OS2 *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->version));
  try(BitstreamReadI16(bs, &self->xAvgCharWidth));
  try(BitstreamReadU16(bs, &self->usWeightClass));
  try(BitstreamReadU16(bs, &self->usWidthClass));
  try(BitstreamReadU16(bs, &self->fsType));
  try(BitstreamReadI16(bs, &self->ySubscriptXSize));
  try(BitstreamReadI16(bs, &self->ySubscriptYSize));
  try(BitstreamReadI16(bs, &self->ySubscriptXOffset));
  try(BitstreamReadI16(bs, &self->ySubscriptYOffset));
  try(BitstreamReadI16(bs, &self->ySuperscriptXSize));
  try(BitstreamReadI16(bs, &self->ySuperscriptYSize));
  try(BitstreamReadI16(bs, &self->ySuperscriptXOffset));
  try(BitstreamReadI16(bs, &self->ySuperscriptYOffset));
  try(BitstreamReadI16(bs, &self->yStrikeoutSize));
  try(BitstreamReadI16(bs, &self->yStrikeoutPosition));
  try(BitstreamReadI16(bs, &self->sFamilyClass));
  try(BitstreamReadBuf(bs, self->panose, 10));
  try(BitstreamReadU32(bs, &self->ulUnicodeRange1));
  try(BitstreamReadU32(bs, &self->ulUnicodeRange2));
  try(BitstreamReadU32(bs, &self->ulUnicodeRange3));
  try(BitstreamReadU32(bs, &self->ulUnicodeRange4));
  try(BitstreamReadTag(bs, self->achVendID));
  try(BitstreamReadU16(bs, &self->fsSelection));
  try(BitstreamReadU16(bs, &self->usFirstCharIndex));
  try(BitstreamReadU16(bs, &self->usLastCharIndex));
  try(BitstreamReadI16(bs, &self->sTypoAscender));
  try(BitstreamReadI16(bs, &self->sTypoDescender));
  try(BitstreamReadI16(bs, &self->sTypoLineGap));
  try(BitstreamReadU16(bs, &self->usWinAscent));
  try(BitstreamReadU16(bs, &self->usWinDescent));
  switch (self->version) {
    case 1:
      try(BitstreamReadU32(bs, &self->ulCodePageRange1));
      try(BitstreamReadU32(bs, &self->ulCodePageRange2));
      break;
    case 2:
    case 3:
    case 4:
      try(BitstreamReadI16(bs, &self->sxHeight));
      try(BitstreamReadI16(bs, &self->sCapHeight));
      try(BitstreamReadU16(bs, &self->usDefaultChar));
      try(BitstreamReadU16(bs, &self->usBreakChar));
      try(BitstreamReadU16(bs, &self->usMaxContext));
      break;
    case 5:
      try(BitstreamReadU16(bs, &self->usLowerOpticalPointSize));
      try(BitstreamReadU16(bs, &self->usUpperOpticalPointSize));
      break;
  }

  return Ok;
}

Result CmapTableParse(CmapTable *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->version));
  try(BitstreamReadU16(bs, &self->numTables));

  uint size = self->numTables * sizeof(EncodingRecord);
  Assert(size < MAX_ALLOC_SIZE, "Cmap numTables is too large");
  self->encodingRecords = malloc(size);
  for (int i = 0; i < self->numTables; i++) {
    EncodingRecordParse(&self->encodingRecords[i], bs);
  }

  return Ok;
}

Result EncodingRecordParse(EncodingRecord *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->platformID));
  try(BitstreamReadU16(bs, &self->encodingID));
  try(BitstreamReadU32(bs, &self->subtableOffset));

  return Ok;
}

void CmapTableFree(CmapTable *self) { free(self->encodingRecords); }

Result CvtTableParse(CvtTable *self, Bitstream *bs) {
  self->size = bs->size / 2;
  uint size = self->size * sizeof(i16);
  Assert(size < MAX_ALLOC_SIZE, "Cvt instructions size is too large");
  self->instructions = malloc(size);
  for (u32 i = 0; i < self->size; i++) {
    try(BitstreamReadI16(bs, &self->instructions[i]));
  }

  return Ok;
}

void CvtTableFree(CvtTable *self) { free(self->instructions); }

Result FpgmTableParse(FpgmTable *self, Bitstream *bs) {
  self->size = bs->size;
  Assert(self->size < MAX_ALLOC_SIZE, "Fpgm instructions size is too large");
  self->instructions = malloc(self->size * sizeof(u8));
  try(BitstreamReadBuf(bs, self->instructions, self->size));

  return Ok;
}

void FpgmTableFree(FpgmTable *self) { free(self->instructions); }

Result GaspTableParse(GaspTable *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->version));
  try(BitstreamReadU16(bs, &self->numRanges));

  uint size = self->numRanges * sizeof(GaspRange);
  Assert(size < MAX_ALLOC_SIZE, "Gasp numRanges is too large");
  self->gaspRanges = malloc(size);
  for (int i = 0; i < self->numRanges; i++) {
    GaspRangeParse(&self->gaspRanges[i], bs);
  }

  return Ok;
}

Result GaspRangeParse(GaspRange *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->rangeMaxPPEM));
  try(BitstreamReadU16(bs, &self->rangeGaspBehavior));

  return Ok;
}

void GaspTableFree(GaspTable *self) { free(self->gaspRanges); }

Result MaxpTableParse(MaxpTable *self, Bitstream *bs) {
  try(BitstreamReadU32(bs, &self->version));
  try(BitstreamReadU16(bs, &self->numGlyphs));

  if (self->version == 0x00010000) {
    try(BitstreamReadU16(bs, &self->maxPoints));
    try(BitstreamReadU16(bs, &self->maxContours));
    try(BitstreamReadU16(bs, &self->maxCompositePoints));
    try(BitstreamReadU16(bs, &self->maxCompositeContours));
    try(BitstreamReadU16(bs, &self->maxZones));
    try(BitstreamReadU16(bs, &self->maxTwilightPoints));
    try(BitstreamReadU16(bs, &self->maxStorage));
    try(BitstreamReadU16(bs, &self->maxFunctionDefs));
    try(BitstreamReadU16(bs, &self->maxInstructionDefs));
    try(BitstreamReadU16(bs, &self->maxStackElements));
    try(BitstreamReadU16(bs, &self->maxSizeOfInstructions));
    try(BitstreamReadU16(bs, &self->maxComponentElements));
    try(BitstreamReadU16(bs, &self->maxComponentDepth));
  }

  return Ok;
}

Result HeadTableParse(HeadTable *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->majorVersion));
  try(BitstreamReadU16(bs, &self->minorVersion));
  try(BitstreamReadI32(bs, &self->fontRevision));
  try(BitstreamReadU32(bs, &self->checksumAdjustment));
  try(BitstreamReadU32(bs, &self->magicNumber));
  try(BitstreamReadU16(bs, &self->flags));
  try(BitstreamReadU16(bs, &self->unitsPerEm));
  try(BitstreamReadI64(bs, &self->created));
  try(BitstreamReadI64(bs, &self->modified));
  try(BitstreamReadI16(bs, &self->xMin));
  try(BitstreamReadI16(bs, &self->yMin));
  try(BitstreamReadI16(bs, &self->xMax));
  try(BitstreamReadI16(bs, &self->yMax));
  try(BitstreamReadU16(bs, &self->macStyle));
  try(BitstreamReadU16(bs, &self->lowestRecPPEM));
  try(BitstreamReadI16(bs, &self->fontDirectionHint));
  i16 locFormat;
  try(BitstreamReadI16(bs, &locFormat));
  self->indexToLocFormat = (LocFormat)locFormat;
  try(BitstreamReadI16(bs, &self->glyphDataFormat));

  return Ok;
}

Result LocaTableParse(LocaTable *self, Bitstream *bs, const HeadTable *head) {
  uint size;
  switch (head->indexToLocFormat) {
    case LocFormat_Short: // Offset16
      self->size = bs->size / sizeof(u16);
      size = self->size * sizeof(u16);
      Assert(size < MAX_ALLOC_SIZE, "Loca offsets size is too large");
      self->offsets = malloc(size);
      for (u32 i = 0; i < self->size; i++) {
        try(BitstreamReadU16(bs, &((u16 *)self->offsets)[i]));
      }
      break;
    case LocFormat_Long: // Offset32
      self->size = bs->size / sizeof(u32);
      size = self->size * sizeof(u32);
      Assert(size < MAX_ALLOC_SIZE, "Loca offsets size is too large");
      self->offsets = malloc(size);
      for (u32 i = 0; i < self->size; i++) {
        try(BitstreamReadU32(bs, &((u32 *)self->offsets)[i]));
      }
      break;
  }

  return Ok;
}

Result NameRecordParse(NameRecord *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->platformID));
  try(BitstreamReadU16(bs, &self->encodingID));
  try(BitstreamReadU16(bs, &self->languageID));
  try(BitstreamReadU16(bs, &self->nameID));
  try(BitstreamReadU16(bs, &self->length));
  try(BitstreamReadU16(bs, &self->stringOffset));

  return Ok;
}

Result LangTagRecordParse(LangTagRecord *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->length));
  try(BitstreamReadU16(bs, &self->langTagOffset));

  return Ok;
}

Result NameTableParse(NameTable *self, Bitstream *bs) {
  try(BitstreamReadU16(bs, &self->version));
  try(BitstreamReadU16(bs, &self->count));
  try(BitstreamReadU16(bs, &self->storageOffset));

  uint size = self->count * sizeof(NameRecord);
  Assert(size < MAX_ALLOC_SIZE, "Name table nameRecords is too large");
  self->nameRecord = malloc(size);
  for (int i = 0; i < self->count; i++) {
    try(NameRecordParse(&self->nameRecord[i], bs));
  }

  if (self->version == 1) {
    try(BitstreamReadU16(bs, &self->langTagCount));
    uint size = self->langTagCount * sizeof(NameRecord);
    Assert(size < MAX_ALLOC_SIZE, "Name table langTagRecord is too large");
    self->langTagRecord = malloc(size);
    for (int i = 0; i < self->langTagCount; i++) {
      try(LangTagRecordParse(&self->langTagRecord[i], bs));
    }
  }

  // size = self->count * sizeof(char *);
  // Assert(size < MAX_ALLOC_SIZE, "Name table strings is too large");
  // self->strings = malloc(size);
  // NameRecord *record;
  // Bitstream data;
  // for (int i = 0; i < self->count; i++) {
  //   record = &self->nameRecord[i];
  //   BitstreamSlice(&data, bs, self->storageOffset + record->stringOffset, record->length);
  //   self->strings[i] = malloc(record->length + 1);
  //   try(BitstreamReadStr(&data, self->strings[i], record->length));
  // }

  return Ok;
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
