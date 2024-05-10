#include "game.h"
#include "renderer.h"
#include "ttf/tables.h"
#include "utils.h"

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Usage: %s <font|gl> <fontPath>|[fps]\n", argv[0]);
    return -1;
  }
  char *cmd = argv[1];

  if (streq(cmd, "font")) {
    if (argc < 3) {
      printf("Missing font path\n");
      return -1;
    }

    Bitstream bs;
    if (UNWRAP(Bitstream_fromFile(&bs, argv[2]))) { return -1; }
    TableDir table;
    if (UNWRAP(TableDir_parse(&table, &bs))) { goto Bitstream_free; }

    Bitstream data;
    if (UNWRAP(Bitstream_slice(&data, &bs, 0, bs.size))) { goto Bitstream_free; }
    GDEFTable gdef;
    if (UNWRAP(TableDir_findTable(&table, TableTag_GDEF, &data))) { goto Bitstream_free; }
    if (data.size) {
      if (UNWRAP(GDEFTable_parse(&gdef, &data))) { goto Bitstream_free; }
    }
    GPOSTable gpos;
    if (UNWRAP(TableDir_findTable(&table, TableTag_GPOS, &data))) { goto Bitstream_free; }
    if (data.size) {
      if (UNWRAP(GPOSTable_parse(&gpos, &data))) { goto Bitstream_free; }
    }
    GSUBTable gsub;
    if (UNWRAP(TableDir_findTable(&table, TableTag_GSUB, &data))) { goto Bitstream_free; }
    if (UNWRAP(GPOSTable_parse(&gsub, &data))) { goto Bitstream_free; }
    OS2Table os2;
    if (UNWRAP(TableDir_findTable(&table, TableTag_OS2, &data))) { goto Bitstream_free; }
    if (UNWRAP(OS2Table_parse(&os2, &data))) { goto Bitstream_free; }
    CvtTable cvt;
    if (UNWRAP(TableDir_findTable(&table, TableTag_Cvt, &data))) { goto Bitstream_free; }
    if (UNWRAP(CvtTable_parse(&cvt, &data))) { goto Bitstream_free; }
    FpgmTable fpgm;
    if (UNWRAP(TableDir_findTable(&table, TableTag_Fpgm, &data))) { goto CvtTable_free; }
    if (UNWRAP(FpgmTable_parse(&fpgm, &data))) { goto CvtTable_free; }
    GaspTable gasp;
    if (UNWRAP(TableDir_findTable(&table, TableTag_Gasp, &data))) { goto FpgmTable_free; }
    if (UNWRAP(GaspTable_parse(&gasp, &data))) { goto FpgmTable_free; }
    NameTable name;
    if (UNWRAP(TableDir_findTable(&table, TableTag_Name, &data))) { goto GaspTable_free; }
    if (UNWRAP(NameTable_parse(&name, &data))) { goto GaspTable_free; }

    GlyphParser glyph;
    if (UNWRAP(GlyphParser_new(&glyph, &table))) { goto NameTable_free; }

    char **strings = malloc(name.count * sizeof(char *));
    for (int i = 0; i < name.count; i++) {
      if (UNWRAP(NameRecord_getString(&name.nameRecord[i], &name, &strings[i]))) { goto GlyphParser_free; }
    }

    for (int i = 0; i < name.count; i++) {
      free(strings[i]);
    }
    free(strings);

  GlyphParser_free:
    GlyphParser_free(&glyph);
  NameTable_free:
    NameTable_free(&name);
  GaspTable_free:
    GaspTable_free(&gasp);
  FpgmTable_free:
    FpgmTable_free(&fpgm);
  CvtTable_free:
    CvtTable_free(&cvt);
  Bitstream_free:
    Bitstream_free(&bs);
  }
  else if (streq(cmd, "gl")) {
    window.renderFps = argc > 2 && streq(argv[2], "fps");
    State state = State_new();
    if (UNWRAP(State_createScene(&state, "Gfx"))) { State_free(&state); }
    Window_run();
    State_free(&state);
  }
}
