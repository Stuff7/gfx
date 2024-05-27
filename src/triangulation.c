#include "math.h"
#include "ttf/tables.h"
#include <float.h>
#include <string.h>

bool isEar(Vec2 *vertices, int n, int i, uint *indices) {
  int prev = (i - 1 + n) % n;
  int next = (i + 1) % n;

  Triangle t = {vertices[indices[i]], vertices[indices[prev]], vertices[indices[next]]};

  if (Triangle_area(&t) < 0) { return false; }

  for (int j = 0; j < n; j++) {
    if (j == prev || j == i || j == next) { continue; }
    if (Triangle_isPointInside(&t, vertices[indices[j]])) { return false; }
  }
  return true;
}

uint closestPoint(Vec2 self, const Vec2 *points, u16 numPoints) {
  uint closest = 0;
  f64 minDistance = Vec2_distanceSquared(self, points[closest]);

  for (u16 i = 1; i < numPoints; i++) {
    f64 distance = Vec2_distanceSquared(self, points[i]);
    if (distance < minDistance) {
      minDistance = distance;
      closest = i;
    }
  }

  return closest;
}

uint *polybridge(const NormalizedGlyph *glyph, uint *numIndices) {
  u16 numPoints = glyph->numPoints;
  u16 allHolesIdx = glyph->endPtsOfContours[0] + 1;
  Vec2 *holes = glyph->points + allHolesIdx;

  *numIndices = glyph->numPoints + 3;
  uint *indices = malloc(*numIndices * sizeof(uint));
  for (uint i = 0; i < *numIndices; i++) {
    indices[i] = i;
  }

  for (uint i = 0; i < glyph->numberOfContours; i++) {
    uint holeStart = glyph->endPtsOfContours[i] + 1;
    uint holeEnd = i + 1 < glyph->numberOfContours ? glyph->endPtsOfContours[i + 1] + 1 : numPoints;
    uint numHolePoints = holeEnd - holeStart;
    if (numHolePoints <= 0) { break; }

    uint *hole = &indices[holeStart];

    hole[0] = 0;
    uint closest = closestPoint(glyph->points[0], holes, numHolePoints);
    for (uint j = 1; j < numHolePoints + 2; j++) {
      hole[j] = closest + holeStart;
      closest = (closest + 1) % numHolePoints;
    }
    hole[numHolePoints + 2] = 0;
  }

  return indices;
}

uint *earClippingTriangulation(const NormalizedGlyph *glyph, uint *numOutIndices) {
  if (glyph->numPoints < 3) {
    *numOutIndices = 0;
    return NULL;
  }

  uint *indices = polybridge(glyph, numOutIndices);

  *numOutIndices = (glyph->numPoints) * 3;
  uint *outIndices = calloc(*numOutIndices, sizeof(uint));

  int remainingVertices = glyph->numPoints + 2;
  uint numIndices = 0;

  while (remainingVertices > 3) {
    bool earFound = false;

    for (int i = 0; i < remainingVertices; i++) {
      if (isEar(glyph->points, remainingVertices, i, indices)) {
        int prev = (i - 1 + remainingVertices) % remainingVertices;
        int next = (i + 1) % remainingVertices;

        outIndices[numIndices++] = indices[prev];
        outIndices[numIndices++] = indices[i];
        outIndices[numIndices++] = indices[next];

        remainingVertices--;
        for (int j = i; j < remainingVertices; j++) {
          indices[j] = indices[j + 1];
        }

        earFound = true;
        break;
      }
    }

    if (!earFound) { break; }
  }

  outIndices[numIndices++] = indices[0];
  outIndices[numIndices++] = indices[1];
  outIndices[numIndices++] = indices[2];

  if (numIndices != *numOutIndices) {
    *numOutIndices = numIndices;
    outIndices = realloc(outIndices, numIndices * sizeof(uint));
  }

  free(indices);
  return outIndices;
}
