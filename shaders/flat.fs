#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

//structs
struct Material_s
{
    sampler2D Diffuse[8];
};

// texture samplers
uniform Material_s Material;

void main()
{
	FragColor = texture(Material.Diffuse[0], TexCoord);
}

