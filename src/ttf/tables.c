#include "tables.h"
#include "types.h"

Result *GDEFTableParse(GDEFTable *self, Bitstream *bs) {
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

Result *GPOSTableParse(GPOSTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->majorVersion));
  TRY(BitstreamReadU16(bs, &self->minorVersion));
  TRY(BitstreamReadU16(bs, &self->scriptListOffset));
  TRY(BitstreamReadU16(bs, &self->featureListOffset));
  TRY(BitstreamReadU16(bs, &self->lookupListOffset));

  if (self->minorVersion == 1) { TRY(BitstreamReadU32(bs, &self->featureVariationsOffset)); }
  return OK;
}

Result *CvtTableParse(CvtTable *self, Bitstream *bs) {
  self->size = bs->size / sizeof(i16);
  ASSERT_ALLOC(i16, self->size, self->instructions);
  for (u32 i = 0; i < self->size; i++) {
    TRY(BitstreamReadI16(bs, &self->instructions[i]));
  }

  return OK;
}

void CvtTableFree(CvtTable *self) { free(self->instructions); }

Result *FpgmTableParse(FpgmTable *self, Bitstream *bs) {
  self->size = bs->size;
  ASSERT_ALLOC(u8, self->size, self->instructions);
  TRY(BitstreamReadBuf(bs, self->instructions, self->size));

  return OK;
}

void FpgmTableFree(FpgmTable *self) { free(self->instructions); }

Result *MaxpTableParse(MaxpTable *self, Bitstream *bs) {
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

Result *HeadTableParse(HeadTable *self, Bitstream *bs) {
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
