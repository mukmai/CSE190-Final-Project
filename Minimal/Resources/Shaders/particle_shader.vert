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
    vec3 position = vec3(pos.x, pos.y, pos.z);

	vec3 camRight = vec3(camMat[0][0], camMat[1][0], camMat[2][0]);
	vec3 camUp    = vec3(camMat[0][1], camMat[1][1], camMat[2][1]);

	vec3 finalPosition = position + (camRight * vert.x * pos.w) + (camUp * vert.y * pos.w);

    gl_Position = projMat * camMat * vec4(finalPosition, 1.0);
	
	fragColor = color;
}
