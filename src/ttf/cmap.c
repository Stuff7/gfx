#include "tables.h"
#include "types.h"

const char *PlatformID_string(PlatformID platform) {
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

const char *EncodingID_string(PlatformID platform, EncodingID encoding) {
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

Result *EncodingID_parse(PlatformID *platform, EncodingID *encoding, Bitstream *bs) {
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
          PlatformID_string(*platform)
      );
  }

  return OK;
}

Result *CmapTable_findOffset(CmapTable *self, int *offset) {
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

Result *CmapSubtable_findGlyphIdFromCharCode(CmapSubtable *self, u16 numGlyphs, u16 c, u16 *glyphId) {
  for (u16 i = 0; i < self->segCount; ++i) {
    if (c > self->endCode[i]) { continue; }
    if (c < self->startCode[i]) { break; }

    if (self->idRangeOffsets[i] == 0) { *glyphId = (self->idDelta[i] + c) % 65536; }
    else {
      u16 index = self->idRangeOffsets[i] + (c - self->startCode[i] + i) * sizeof(u16);
      Bitstream bs;
      TRY(Bitstream_slice(&bs, &self->glyphIdStream, index, self->glyphIdStream.size - index));
      TRY(Bitstream_readU16(&bs, glyphId));

      if (*glyphId != 0) { *glyphId = (*glyphId + self->idDelta[i]) & 0xFFFF; }
    }
    if (*glyphId >= numGlyphs) { return ERR("Glyph ID out of range\n\tglyphId: %u\n\tchar: %u", *glyphId, c); }
    return OK;
  }

  *glyphId = 0;
  return OK;
}

Result *CmapSubtable_parse(CmapSubtable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->format));
  ASSERT(self->format == 4, "Unsupported cmap subtable format\n\tExpected: 4\n\tReceived: %u", self->format);

  TRY(Bitstream_readU16(bs, &self->length));
  ASSERT(
      self->length <= bs->size,
      "cmap length exceeds bitstream size\n\tBitstream: %lu\n\tCmap: %u",
      bs->size,
      self->length
  );

  TRY(Bitstream_readU16(bs, &self->language));
  TRY(Bitstream_readU16(bs, &self->segCount));
  self->segCount /= 2;
  ASSERT(self->segCount >= 1, "Invalid segCount\n\tExpected: >= 1\n\tReceived: %u", self->segCount);
  TRY(Bitstream_readU16(bs, &self->searchRange));
  TRY(Bitstream_readU16(bs, &self->entrySelector));
  TRY(Bitstream_readU16(bs, &self->rangeShift));

  Result *ret = OK;
  FILL_BUF(cleanEndCode, ret, u16, U16, bs, self->segCount, self->endCode);
  TRY(Bitstream_readU16(bs, &self->reservedPad));
  ASSERT(self->reservedPad == 0, "Unexpected cmap.reservedPad value\n\tExpected: 0\n\tReceived: %u", self->reservedPad);

  FILL_BUF(cleanStartCode, ret, u16, U16, bs, self->segCount, self->startCode);
  FILL_BUF(cleanIdDelta, ret, i16, I16, bs, self->segCount, self->idDelta);
  TRY(Bitstream_slice(&self->glyphIdStream, bs, bs->i, bs->size - bs->i));
  FILL_BUF(cleanIdRangeOffsets, ret, u16, U16, bs, self->segCount, self->idRangeOffsets);
  goto ret;

cleanIdRangeOffsets:
  free(self->endCode);
cleanIdDelta:
  free(self->idDelta);
cleanStartCode:
  free(self->startCode);
cleanEndCode:
  free(self->idRangeOffsets);
ret:
  return ret;
}

void CmapSubtable_free(CmapSubtable *self) {
  free(self->endCode);
  free(self->startCode);
  free(self->idDelta);
  free(self->idRangeOffsets);
}

Result *CmapTable_parse(CmapTable *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->version));
  ASSERT(self->version == 0, "Unsupported cmap version\n\rExpected: 0\n\tReceived: %u", self->version);
  TRY(Bitstream_readU16(bs, &self->numTables));

  ASSERT_ALLOC(EncodingRecord, self->numTables, self->encodingRecords);
  for (int i = 0; i < self->numTables; i++) {
    EncodingRecord_parse(&self->encodingRecords[i], bs);
  }

  int subtableOffset = 6490;
  Result *ret = OK;
  OK_OR_GOTO(cleanRecords, ret, CmapTable_findOffset(self, &subtableOffset));
  OK_OR_GOTO(cleanRecords, ret, Bitstream_slice(bs, bs, subtableOffset, bs->size - subtableOffset));
  OK_OR_GOTO(cleanRecords, ret, CmapSubtable_parse(&self->subtable, bs));
  goto ret;

cleanRecords:
  free(self->encodingRecords);
ret:
  return ret;
}

Result *EncodingRecord_parse(EncodingRecord *self, Bitstream *bs) {
  TRY(EncodingID_parse(&self->platformID, &self->encodingID, bs));
  TRY(Bitstream_readU32(bs, &self->subtableOffset));

  return OK;
}

void CmapTable_free(CmapTable *self) {
  free(self->encodingRecords);
  CmapSubtable_free(&self->subtable);
}
