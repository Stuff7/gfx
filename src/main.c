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
    if (UNWRAP(TableDir_parse(&table, &bs))) { return -1; }

    Bitstream data;
    if (UNWRAP(Bitstream_slice(&data, &bs, 0, bs.size))) { return -1; }
    GDEFTable gdef;
    if (UNWRAP(TableDir_findTable(&table, TableTag_GDEF, &data))) { return -1; }
    if (data.size) {
      if (UNWRAP(GDEFTable_parse(&gdef, &data))) { return -1; }
    }
    GPOSTable gpos;
    if (UNWRAP(TableDir_findTable(&table, TableTag_GPOS, &data))) { return -1; }
    if (data.size) {
      if (UNWRAP(GPOSTable_parse(&gpos, &data))) { return -1; }
    }
    GSUBTable gsub;
    if (UNWRAP(TableDir_findTable(&table, TableTag_GSUB, &data))) { return -1; }
    if (UNWRAP(GPOSTable_parse(&gsub, &data))) { return -1; }
    OS2Table os2;
    if (UNWRAP(TableDir_findTable(&table, TableTag_OS2, &data))) { return -1; }
    if (UNWRAP(OS2Table_parse(&os2, &data))) { return -1; }
    CvtTable cvt;
    if (UNWRAP(TableDir_findTable(&table, TableTag_Cvt, &data))) { return -1; }
    if (UNWRAP(CvtTable_parse(&cvt, &data))) { return -1; }
    FpgmTable fpgm;
    if (UNWRAP(TableDir_findTable(&table, TableTag_Fpgm, &data))) { return -1; }
    if (UNWRAP(FpgmTable_parse(&fpgm, &data))) { return -1; }
    GaspTable gasp;
    if (UNWRAP(TableDir_findTable(&table, TableTag_Gasp, &data))) { return -1; }
    if (UNWRAP(GaspTable_parse(&gasp, &data))) { return -1; }
    NameTable name;
    if (UNWRAP(TableDir_findTable(&table, TableTag_Name, &data))) { return -1; }
    if (UNWRAP(NameTable_parse(&name, &data))) { return -1; }

    GlyphParser glyph;
    if (UNWRAP(GlyphParser_new(&glyph, &table))) {
      goto cleanup;
      return -1;
    }

    char **strings = malloc(name.count * sizeof(char *));
    for (int i = 0; i < name.count; i++) {
      if (UNWRAP(NameRecord_getString(&name.nameRecord[i], &name, &strings[i]))) { return -1; }
    }

    for (int i = 0; i < name.count; i++) {
      free(strings[i]);
    }
    free(strings);

  cleanup:
    GlyphParser_free(&glyph);
    CvtTable_free(&cvt);
    FpgmTable_free(&fpgm);
    GaspTable_free(&gasp);
    NameTable_free(&name);
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
