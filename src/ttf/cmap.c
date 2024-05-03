#include "tables.h"
#include "types.h"

const char *PlatformIDString(PlatformID platform) {
  switch (platform) {
    case PlatformID_Unicode:
      return "Unicode";
    case PlatformID_Macintosh:
      return "Macintosh";
    case PlatformID_ISO:
      return "ISO";
    case PlatformID_Windows:
      return "Windows";
    case PlatformID_Custom:
      return "Custom";
  }

  return "Unknown";
}

const char *EncodingIDString(PlatformID platform, EncodingID encoding) {
  switch (platform) {
    case PlatformID_Unicode:
      switch (encoding.unicode) {
        case EncodingIDUnicode_1_0:
          return "EncodingIDUnicode_1_0";
        case EncodingIDUnicode_1_1:
          return "EncodingIDUnicode_1_1";
        case EncodingIDUnicode_ISO_IEC_10646:
          return "EncodingIDUnicode_ISO_IEC_10646";
        case EncodingIDUnicode_2_0_BMP:
          return "EncodingIDUnicode_2_0_BMP";
        case EncodingIDUnicode_2_0_Full:
          return "EncodingIDUnicode_2_0_Full";
        case EncodingIDUnicode_Variation:
          return "EncodingIDUnicode_Variation";
        case EncodingIDUnicode_Full:
          return "EncodingIDUnicode_Full";
      }
      break;
    case PlatformID_Macintosh:
      switch (encoding.mac) {
        case EncodingIDMacintosh_Roman:
          return "EncodingIDMacintosh_Roman";
      }
      break;
    case PlatformID_Windows:
      switch (encoding.windows) {
        case EncodingIDWindows_Symbol:
          return "EncodingIDWindows_Symbol";
        case EncodingIDWindows_UnicodeBMP:
          return "EncodingIDWindows_UnicodeBMP";
        case EncodingIDWindows_ShiftJIS:
          return "EncodingIDWindows_ShiftJIS";
        case EncodingIDWindows_PRC:
          return "EncodingIDWindows_PRC";
        case EncodingIDWindows_Big5:
          return "EncodingIDWindows_Big5";
        case EncodingIDWindows_Wansung:
          return "EncodingIDWindows_Wansung";
        case EncodingIDWindows_Johab:
          return "EncodingIDWindows_Johab";
        case EncodingIDWindows_Reserved7:
          return "EncodingIDWindows_Reserved7";
        case EncodingIDWindows_Reserved8:
          return "EncodingIDWindows_Reserved8";
        case EncodingIDWindows_Reserved9:
          return "EncodingIDWindows_Reserved9";
        case EncodingIDWindows_UnicodeFull:
          return "EncodingIDWindows_UnicodeFull";
      }
      break;
    default:
      break;
  }

  return "Unknown";
}

Result *PlatformEncodingIDParse(PlatformID *platform, EncodingID *encoding, Bitstream *bs) {
  ENUM_PARSE(bs, u16, U16, PlatformID, *platform);

  switch (*platform) {
    case PlatformID_Unicode:
      ENUM_PARSE(bs, u16, U16, EncodingIDUnicode, encoding->unicode);
      break;
    case PlatformID_Macintosh:
      ENUM_PARSE(bs, u16, U16, EncodingIDMacintosh, encoding->mac);
      break;
    case PlatformID_Windows:
      ENUM_PARSE(bs, u16, U16, EncodingIDWindows, encoding->windows);
      break;
    default:
      ASSERT(
          false,
          "Unsupported platform\n\tExpected: Unicode | Mac | Windows\n\tReceived: %s",
          PlatformIDString(*platform)
      );
  }

  return OK;
}

Result *CmapTableFindOffset(CmapTable *self, int *offset) {
  bool found = false;
  for (int i = 0; i < self->numTables; i++) {
    EncodingRecord *record = &self->encodingRecords[i];

    if (record->platformID == PlatformID_Windows) {
      EncodingIDWindows encodingID = record->encodingID.windows;
      found =
          (encodingID == EncodingIDWindows_Symbol || encodingID == EncodingIDWindows_UnicodeBMP ||
           encodingID == EncodingIDWindows_UnicodeFull);
    }
    else if (record->platformID == PlatformID_Unicode) {
      EncodingIDUnicode encodingID = record->encodingID.unicode;
      found =
          (encodingID == EncodingIDUnicode_1_0 || encodingID == EncodingIDUnicode_1_1 ||
           encodingID == EncodingIDUnicode_2_0_BMP || encodingID == EncodingIDUnicode_Full);
    }

    if (found) {
      *offset = record->subtableOffset;
      return OK;
    }
  }

  return ERR("Could not find any supported cmap platform/encoding");
}

Result *CmapSubtableFindGlyphIdFromCharCode(CmapSubtable *self, u16 c, u16 *glyphId) {
  for (u16 i = 0; i < self->segCount; ++i) {
    if (c > self->endCode[i]) { continue; }
    if (c < self->startCode[i]) { break; }

    if (self->idRangeOffsets[i] == 0) { *glyphId = (self->idDelta[i] + c) % 65536; }
    else {
      u16 index = self->idRangeOffsets[i] + (c - self->startCode[i] + i) * sizeof(u16);
      Bitstream bs;
      TRY(BitstreamSlice(&bs, &self->glyphIdStream, index, self->glyphIdStream.size - index));
      TRY(BitstreamReadU16(&bs, glyphId));

      if (*glyphId != 0) { *glyphId = (*glyphId + self->idDelta[i]) & 0xFFFF; }
    }
    return OK;
  }

  *glyphId = 0;
  return OK;
}

Result *CmapSubtableGetBMPCharGlyphIDMap(CmapSubtable *self, u16 *glyphIds) {
  for (u16 c = 0x0000; c < 0xFFFF; c++) {
    TRY(CmapSubtableFindGlyphIdFromCharCode(self, c, &glyphIds[c]));
  }

  return OK;
}

Result *CmapSubtableParse(CmapSubtable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->format));
  ASSERT(self->format == 4, "Unsupported cmap subtable format\n\tExpected: 4\n\tReceived: %u", self->format);

  TRY(BitstreamReadU16(bs, &self->length));
  ASSERT(
      self->length <= bs->size,
      "cmap length exceeds bitstream size\n\tBitstream: %lu\n\tCmap: %u",
      bs->size,
      self->length
  );

  TRY(BitstreamReadU16(bs, &self->language));
  TRY(BitstreamReadU16(bs, &self->segCount));
  self->segCount /= 2;
  TRY(BitstreamReadU16(bs, &self->searchRange));
  TRY(BitstreamReadU16(bs, &self->entrySelector));
  TRY(BitstreamReadU16(bs, &self->rangeShift));

  FILL_BUF(u16, U16, bs, self->segCount, self->endCode);
  TRY(BitstreamReadU16(bs, &self->reservedPad));
  ASSERT(self->reservedPad == 0, "Unexpected cmap.reservedPad value\n\tExpected: 0\n\tReceived: %u", self->reservedPad);

  FILL_BUF(u16, U16, bs, self->segCount, self->startCode);
  FILL_BUF(i16, I16, bs, self->segCount, self->idDelta);
  TRY(BitstreamSlice(&self->glyphIdStream, bs, bs->i, bs->size - bs->i));
  FILL_BUF(u16, U16, bs, self->segCount, self->idRangeOffsets);

  return OK;
}

void CmapSubtableFree(CmapSubtable *self) {
  free(self->endCode);
  free(self->startCode);
  free(self->idDelta);
  free(self->idRangeOffsets);
}

Result *CmapTableParse(CmapTable *self, Bitstream *bs) {
  TRY(BitstreamReadU16(bs, &self->version));
  ASSERT(self->version == 0, "Unsupported cmap version\n\rExpected: 0\n\tReceived: %u", self->version);
  TRY(BitstreamReadU16(bs, &self->numTables));

  ASSERT_ALLOC(EncodingRecord, self->numTables, self->encodingRecords);
  for (int i = 0; i < self->numTables; i++) {
    EncodingRecordParse(&self->encodingRecords[i], bs);
  }

  int subtableOffset;
  TRY(CmapTableFindOffset(self, &subtableOffset));
  TRY(BitstreamSlice(bs, bs, subtableOffset, bs->size - subtableOffset));
  return CmapSubtableParse(&self->subtable, bs);
}

Result *EncodingRecordParse(EncodingRecord *self, Bitstream *bs) {
  TRY(PlatformEncodingIDParse(&self->platformID, &self->encodingID, bs));
  TRY(BitstreamReadU32(bs, &self->subtableOffset));

  return OK;
}

void CmapTableFree(CmapTable *self) {
  free(self->encodingRecords);
  CmapSubtableFree(&self->subtable);
}
