float PI = 3.141592654;

vec3 ImportanceSampleGGX(vec2 xi, float roughness, vec3 normal)
{
	float a = roughness * roughness;

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

	return tx * h.x + ty * h.y + up * h.z;
}