#define TREE_TYPE int
#include "data_structs/avl_tree.h"
#undef TREE_TYPE

#include "polygon.h"
#define TREE_TYPE StatusData
#include "data_structs/avl_tree.h"
#undef TREE_TYPE

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
  else if (streq(cmd, "test")) {
    if (argc < 3) {
      printf("Missing font path\n");
      return -1;
    }

    FontInfo font;
    if (UNWRAP(loadFont(&font, argv[2]))) { return -1; }

    const u16 ch = 0x0126;
    LOG("GLYPH: %c", ch);
    Glyph glyph;
    NormalizedGlyph normalGlyph;
    if (UNWRAP(GlyphParser_getGlyph(&font.glyph, ch, &glyph))) { goto unload_font; }
    if (UNWRAP(Glyph_normalize(&normalGlyph, &glyph, &font.glyph.head))) { goto unload_font; }

    intAVLTree bt;
    intAVLTree_new(&bt, 128);
    intAVLTree_insert(&bt, 12);
    intAVLTree_insert(&bt, 15);
    intAVLTree_insert(&bt, 8);
    intAVLTree_insert(&bt, 7);
    intAVLTree_insert(&bt, 6);
    intAVLTree_insert(&bt, 5);
    intAVLTree_insert(&bt, 4);
    intAVLTree_insert(&bt, 3);
    intAVLTree_insert(&bt, 2);
    intAVLTree_insert(&bt, 1);
    LOG("HEAD %d", bt.head->data);
    intAVLTree_string(&bt);

    intAVLTree_delete(&bt, 3);
    intAVLTree_string(&bt);
    intAVLTree_delete(&bt, 5);
    intAVLTree_string(&bt);
    intAVLTree_delete(&bt, 7);
    intAVLTree_string(&bt);

    // {
    //   intAVLIterBFS iter;
    //   intAVLNode *queue[intAVLTree_len(&bt)], *node;
    //   intAVLIterBFS_new(&iter, bt.head, queue);
    //
    //   while ((node = intAVLIterBFS_next(&iter)) != NULL) {
    //     LOG("NODE: %d", node->data);
    //   }
    // }

    // StatusDataAVLTree bts;
    // StatusDataAVLTree_new(&bts, 128);
    // StatusDataAVLTree_insert(&bts, (StatusData){.b = 12.0});
    // StatusDataAVLTree_insert(&bts, (StatusData){.b = 15.0});
    // StatusDataAVLTree_insert(&bts, (StatusData){.b = 8.0});
    // StatusDataAVLTree_insert(&bts, (StatusData){.b = 7.0});
    // StatusDataAVLTree_insert(&bts, (StatusData){.b = 6.0});
    // StatusDataAVLTree_insert(&bts, (StatusData){.b = 5.0});
    // LOG("HEAD %f", bts.head->data.b);
    // StatusDataAVLTree_string(&bts);
    // StatusDataAVLTree_free(&bts);

    NormalizedGlyph_free(&normalGlyph);
    intAVLTree_free(&bt);
  unload_font:
    unloadFont(&font);
  }
}
