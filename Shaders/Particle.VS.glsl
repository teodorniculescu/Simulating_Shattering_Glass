#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

layout(location = 0) out vec3 rotation_woah;
layout(location = 1) out vec3 world_position;
layout(location = 2) out vec3 world_normal;
layout(location = 3) out vec2 texture_coord;
layout(location = 4) out mat4 MVP_Matrix;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float reset;
uniform float speed_modifier;

uniform int colision;

struct Particle
{
	vec4 position;
	vec4 speed;
	vec4 rotation;
	vec4 iposition;
	vec4 ispeed;
	vec4 irotation;
	vec4 lcm;
};

layout(std430, binding = 0) buffer particles {
	Particle data[];
};

float rand(vec2 co)
{
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float sgn(float num) {
	if (num > 0)
		return 1;
	if (num < 0)
		return -1;
	return 0;
}

void main()
{
	vec3 pos = data[gl_VertexID].position.xyz;
	vec3 spd = data[gl_VertexID].speed.xyz;
	vec3 rot = data[gl_VertexID].rotation.xyz;
	vec3 lcm = data[gl_VertexID].lcm.xyz;

	float dt = 0.1;
	vec3 grav, impact, added_rot;
	float ix, iy, iz;
	
	// grav m / s^2
	// impact m / s viteza instantanee

	if (colision == 1) {
		ix = lcm.x;
		iy = lcm.y;
		iz = lcm.z;
		grav = vec3(0, -0.98 / 4, 0) * speed_modifier;
		impact = vec3(iy, ix, iz) * speed_modifier;
		added_rot = vec3(rand(pos.xy), rand(spd.xy), rand(rot.xy)) / 30 * speed_modifier;
	} else {
		grav = vec3(0);
		impact = vec3(0);
		added_rot = vec3(0);
	}
	pos = pos + (spd * dt + grav * dt * dt / 2.0f) * speed_modifier;
	spd = spd + (grav * dt + impact) * speed_modifier;
	rot = rot + (added_rot) * speed_modifier;

	// daca au trecut de o limita, valorile sunt resetate

	// valorile noi ale vitezei, positiei si rotatie sunt updatate
	data[gl_VertexID].position.xyz =  pos;
	data[gl_VertexID].speed.xyz =  spd;
	data[gl_VertexID].rotation.xyz =  rot;

	if (reset == 1) {
		data[gl_VertexID].position.xyz = data[gl_VertexID].iposition.xyz;
		data[gl_VertexID].speed.xyz = data[gl_VertexID].ispeed.xyz;
		data[gl_VertexID].rotation.xyz = data[gl_VertexID].irotation.xyz;
	}


	pos = data[gl_VertexID].position.xyz;
	rot = data[gl_VertexID].rotation.xyz;
	rotation_woah = rot.xyz;

    world_position = (Model * vec4(pos, 1)).xyz;
    world_normal = normalize(mat3(Model) * v_normal);

	MVP_Matrix = Projection * View;
	gl_Position = Model * vec4(pos, 1);

	texture_coord = v_texture_coord;
}


