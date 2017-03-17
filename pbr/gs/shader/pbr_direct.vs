#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;
out vec3 worldPos;
out vec3 worldNormal;

uniform mat4 MVP;
uniform mat4 Model;

void main()
{
//fragNormal = normalize(vec3(Model * vec4(normal,0)));
fragNormal = transpose(inverse(mat3(Model)))*(normal);
fragPos = (Model * vec4(position, 1)).xyz;
fragUV = uv;
worldPos = position;
gl_Position = MVP * vec4(position.x, position.y, position.z, 1.0);
}