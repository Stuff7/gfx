#include "tables.h"

Result *OS2TableParse(OS2Table *self, Bitstream *bs) {
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
