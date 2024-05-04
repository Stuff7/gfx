#include "math.h"

Quad Quad_new(f32 w, f32 h) {
  w *= 0.5f;
  h *= 0.5f;
  return (Quad){
      {{-w, h, .0f}},
      {{w, h, .0f}},
      {{w, -h, .0f}},
      {{-w, -h, .0f}},
  };
}

void Quad_indices(uint *indices) {
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;
  indices[3] = 2;
  indices[4] = 3;
  indices[5] = 0;
}

Cube Cube_new(f32 width, f32 height, f32 depth) {
  width *= 0.5f;
  height *= 0.5f;
  depth *= 0.5f;
  return (Cube){// Front face
                .ftl = {{-width, height, depth}},
                .ftr = {{width, height, depth}},
                .fbl = {{width, -height, depth}},
                .fbr = {{-width, -height, depth}},
                // Back face
                .btl = {{-width, height, -depth}},
                .btr = {{width, height, -depth}},
                .bbl = {{width, -height, -depth}},
                .bbr = {{-width, -height, -depth}}
  };
}

void Cube_indices(uint *indices) {
  // Front face
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;
  indices[3] = 2;
  indices[4] = 3;
  indices[5] = 0;

  // Back face
  indices[6] = 4;
  indices[7] = 5;
  indices[8] = 6;
  indices[9] = 6;
  indices[10] = 7;
  indices[11] = 4;

  // Top face
  indices[12] = 0;
  indices[13] = 1;
  indices[14] = 5;
  indices[15] = 5;
  indices[16] = 4;
  indices[17] = 0;

  // Bottom face
  indices[18] = 3;
  indices[19] = 2;
  indices[20] = 6;
  indices[21] = 6;
  indices[22] = 7;
  indices[23] = 3;

  // Left face
  indices[24] = 0;
  indices[25] = 3;
  indices[26] = 7;
  indices[27] = 7;
  indices[28] = 4;
  indices[29] = 0;

  // Right face
  indices[30] = 1;
  indices[31] = 2;
  indices[32] = 6;
  indices[33] = 6;
  indices[34] = 5;
  indices[35] = 1;
}

void Color_set(Color *c, u8 r, u8 g, u8 b) {
  c->r = r;
  c->g = g;
  c->b = b;
}
