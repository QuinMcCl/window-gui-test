#version 330 core
layout (location =  0) in vec3 aPos;
// layout (location =  1) in vec3 aNormal;
// layout (location =  2) in vec3 aTangent;
// layout (location =  3) in vec3 aBitTangent;
// layout (location =  4) in vec4 aColor0;
layout (location = 12) in vec3 aTexCoord0;

out vec2 TexCoord;

uniform mat4 model;
layout(std140) uniform uboViewProjection
{
	mat4 view;
	mat4 projection;
};

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	TexCoord = vec2(aTexCoord0.x, aTexCoord0.y);
}

