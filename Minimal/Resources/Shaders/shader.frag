#version 410 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 vertNormal;
in vec2 frag_uv;
in vec3 vertPosition;

uniform int mode;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
float shininess;

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform vec3 viewPos;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
	vec3 norm = normalize(vertNormal);
	vec3 viewDir = normalize(viewPos - vertPosition);

	vec3 color;

	//color = texture(texture_diffuse0, frag_uv).rgb;
	color = CalcDirLight(dirLight, norm, viewDir);
    fragColor = vec4(color, 1.0);
}

//vec3 CalcDirLight(DirLight light, vec3 normal)
//{
//    vec3 lightDir = normalize(-light.direction);
//    // Diffuse shading
//    float diff = max(dot(normal, lightDir), 0.0);
//    // Combine results
//    vec3 ambient = light.ambient * vec3(0.3,0.3,0);
//    vec3 diffuse = light.diffuse * diff * vec3(1,1,0);
//    return (ambient + diffuse);
//}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse0, frag_uv));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse0, frag_uv));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular0, frag_uv));
    return (ambient + diffuse + specular);
}