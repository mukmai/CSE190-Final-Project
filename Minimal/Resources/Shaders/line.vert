#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
}  