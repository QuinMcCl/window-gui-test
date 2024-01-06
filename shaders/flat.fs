#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform Material
{
    sampler2D Diffuse[8];
};

void main()
{
	FragColor = texture(Material.Diffuse[0], TexCoord);
}

