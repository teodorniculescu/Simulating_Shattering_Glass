#version 430

layout(location = 0) in vec3 wp;
layout(location = 1) in vec3 wn;
layout(location = 2) in float is_normal;
layout(location = 3) in vec2 tc;
layout(location = 4) in vec3 is_normal_color;
layout(location = 5) in float boat_side;
layout(location = 6) in float boat_side_refract;


uniform samplerCube texture_cubemap;
uniform sampler2D texture_1;

uniform vec3 camera_position;

layout(location = 0) out vec4 out_color;

vec4 myReflect()
{
	vec4 res;
	vec3 incident = normalize(camera_position - wp);
	incident.x = -incident.x;
	incident.y = -incident.y;
	vec3 normal = wn;
	vec3 res_reflect = reflect(incident, normal);
	if (boat_side == 1)
		res = texture2D(texture_1, tc);
	else
		res = texture(texture_cubemap, res_reflect);
	return res;
}

vec4 myRefract(float refractive_index)
{
	vec4 res;
	vec3 incident = normalize(camera_position - wp);
	incident = -incident;
	vec3 normal = wn;
	float eta = 1.0 / refractive_index;
	vec3 res_refract = refract(incident, normal, eta);
	if (boat_side_refract == 1)
		res = texture2D(texture_1, tc);
	else
		res = texture(texture_cubemap, res_refract);
	return res;
}

void main()
{
	if (is_normal == 0) {
		vec4 c1, c2;
		c1 = myReflect();
		c2 = myRefract(1.5);
		float perc = 0.4;
		out_color = perc * c1 + (1 -perc) * c2;
	} else {
		out_color = vec4(is_normal_color, 1);
	}
}