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

    BitstreamDestroy(&bs);
    TableDirDestroy(&table);
  } else if (strcmp(cmd, "gl") == 0) {
    window.renderFps = argc > 2 && !strcmp(argv[2], "fps");
    State state = StateNew();
    if (isErr(StateCreateScene(&state, "Gfx"))) { StateDestroy(&state); }
    WindowRun();
    StateDestroy(&state);
  }
}
