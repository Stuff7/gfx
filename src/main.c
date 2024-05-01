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
    if (ResultUnwrap(BitstreamFromFile(&bs, argv[2]))) { return -1; }
    TableDir table;
    if (ResultUnwrap(TableDirParse(&table, &bs))) { return -1; }

    Bitstream data;
    if (ResultUnwrap(BitstreamSlice(&data, &bs, 0, bs.size))) { return -1; }
    GDEFTable gdef;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_GDEF, &data))) { return -1; }
    if (ResultUnwrap(GDEFTableParse(&gdef, &data))) { return -1; }
    GPOSTable gpos;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_GPOS, &data))) { return -1; }
    if (ResultUnwrap(GPOSTableParse(&gpos, &data))) { return -1; }
    GSUBTable gsub;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_GSUB, &data))) { return -1; }
    if (ResultUnwrap(GPOSTableParse(&gsub, &data))) { return -1; }
    OS2Table os2;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_OS2, &data))) { return -1; }
    if (ResultUnwrap(OS2TableParse(&os2, &data))) { return -1; }
    CmapTable cmap;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_Cmap, &data))) { return -1; }
    if (ResultUnwrap(CmapTableParse(&cmap, &data))) { return -1; }
    CvtTable cvt;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_Cvt, &data))) { return -1; }
    if (ResultUnwrap(CvtTableParse(&cvt, &data))) { return -1; }
    FpgmTable fpgm;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_Fpgm, &data))) { return -1; }
    if (ResultUnwrap(FpgmTableParse(&fpgm, &data))) { return -1; }
    GaspTable gasp;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_Gasp, &data))) { return -1; }
    if (ResultUnwrap(GaspTableParse(&gasp, &data))) { return -1; }
    NameTable name;
    if (ResultUnwrap(TableDirFindTable(&table, TableTag_Name, &data))) { return -1; }
    if (ResultUnwrap(NameTableParse(&name, &data))) { return -1; }

    GlyphParser glyph;
    if (ResultUnwrap(GlyphParserNew(&glyph, &table))) {
      goto cleanup;
      return -1;
    }

    char **strings = malloc(name.count * sizeof(char *));
    for (int i = 0; i < name.count; i++) {
      NameRecordGetString(&name.nameRecord[i], &name, &strings[i]);
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
    if (ResultUnwrap(StateCreateScene(&state, "Gfx"))) { StateDestroy(&state); }
    WindowRun();
    StateDestroy(&state);
  }
}
