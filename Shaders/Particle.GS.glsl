#version 430
layout(points) in;
layout(triangle_strip, max_vertices = 20) out;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 eye_position;
uniform float grosime;
uniform float latura;
uniform int togle_normal;
uniform float pozl;
uniform float pozh;

layout(location = 0) in vec3 rotation_woah [];
layout(location = 1) in vec3 world_position [];
layout(location = 2) in vec3 world_normal [];
layout(location = 3) in vec2 texture_coord [];
layout(location = 4) in mat4 MVP_Matrix [];

layout(location = 0) out vec3 wp;
layout(location = 1) out vec3 wn;
layout(location = 2) out float is_normal;
layout(location = 3) out vec2 tc;
layout(location = 4) out vec3 is_normal_color;
layout(location = 5) out float bs;
layout(location = 6) out float bsr;

vec3 vpos = gl_in[0].gl_Position.xyz;

mat4 rotate_x(float angle) {
	mat4 result = mat4(
	1, 0, 0, 0,
	0, cos(angle), -sin(angle), 0,
	0, sin(angle), cos(angle), 0,
	0, 0, 0, 1);
	return result;
}

mat4 rotate_y(float angle) {
	mat4 result = mat4(
	cos(angle), 0, sin(angle), 0,
	0, 1, 0, 0,
	-sin(angle), 0, cos(angle), 0,
	0, 0, 0, 1);
	return result;
}

mat4 rotate_z(float angle) {
	mat4 result = mat4(
		cos(angle), -sin(angle), 0, 0,
		sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	return result;
}

mat4 translate(vec3 pos) {
	mat4 result = mat4(
	1, 0, 0, pos.x,
	0, 1, 0, pos.y,
	0, 0, 1, pos.z,
	0, 0, 0, 1);
	return result;
}

vec3 modifyRotation(vec3 offset) {
	mat4 mod = rotate_x(rotation_woah[0].x) * rotate_y(rotation_woah[0].y) * rotate_z(rotation_woah[0].z);
	offset = (mod * vec4(offset, 1)).xyz;
	return offset;
}

void EmitPoint(vec3 offset, float boat_side, float boat_side_refract)
{
	vec3 pos = offset + vpos;
	gl_Position = Projection * View * vec4(pos, 1.0);
	wp = (Model * vec4(pos, 1.0)).xyz;
	is_normal = 0;
	tc = vec2(pos.x / pozl, pos.y/ (pozh + 1));
	bs = boat_side;
	bsr = boat_side_refract;
	EmitVertex();
}

vec3 calculateNormal(vec3 v1, vec3 v2, vec3 v3) {
	vec3 result;
	vec3 U = v1 - v2;
	vec3 V = v1 - v3;
	result = normalize(cross(U, V));
	result.x = result.x;
	return result;
}

void showNormal(vec3 center, vec3 normal) {
	float normal_length = 1;
	float arrow_width = 0.1;
	vec3 deva = vec3(0, arrow_width / 2, 0);
	vec3 devb = vec3(0, -arrow_width / 2, 0);
	{
		vec3 pos = center;
		gl_Position = MVP_Matrix[0] * vec4(pos, 1.0);
		is_normal = 1;
		is_normal_color = vec3(1, 0, 0);
		EmitVertex();
	}
	{
		vec3 pos = center + normal * normal_length + deva;
		gl_Position = MVP_Matrix[0] * vec4(pos, 1.0);
		is_normal = 1;
		is_normal_color = vec3(0, 0, 1);
		EmitVertex();
	}
	{
		vec3 pos = center + normal * normal_length + devb;
		gl_Position = MVP_Matrix[0] * vec4(pos, 1.0);
		is_normal = 1;
		is_normal_color = vec3(0, 0, 1);
		EmitVertex();
	}
	EndPrimitive();
}

void main()
{
	float ipotenuza = latura * sqrt(2);
	float mediana_principala = sqrt(latura * latura - ipotenuza * ipotenuza / 4);
	vec3 normal;

	vec3 A = vec3(0, 2 * mediana_principala / 3, grosime);
	vec3 B = vec3(ipotenuza / 2, - mediana_principala / 3, grosime);
	vec3 C = vec3(-ipotenuza / 2, - mediana_principala / 3, grosime);
	vec3 F = vec3(0, 2 * mediana_principala / 3, -grosime);
	vec3 D = vec3(ipotenuza / 2, - mediana_principala / 3, -grosime);
	vec3 E = vec3(-ipotenuza / 2, - mediana_principala / 3, -grosime);

	A = modifyRotation(A);
	B = modifyRotation(B);
	C = modifyRotation(C);
	D = modifyRotation(D);
	E = modifyRotation(E);
	F = modifyRotation(F);

	float resolutionx = 1;
	float resolutiony = 1;

	int set_triangle = 0;

	normal = calculateNormal(E, D, C);
	EmitPoint(E, 0, 0);

	EmitPoint(D, 0, 0);

	EmitPoint(C, 0, 1);

	normal = calculateNormal(D, C, B);
	EmitPoint(B, 0, 1);

	normal = calculateNormal(C, B, A);
	EmitPoint(A, 0, 1);

	normal = calculateNormal(B, A, D);
	EmitPoint(D, 1, 0);

	normal = calculateNormal(A, D, F);
	EmitPoint(F, 1, 0);

	normal = calculateNormal(D, F, E);
	EmitPoint(E, 1, 0);

	normal = calculateNormal(F, E, A);
	EmitPoint(A, 0, 0);

	normal = calculateNormal(E, A, C);
	EmitPoint(C, 0, 0);

	EndPrimitive();

	vec3 normalRepresentation = calculateNormal(A, B, C);
	vec3 center = (A + B + C) / 3.0f + vpos;
	if (togle_normal == 1) {
		showNormal(center, normalRepresentation);
	}
}
