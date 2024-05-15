#include "defs.h"
#include "ttf/tables.h"

bool isPointInTriangle(NormalizedGlyphPoint a, NormalizedGlyphPoint b, NormalizedGlyphPoint c, NormalizedGlyphPoint p) {
  f32 area1 = (a.x - p.x) * (b.y - p.y) - (b.x - p.x) * (a.y - p.y);
  f32 area2 = (b.x - p.x) * (c.y - p.y) - (c.x - p.x) * (b.y - p.y);
  f32 area3 = (c.x - p.x) * (a.y - p.y) - (a.x - p.x) * (c.y - p.y);

  if ((area1 > 0 && area2 > 0 && area3 > 0) || (area1 < 0 && area2 < 0 && area3 < 0)) { return true; }
  return false;
}

bool isEar(NormalizedGlyphPoint *vertices, int n, int i, int *indices) {
  int prev = (i - 1 + n) % n;
  int next = (i + 1) % n;

  NormalizedGlyphPoint p0 = vertices[indices[prev]];
  NormalizedGlyphPoint p1 = vertices[indices[i]];
  NormalizedGlyphPoint p2 = vertices[indices[next]];

  if (((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y)) >= 0) { return false; }

  for (int j = 0; j < n; j++) {
    if (j == prev || j == i || j == next) { continue; }
    if (isPointInTriangle(p0, p1, p2, vertices[indices[j]])) { return false; }
  }
  return true;
}

void earClippingTriangulation(NormalizedGlyphPoint *vertices, int n, uint *outIndices) {
  if (n < 3) { return; }

  int *indices = (int *)malloc(n * sizeof(int));
  for (int i = 0; i < n; i++) {
    indices[i] = i;
  }

  int remainingVertices = n;
  int outIndex = 0;

  while (remainingVertices > 3) {
    bool earFound = false;

    for (int i = 0; i < remainingVertices; i++) {
      if (isEar(vertices, remainingVertices, i, indices)) {
        int prev = (i - 1 + remainingVertices) % remainingVertices;
        int next = (i + 1) % remainingVertices;

        outIndices[outIndex++] = indices[prev];
        outIndices[outIndex++] = indices[i];
        outIndices[outIndex++] = indices[next];

        for (int j = i; j < remainingVertices - 1; j++) {
          indices[j] = indices[j + 1];
        }
        remainingVertices--;

        earFound = true;
        break;
      }
    }

    if (!earFound) {
      free(indices);
      return;
    }
  }

  outIndices[outIndex++] = indices[0];
  outIndices[outIndex++] = indices[1];
  outIndices[outIndex++] = indices[2];

  free(indices);
}
