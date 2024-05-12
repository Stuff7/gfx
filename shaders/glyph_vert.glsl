#version 330 core

layout (location = 0) in vec4 iPos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec4 vertex;

void main() {
  gl_Position = proj * view * model * iPos;
  vertex = iPos;
}
