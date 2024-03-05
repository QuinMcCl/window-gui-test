#version 330 core
layout (location = 0) in vec3 aPosition; // LAT LON [0 - 1]

layout(std140) uniform uboViewProjection
{
	mat4 view;
	mat4 projection;
};


out VS_OUT {
    vec3 TexCoord;
} vs_out;

void main()
{
    gl_Position = projection * view * vec4(aPosition, 1.0); 
}