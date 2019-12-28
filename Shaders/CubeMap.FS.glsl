#version 430

layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;

uniform vec3 camera_position;

layout(location = 0) out vec4 out_color;

vec4 myReflect()
{
    // TODO - compute the reflection color value
	vec3 res_reflect = reflect(normalize(- camera_position + world_position), world_normal);
	vec4 res = texture(texture_cubemap, res_reflect);
	return res;
}

vec4 myRefract(float refractive_index)
{
	vec3 res_refract = refract(normalize(- camera_position + world_position), world_normal, 1.0/refractive_index);
	vec4 res = texture(texture_cubemap, res_refract);
	return res;
	return vec4(0.5);
}

void main()
{
	vec4 c1, c2;
    c1 = myReflect();
	c2 = myRefract(1.33);
	float prc = 0.90;
	out_color =  c2;
}
