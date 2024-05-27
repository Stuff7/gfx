#include "loading_fonts.h"
#include "../ttf/tables.h"

Result *loadFont(FontInfo *font, const char *ttfPath) {
  Result *ret = OK;
  TRY(Bitstream_fromFile(&font->bs, ttfPath))
  OK_OR_GOTO(Bitstream_free, ret, TableDir_parse(&font->table, &font->bs));

  Bitstream data;
  OK_OR_GOTO(Bitstream_free, ret, Bitstream_slice(&font->data, &font->bs, 0, font->bs.size));
  OK_OR_GOTO(Bitstream_free, ret, TableDir_findTable(&font->table, TableTag_GDEF, &data));
  if (data.size) { OK_OR_GOTO(Bitstream_free, ret, GDEFTable_parse(&font->gdef, &data)); }
  OK_OR_GOTO(Bitstream_free, ret, TableDir_findTable(&font->table, TableTag_GPOS, &data));
  if (data.size) { OK_OR_GOTO(Bitstream_free, ret, GPOSTable_parse(&font->gpos, &data)); }
  OK_OR_GOTO(Bitstream_free, ret, TableDir_findTable(&font->table, TableTag_GSUB, &data));
  OK_OR_GOTO(Bitstream_free, ret, GPOSTable_parse(&font->gsub, &data));
  OK_OR_GOTO(Bitstream_free, ret, TableDir_findTable(&font->table, TableTag_OS2, &data));
  OK_OR_GOTO(Bitstream_free, ret, OS2Table_parse(&font->os2, &data));
  OK_OR_GOTO(Bitstream_free, ret, TableDir_findTable(&font->table, TableTag_Cvt, &data));
  OK_OR_GOTO(Bitstream_free, ret, CvtTable_parse(&font->cvt, &data));
  OK_OR_GOTO(CvtTable_free, ret, TableDir_findTable(&font->table, TableTag_Fpgm, &data));
  OK_OR_GOTO(CvtTable_free, ret, FpgmTable_parse(&font->fpgm, &data));
  OK_OR_GOTO(FpgmTable_free, ret, TableDir_findTable(&font->table, TableTag_Gasp, &data));
  OK_OR_GOTO(FpgmTable_free, ret, GaspTable_parse(&font->gasp, &data));
  OK_OR_GOTO(GaspTable_free, ret, TableDir_findTable(&font->table, TableTag_Name, &data));
  OK_OR_GOTO(GaspTable_free, ret, NameTable_parse(&font->name, &data));

  OK_OR_GOTO(NameTable_free, ret, GlyphParser_new(&font->glyph, &font->table));

  font->strings = malloc(font->name.count * sizeof(char *));
  for (int i = 0; i < font->name.count; i++) {
    OK_OR_GOTO(GlyphParser_free, ret, NameRecord_getString(&font->name.nameRecord[i], &font->name, &font->strings[i]));
  }
  goto ret;

GlyphParser_free:
  GlyphParser_free(&font->glyph);
NameTable_free:
  NameTable_free(&font->name);
GaspTable_free:
  GaspTable_free(&font->gasp);
FpgmTable_free:
  FpgmTable_free(&font->fpgm);
CvtTable_free:
  CvtTable_free(&font->cvt);
Bitstream_free:
  Bitstream_free(&font->bs);
ret:
  return ret;
}

void unloadFont(FontInfo *font) {
  for (int i = 0; i < font->name.count; i++) {
    free(font->strings[i]);
  }
  free(font->strings);

  GlyphParser_free(&font->glyph);
  NameTable_free(&font->name);
  GaspTable_free(&font->gasp);
  FpgmTable_free(&font->fpgm);
  CvtTable_free(&font->cvt);
  Bitstream_free(&font->bs);
}
