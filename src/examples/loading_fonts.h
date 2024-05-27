#include "../ttf/tables.h"

typedef struct {
  Bitstream bs;
  TableDir table;
  Bitstream data;
  GDEFTable gdef;
  GPOSTable gpos;
  GSUBTable gsub;
  OS2Table os2;
  CvtTable cvt;
  FpgmTable fpgm;
  GaspTable gasp;
  NameTable name;
  GlyphParser glyph;
  char **strings;
} FontInfo;

Result *loadFont(FontInfo *font, const char *ttfPath);
void unloadFont(FontInfo *font);
