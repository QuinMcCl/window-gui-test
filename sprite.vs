#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 m_BoneIDs;
layout (location = 6) in vec4 m_Weights;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


out vec2 TexCoords;


void main()
{

    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
}

