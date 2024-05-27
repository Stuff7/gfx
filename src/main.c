#include "examples/cubes.h"
#include "examples/loading_fonts.h"
#include "examples/render_glyphs.h"
#include "renderer.h"
#include "result.h"
#include "utils.h"

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Usage: %s <font|gl|glyphs> <fontPath>|[fps]|<fontPath>\n", argv[0]);
    return -1;
  }
  char *cmd = argv[1];

  if (streq(cmd, "font")) {
    if (argc < 3) {
      printf("Missing font path\n");
      return -1;
    }

    FontInfo font;
    if (UNWRAP(loadFont(&font, argv[2]))) { return -1; }
    unloadFont(&font);
    return 0;
  }
  else if (streq(cmd, "gl")) {
    window.renderFps = argc > 2 && streq(argv[2], "fps");
    State state = State_new();
    if (UNWRAP(State_createScene(&state, "Gfx"))) { return -1; }
    Window_run();
    State_free(&state);
  }
  else if (streq(cmd, "glyphs")) {
    if (argc < 3) {
      printf("Missing font path\n");
      return -1;
    }
    RenderGlyphsState state;
    if (UNWRAP(RenderGlyphsState_createScene(&state, argv[2], "Glyphs"))) { return -1; }
    Window_run();
    RenderGlyphsState_free(&state);
  }
}
