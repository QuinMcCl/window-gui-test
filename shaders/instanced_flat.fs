#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

//structs
struct Material_s
{
    sampler2D Diffuse[8];
    sampler2D Specular[8];
    sampler2D Emissive[8];
    sampler2D Opacity[8];
};

uniform Material_s Material;

layout(std140) uniform uboViewPosition
{
	vec3 viewPos;
};

void main()
{
	FragColor = texture(Material.Diffuse[0], TexCoord);
}

