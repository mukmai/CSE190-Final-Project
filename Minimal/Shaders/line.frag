#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

out vec4 fragColor;
uniform int eye;

void main()
{    
	if (eye == 0)
		fragColor = vec4(0.0, 1.0, 0.0, 1.0);
	else
		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
