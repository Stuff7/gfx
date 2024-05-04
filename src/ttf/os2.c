#include "tables.h"

Result *OS2Table_parse(OS2Table *self, Bitstream *bs) {
  TRY(Bitstream_readU16(bs, &self->version));
  TRY(Bitstream_readI16(bs, &self->xAvgCharWidth));
  TRY(Bitstream_readU16(bs, &self->usWeightClass));
  TRY(Bitstream_readU16(bs, &self->usWidthClass));
  TRY(Bitstream_readU16(bs, &self->fsType));
  TRY(Bitstream_readI16(bs, &self->ySubscriptXSize));
  TRY(Bitstream_readI16(bs, &self->ySubscriptYSize));
  TRY(Bitstream_readI16(bs, &self->ySubscriptXOffset));
  TRY(Bitstream_readI16(bs, &self->ySubscriptYOffset));
  TRY(Bitstream_readI16(bs, &self->ySuperscriptXSize));
  TRY(Bitstream_readI16(bs, &self->ySuperscriptYSize));
  TRY(Bitstream_readI16(bs, &self->ySuperscriptXOffset));
  TRY(Bitstream_readI16(bs, &self->ySuperscriptYOffset));
  TRY(Bitstream_readI16(bs, &self->yStrikeoutSize));
  TRY(Bitstream_readI16(bs, &self->yStrikeoutPosition));
  TRY(Bitstream_readI16(bs, &self->sFamilyClass));
  TRY(Bitstream_readBuf(bs, self->panose, 10));
  TRY(Bitstream_readU32(bs, &self->ulUnicodeRange1));
  TRY(Bitstream_readU32(bs, &self->ulUnicodeRange2));
  TRY(Bitstream_readU32(bs, &self->ulUnicodeRange3));
  TRY(Bitstream_readU32(bs, &self->ulUnicodeRange4));
  TRY(Bitstream_readTag(bs, self->achVendID));
  TRY(Bitstream_readU16(bs, &self->fsSelection));
  TRY(Bitstream_readU16(bs, &self->usFirstCharIndex));
  TRY(Bitstream_readU16(bs, &self->usLastCharIndex));
  TRY(Bitstream_readI16(bs, &self->sTypoAscender));
  TRY(Bitstream_readI16(bs, &self->sTypoDescender));
  TRY(Bitstream_readI16(bs, &self->sTypoLineGap));
  TRY(Bitstream_readU16(bs, &self->usWinAscent));
  TRY(Bitstream_readU16(bs, &self->usWinDescent));
  switch (self->version) {
    case 1:
      TRY(Bitstream_readU32(bs, &self->ulCodePageRange1));
      TRY(Bitstream_readU32(bs, &self->ulCodePageRange2));
      break;
    case 2:
    case 3:
    case 4:
      TRY(Bitstream_readI16(bs, &self->sxHeight));
      TRY(Bitstream_readI16(bs, &self->sCapHeight));
      TRY(Bitstream_readU16(bs, &self->usDefaultChar));
      TRY(Bitstream_readU16(bs, &self->usBreakChar));
      TRY(Bitstream_readU16(bs, &self->usMaxContext));
      break;
    case 5:
      TRY(Bitstream_readU16(bs, &self->usLowerOpticalPointSize));
      TRY(Bitstream_readU16(bs, &self->usUpperOpticalPointSize));
      break;
  }

  return OK;
}
