#version 330 core

layout (location = 0) in vec4 iPos;
layout (location = 1) in vec4 iCol;
layout (location = 2) in mat4 iModel;

uniform mat4 proj;
uniform mat4 view;

out vec4 color;
out vec4 vertex;

void main() {
  gl_Position = proj * view * iModel * iPos;
  color = iCol;
  vertex = iPos;
}
