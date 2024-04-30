#include "font.h"
#include "game.h"
#include "renderer.h"
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
    if (isErr(BitstreamFromFile(&bs, argv[2]))) { return -1; }
    TableDir table;
    if (isErr(TableDirParse(&table, &bs))) { return -1; }

    GDEFTable gdef;
    TableDirGetTable(&table, TableTag_GDEF, &gdef);
    GPOSTable gpos;
    TableDirGetTable(&table, TableTag_GPOS, &gpos);
    GSUBTable gsub;
    TableDirGetTable(&table, TableTag_GSUB, &gsub);
    OS2Table os2;
    TableDirGetTable(&table, TableTag_OS2, &os2);
    CmapTable cmap;
    TableDirGetTable(&table, TableTag_Cmap, &cmap);
    CvtTable cvt;
    TableDirGetTable(&table, TableTag_Cvt, &cvt);
    FpgmTable fpgm;
    TableDirGetTable(&table, TableTag_Fpgm, &fpgm);
    GaspTable gasp;
    TableDirGetTable(&table, TableTag_Gasp, &gasp);
    LocaTable loca;
    TableDirGetTable(&table, TableTag_Loca, &loca);
    MaxpTable maxp;
    TableDirGetTable(&table, TableTag_Maxp, &maxp);
    NameTable name;
    TableDirGetTable(&table, TableTag_Name, &name);

    CmapTableFree(&cmap);
    CvtTableFree(&cvt);
    FpgmTableFree(&fpgm);
    GaspTableFree(&gasp);
    LocaTableFree(&loca);
    NameTableFree(&name);
    BitstreamDestroy(&bs);
  } else if (strcmp(cmd, "gl") == 0) {
    window.renderFps = argc > 2 && !strcmp(argv[2], "fps");
    State state = StateNew();
    if (isErr(StateCreateScene(&state, "Gfx"))) { StateDestroy(&state); }
    WindowRun();
    StateDestroy(&state);
  }
}
