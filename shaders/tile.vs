#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 TexColor;

void main()
{
    gl_Position = vec4(aPosition, 1.0); 
    TexColor = aColor;
    TexCoord = aTexCoord;
}