#version 330 core
in vec3 fragDir;

out vec4 color;

uniform samplerCube cubemap;

void main()
{
	color = texture(cubemap, fragDir);
	//color = vec4(1,1,1,1);
}

