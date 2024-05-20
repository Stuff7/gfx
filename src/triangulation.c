#include "math.h"
#include "utils.h"
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

u16 closestPoint(Vec2 self, Vec2 *points, u16 numPoints) {
  u16 closest = 0;
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

Vec2 *polybridge(Vec2 *points, u16 *pNumPoints, u16 *holeIndices, u16 numHoles) {
  u16 numPoints = *pNumPoints;
  Vec2 *allHolesCpy;

  u16 allHolesIdx = holeIndices[0] + 1;
  {
    u64 allHolesSize = (numPoints - allHolesIdx) * sizeof(Vec2);
    allHolesCpy = malloc(allHolesSize);
    memcpy(allHolesCpy, points + allHolesIdx, allHolesSize);
  }

  *pNumPoints = *pNumPoints + 2;
  points = realloc(points, *pNumPoints * sizeof(Vec2));

  for (u16 i = 0; i < numHoles; i++) {
    u16 holeStart = holeIndices[i] + 1;
    u16 holeEnd = i + 1 < numHoles ? holeIndices[i + 1] + 1 : numPoints;
    u16 numHolePoints = holeEnd - holeStart;
    if (numHolePoints <= 0) { break; }

    Vec2 *hole = &points[holeStart];
    Vec2 *holeCpy = &allHolesCpy[holeStart - allHolesIdx];

    hole[0] = points[0];
    u16 closest = closestPoint(points[0], holeCpy, numHolePoints);
    for (u16 j = 1; j < numHolePoints + 2; j++) {
      hole[j] = holeCpy[closest];
      closest = (closest + 1) % numHolePoints;
    }
  }

  free(allHolesCpy);

  return points;
}

uint *earClippingTriangulation(Vec2 *vertices, uint numVertices, uint *numOutIndices) {
  if (numVertices < 3) {
    *numOutIndices = 0;
    return NULL;
  }

  uint *indices = malloc(numVertices * sizeof(uint));
  for (uint i = 0; i < numVertices; i++) {
    indices[i] = i;
  }

  *numOutIndices = (numVertices - 2) * 3;
  uint *outIndices = calloc(*numOutIndices, sizeof(uint));

  int remainingVertices = numVertices;
  uint numIndices = 0;

  while (remainingVertices > 3) {
    bool earFound = false;

    for (int i = 0; i < remainingVertices; i++) {
      if (isEar(vertices, remainingVertices, i, indices)) {
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

    if (!earFound) { goto ret; }
  }

  outIndices[numIndices++] = indices[0];
  outIndices[numIndices++] = indices[1];
  outIndices[numIndices++] = indices[2];

ret:
  if (numIndices != *numOutIndices) {
    *numOutIndices = numIndices;
    outIndices = realloc(outIndices, numIndices * sizeof(uint));
  }

  free(indices);
  return outIndices;
}
