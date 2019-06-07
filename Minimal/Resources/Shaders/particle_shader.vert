#version 410 core
// PARTICLE SHADER : VERTEX 
layout (location = 0) in vec3 vert;
layout (location = 1) in vec4 pos;
layout (location = 2) in vec4 color;

uniform mat4 projMat;
uniform mat4 camMat;

out vec4 fragColor;

void main()
{
    // The vector "vert" contains the vertex data of the particle, but
	// it has to be offset by the vector "pos" to get the individual
	// particle's world position
    vec3 position = (pos.w * vert) + vec3(pos.x, pos.y, pos.z);
	//vec3 position = pos.w (vert + vec3(pos.x, pos.y, pos.z));
    gl_Position = projMat * camMat * vec4(position, 1.0);
	
	fragColor = color;
}
