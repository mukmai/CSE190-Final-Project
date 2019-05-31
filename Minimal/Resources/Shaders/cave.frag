#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec2 frag_uv;
in vec3 frag_normal;
in vec3 frag_pos;

uniform sampler2D renderedTexture;
uniform bool fallOffLCD;
uniform vec3 eyePos;
uniform bool failed;

out vec4 fragColor;

void main()
{    
	vec3 color = texture(renderedTexture, frag_uv).rgb;
	
	if (fallOffLCD) {
		float brightness = max(dot(normalize(frag_normal), normalize(eyePos - frag_pos)), 0);
		color *= brightness;
	}
	fragColor = vec4(color, 1.0);

	if (failed)
		fragColor = vec4(0, 0, 0, 1.0);
}
