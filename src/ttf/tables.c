#include "tables.h"
#include "types.h"

Result *GDEFTable_parse(GDEFTable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->majorVersion));
  TRY(Bitstream_readU16(bs, &self->minorVersion));
  TRY(Bitstream_readU16(bs, &self->glyphClassDefOffset));
  TRY(Bitstream_readU16(bs, &self->attachListOffset));
  TRY(Bitstream_readU16(bs, &self->ligCaretListOffset));
  TRY(Bitstream_readU16(bs, &self->markAttachClassDefOffset));

  switch (self->minorVersion) {
    case 2:
      TRY(Bitstream_readU16(bs, &self->markGlyphSetsDefOffset));
      break;
    case 3:
      TRY(Bitstream_readU16(bs, &self->itemVarStoreOffset));
      break;
  }
  return OK;
}

Result *GPOSTable_parse(GPOSTable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->majorVersion));
  TRY(Bitstream_readU16(bs, &self->minorVersion));
  TRY(Bitstream_readU16(bs, &self->scriptListOffset));
  TRY(Bitstream_readU16(bs, &self->featureListOffset));
  TRY(Bitstream_readU16(bs, &self->lookupListOffset));

  if (self->minorVersion == 1) { TRY(Bitstream_readU32(bs, &self->featureVariationsOffset)); }
  return OK;
}

Result *CvtTable_parse(CvtTable *self, Bitstream *bs) {
  self->size = bs->size / sizeof(i16);
  ASSERT_ALLOC(i16, self->size, self->instructions);
  for (u32 i = 0; i < self->size; i++) {
    TRY(Bitstream_readI16(bs, &self->instructions[i]));
  }

  return OK;
}

void CvtTable_free(CvtTable *self) { free(self->instructions); }

Result *FpgmTable_parse(FpgmTable *self, Bitstream *bs) {
  self->size = bs->size;
  ASSERT_ALLOC(u8, self->size, self->instructions);
  TRY(Bitstream_readBuf(bs, self->instructions, self->size));

  return OK;
}

void FpgmTable_free(FpgmTable *self) { free(self->instructions); }

Result *MaxpTable_parse(MaxpTable *self, Bitstream *bs) {
  u32 version;
  TRY(Bitstream_readU32(bs, &version));
  self->major = version >> 16;
  self->minor = version << 16 >> 16;
  TRY(Bitstream_readU16(bs, &self->numGlyphs));

  if (self->major == 1) {
    TRY(Bitstream_readU16(bs, &self->maxPoints));
    TRY(Bitstream_readU16(bs, &self->maxContours));
    TRY(Bitstream_readU16(bs, &self->maxCompositePoints));
    TRY(Bitstream_readU16(bs, &self->maxCompositeContours));
    TRY(Bitstream_readU16(bs, &self->maxZones));
    TRY(Bitstream_readU16(bs, &self->maxTwilightPoints));
    TRY(Bitstream_readU16(bs, &self->maxStorage));
    TRY(Bitstream_readU16(bs, &self->maxFunctionDefs));
    TRY(Bitstream_readU16(bs, &self->maxInstructionDefs));
    TRY(Bitstream_readU16(bs, &self->maxStackElements));
    TRY(Bitstream_readU16(bs, &self->maxSizeOfInstructions));
    TRY(Bitstream_readU16(bs, &self->maxComponentElements));
    TRY(Bitstream_readU16(bs, &self->maxComponentDepth));
  }

  return OK;
}

Result *HeadTable_parse(HeadTable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->majorVersion));
  TRY(Bitstream_readU16(bs, &self->minorVersion));
  TRY(Bitstream_readI32(bs, &self->fontRevision));
  TRY(Bitstream_readU32(bs, &self->checksumAdjustment));
  TRY(Bitstream_readU32(bs, &self->magicNumber));
  ASSERT(
      self->magicNumber == 0x5F0F3CF5,
      "Invalid magic number\n\tExpected: 0x5F0F3CF5\n\tReceived: 0x%08X",
      self->magicNumber
  );
  TRY(Bitstream_readU16(bs, &self->flags));
  TRY(Bitstream_readU16(bs, &self->unitsPerEm));
  TRY(Bitstream_readI64(bs, &self->created));
  TRY(Bitstream_readI64(bs, &self->modified));
  TRY(Bitstream_readI16(bs, &self->xMin));
  TRY(Bitstream_readI16(bs, &self->yMin));
  TRY(Bitstream_readI16(bs, &self->xMax));
  TRY(Bitstream_readI16(bs, &self->yMax));
  TRY(Bitstream_readU16(bs, &self->macStyle));
  TRY(Bitstream_readU16(bs, &self->lowestRecPPEM));
  TRY(Bitstream_readI16(bs, &self->fontDirectionHint));
  i16 locFormat;
  TRY(Bitstream_readI16(bs, &locFormat));
  self->indexToLocFormat = (LocFormat)locFormat;
  TRY(Bitstream_readI16(bs, &self->glyphDataFormat));

  return OK;
}
