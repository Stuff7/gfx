#version 330 core

in vec4 color;
in vec4 vertex;

uniform float fTime;

out vec4 FragColor;

bool isPixelOnEdge(vec3 pixelPos, float edgeWidth);

void main() {
  if (isPixelOnEdge(vertex.xyz, 0.01)) {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  } else {
    FragColor = color;
  }
}

bool isPixelOnEdge(vec3 pixelPos, float edgeWidth) {
  vec3 cubeVertices[8] = vec3[](
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(0.5, -0.5, -0.5)
  );

  int cubeEdges[24] = int[](
    0, 1, 1, 2, 2, 3, 3, 0,
    4, 5, 5, 6, 6, 7, 7, 4,
    0, 4, 1, 5, 2, 6, 3, 7
  );

  for (int i = 0; i < 12; ++i) {
    vec3 edgeStart = cubeVertices[cubeEdges[i * 2]];
    vec3 edgeEnd = cubeVertices[cubeEdges[i * 2 + 1]];
    vec3 pixelToStart = pixelPos - edgeStart;
    vec3 edgeDirection = normalize(edgeEnd - edgeStart);
    float projectionLength = dot(pixelToStart, edgeDirection);
    vec3 closestPointOnEdge = edgeStart + clamp(projectionLength, 0.0, 1.0) * edgeDirection;
    float distanceToEdge = distance(pixelPos, closestPointOnEdge);

    if (distanceToEdge < edgeWidth) {
      return true;
    }
  }

  return false;
}
