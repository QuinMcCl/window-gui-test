#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitTangent;
layout (location = 4) in vec3 aColor0;
layout (location = 5) in vec3 aColor1;
layout (location = 6) in vec3 aColor2;
layout (location = 7) in vec3 aColor3;
layout (location = 8) in vec3 aColor4;
layout (location = 9) in vec3 aColor5;
layout (location = 10) in vec3 aColor6;
layout (location = 11) in vec3 aColor7;
layout (location = 12) in vec3 aTexCoord0;
layout (location = 13) in vec3 aTexCoord1;
layout (location = 14) in vec3 aTexCoord2;
layout (location = 15) in vec3 aTexCoord3;
layout (location = 16) in vec3 aTexCoord4;
layout (location = 17) in vec3 aTexCoord5;
layout (location = 18) in vec3 aTexCoord6;
layout (location = 19) in vec3 aTexCoord7;

out vec2 TexCoord;

uniform mat4 model[1];
layout(std140) uniform uboViewProjection
{
	mat4 view;
	mat4 projection;
};

void main()
{
	gl_Position = projection * view * model[gl_InstanceID] * vec4(aPosition, 1.0f);
	TexCoord = vec2(aTexCoord0.x, aTexCoord0.y);
}

