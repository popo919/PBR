#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 fragUV;
out vec4 color;
float PI = 3.14159265358979323846;

uniform samplerCube	cubemap;
uniform float roughness;
uniform vec3 cameraPos;
uniform int debugflag;

struct Meterial
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};
uniform Meterial material;

float Convert(uint bits)
{
	//float res = 0;
	//uint iter = 1;
	//while (i)
	//{
	//	if (i % 2)
	//		res += pow(0.5, iter);
	//	iter++;
	//	i /= 2;
	//}

	//return res;

	bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint numSamples)
{
	vec2 res = vec2(0.0, 0.0);
	res.x = float(i) / float(numSamples);
	res.y = Convert(i);
	return res;
}

vec3 ImportanceSampleGGX(vec2 xi, float r, vec3 normal)
{
	float a = r * r;

	float Phi = 2 * PI * xi.x;
	float cosTheta = sqrt((1-xi.y) / (1+(a * a - 1) * xi.y));
	float sinTheta = sqrt(1 - cosTheta * cosTheta);

	vec3 h;
	h.x = sinTheta * cos(Phi);
	h.y = sinTheta * sin(Phi);
	h.z = cosTheta;

	vec3 up = abs(normal.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0) ;
	vec3 tx = normalize(cross(up, normal));
	vec3 ty = normalize(cross(normal, tx));

	return tx * h.x + ty * h.y + normal * h.z;
}

float G_Smith(float roughness, float NOV, float NOL)
{
	//float k = (roughness + 1) * (roughness + 1) / 8.0;
	float k = roughness * roughness / 2.0;

	float G1 = NOV / (k + NOV * (1-k));
	float G2 = NOL / (k + NOL * (1-k));

	return G1 * G2;
}

vec3 SpecularIBL(vec3 SpecularColor, float r, vec3 N, vec3 V)
{
	vec3 debug = vec3(0,0,0);
	vec3 SpecularLighting = vec3(0,0,0);	

	uint samples = 1024;
	for(uint i = 0; i < samples; ++i)
	{
		vec2 xi = Hammersley(i, samples);
		vec3 H = ImportanceSampleGGX(xi, r, N);
		vec3 L = 2 * dot(V, H) * H - V;

		float NOV = clamp(dot(N, V), 1e-5, 1);
		float NOL = clamp(dot(N, L), 0, 1);
		float NOH = clamp(dot(N, H), 1e-5, 1);
		float VOH = clamp(dot(V, H), 1e-5, 1);

		if(NOL > 0)
		{
			vec3 SampleColor = textureLod(cubemap, L, 0).rgb;

			float G = G_Smith(r, NOV, NOL);
			float Fc = pow(1-VOH, 5); // why not NOV
			vec3 F = (1-Fc) * SpecularColor + Fc;

			SpecularLighting += SampleColor * F * G * VOH / (NOH * NOV);
			//debug += vec3(N.x*N.x*N.x, N.y*N.y*N.y, N.z*N.z*N.z);
			debug += G;
		}
	}

	if(0 == debugflag)
	return SpecularLighting / samples;
	else
	return debug / samples;
}

void main()
{
	vec3 spec = vec3(1,1,1);
	vec3 N = normalize(fragNormal);
	vec3 V = normalize(cameraPos-fragPos);
	color.rgb = SpecularIBL(spec, roughness, N, V);
	//color.rgb = normalize(fragPos);
	//color = vec4(1,1,1,1);

	///////////////////////////////////
	vec3 eyeToPoint = normalize(fragPos - cameraPos);
	vec3 reflect = reflect(eyeToPoint, fragNormal);
	//vec3 tangent = eyeToPoint - dot(eyeToPoint, fragNormal) * fragNormal;
	//vec3 refract = eyeToPoint - tangent + tangent * 0.7;

	//color = texture(cubemap, reflect);
	//color = texture(cubemap, refract);
	color.rgb = pow(color.rgb, vec3(1.0)/2.2);
	//color.rgb = normalize(cameraPos-fragPos);
}

