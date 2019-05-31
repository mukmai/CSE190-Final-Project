#version 410 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 vertNormal;
in vec2 frag_uv;
in vec3 vertPosition;

uniform int mode;
uniform sampler2D texture_diffuse0;

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

vec3 CalcDirLight(DirLight light, vec3 normal);

void main()
{
	vec3 norm = normalize(vertNormal);

	vec3 color;
	if (mode == 0)
		color = CalcDirLight(dirLight, norm);
	if (mode == 1) // highlighted ball
		color = texture(texture_diffuse0, frag_uv).rgb;
	else if (mode == 2) // controller ball
		color = vec3(1,0,0);
    fragColor = vec4(color, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Combine results
    vec3 ambient = light.ambient * vec3(0.3,0.3,0);
    vec3 diffuse = light.diffuse * diff * vec3(1,1,0);
    return (ambient + diffuse);
}