#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitTangent;
layout (location = 4) in vec3 aColor0;
layout (location = 12) in vec3 aTexCoord0;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model[1];
layout(std140) uniform uboViewProjection
{
	mat4 view;
	mat4 projection;
};

void main()
{
    vec4 FragPos4 = model[gl_InstanceID] * vec4(aPosition, 1.0)

    FragPos = vec3(FragPos4);
    Normal = mat3(transpose(inverse(model))) * aNormal;  
	TexCoord = vec2(aTexCoord0.x, aTexCoord0.y);
    
    gl_Position = projection * view * FragPos4;
    
}

