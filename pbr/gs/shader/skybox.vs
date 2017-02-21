#version 330 core
layout (location = 0) in vec3 position;

out vec3 fragDir;

uniform mat4 VP;

void main()
{
vec4 pos = VP * vec4(position, 1.0);
fragDir = position;
pos.z = 0.999 * pos.w;
gl_Position = pos;
//gl_Position = pos;
}