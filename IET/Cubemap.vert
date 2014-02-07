#version 330

in vec3 vPosition;

out vec3 fTexCoords;

uniform mat4 P, V;

void main () 
{
  fTexCoords = vPosition;

  gl_Position = P * V * vec4 (vPosition, 1.0);
}