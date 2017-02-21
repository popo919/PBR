#version 330 core
in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragUV;

out vec4 color;

uniform samplerCube cubemap;
uniform vec3 cameraPos;

struct Meterial
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform Meterial material;

void main()
{
vec3 eyeToPoint = normalize(fragPos - cameraPos);
vec3 reflect = reflect(eyeToPoint, fragNormal);
//color = texture(material.texture_diffuse1, fragUV);
color = texture(cubemap, reflect);
//color = vec4(reflect, 1);
}

