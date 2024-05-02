#include "game.h"
#include "renderer.h"
#include "ttf/tables.h"
#include "utils.h"
#include <string.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    fprintf(stderr, "Usage: %s <font|gl> <fontPath>|[fps]\n", argv[0]);
    return -1;
  }
  char *cmd = argv[1];

  if (strcmp(cmd, "font") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Missing font path\n");
      return -1;
    }

    Bitstream bs;
    if (UNWRAP(BitstreamFromFile(&bs, argv[2]))) { return -1; }
    TableDir table;
    if (UNWRAP(TableDirParse(&table, &bs))) { return -1; }

    Bitstream data;
    if (UNWRAP(BitstreamSlice(&data, &bs, 0, bs.size))) { return -1; }
    GDEFTable gdef;
    if (UNWRAP(TableDirFindTable(&table, TableTag_GDEF, &data))) { return -1; }
    if (data.size) {
      if (UNWRAP(GDEFTableParse(&gdef, &data))) { return -1; }
    }
    GPOSTable gpos;
    if (UNWRAP(TableDirFindTable(&table, TableTag_GPOS, &data))) { return -1; }
    if (data.size) {
      if (UNWRAP(GPOSTableParse(&gpos, &data))) { return -1; }
    }
    GSUBTable gsub;
    if (UNWRAP(TableDirFindTable(&table, TableTag_GSUB, &data))) { return -1; }
    if (UNWRAP(GPOSTableParse(&gsub, &data))) { return -1; }
    OS2Table os2;
    if (UNWRAP(TableDirFindTable(&table, TableTag_OS2, &data))) { return -1; }
    if (UNWRAP(OS2TableParse(&os2, &data))) { return -1; }
    CmapTable cmap;
    if (UNWRAP(TableDirFindTable(&table, TableTag_Cmap, &data))) { return -1; }
    if (UNWRAP(CmapTableParse(&cmap, &data))) { return -1; }
    CvtTable cvt;
    if (UNWRAP(TableDirFindTable(&table, TableTag_Cvt, &data))) { return -1; }
    if (UNWRAP(CvtTableParse(&cvt, &data))) { return -1; }
    FpgmTable fpgm;
    if (UNWRAP(TableDirFindTable(&table, TableTag_Fpgm, &data))) { return -1; }
    if (UNWRAP(FpgmTableParse(&fpgm, &data))) { return -1; }
    GaspTable gasp;
    if (UNWRAP(TableDirFindTable(&table, TableTag_Gasp, &data))) { return -1; }
    if (UNWRAP(GaspTableParse(&gasp, &data))) { return -1; }
    NameTable name;
    if (UNWRAP(TableDirFindTable(&table, TableTag_Name, &data))) { return -1; }
    if (UNWRAP(NameTableParse(&name, &data))) { return -1; }

    GlyphParser glyph;
    if (UNWRAP(GlyphParserNew(&glyph, &table))) {
      goto cleanup;
      return -1;
    }

    char **strings = malloc(name.count * sizeof(char *));
    for (int i = 0; i < name.count; i++) {
      if (UNWRAP(NameRecordGetString(&name.nameRecord[i], &name, &strings[i]))) { return -1; }
    }

    for (int i = 0; i < name.count; i++) {
      free(strings[i]);
    }
    free(strings);

  cleanup:
    GlyphParserDestroy(&glyph);
    CmapTableFree(&cmap);
    CvtTableFree(&cvt);
    FpgmTableFree(&fpgm);
    GaspTableFree(&gasp);
    NameTableFree(&name);
    BitstreamDestroy(&bs);
  }
  else if (strcmp(cmd, "gl") == 0) {
    window.renderFps = argc > 2 && !strcmp(argv[2], "fps");
    State state = StateNew();
    if (UNWRAP(StateCreateScene(&state, "Gfx"))) { StateDestroy(&state); }
    WindowRun();
    StateDestroy(&state);
  }
}
