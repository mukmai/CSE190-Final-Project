#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 in_uv;

out vec2 frag_uv;
out vec3 frag_normal;
out vec3 frag_pos;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main()
{
	frag_normal = mat3(transpose(inverse(model))) * vec3(0, 0, 1.0f);
	gl_Position = projection * view * model * vec4(position, 1.0f);
	frag_uv = in_uv;
	frag_pos = vec3(model * vec4(position, 1.0f));
}  