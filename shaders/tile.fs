#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 TexColor;

uniform sampler2D MAP_TEXTURE;

void main()
{
    // FragColor = vec4(TexColor, 1.0);
    FragColor = texture(MAP_TEXTURE, TexCoord);
}

