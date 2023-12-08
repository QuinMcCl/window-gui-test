#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

struct atlasChar {
   vec2 posScale;
   vec2 posOffset;
   vec2 texScale;
   vec2 texOffset;
}

uniform alphabet{
    atlasChar[95];
};


uniform int charIndex[100];
uniform vec2 charOffsets[100];


// uniform mat4 model;
// uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec2 newPos = (alphabet.atlasChar[charIndex[gl_InstanceID]].posScale * aPos) + alphabet.atlasChar[charIndex[gl_InstanceID]].posOffset + charOffsets[gl_InstanceID];
    gl_Position = projection * vec4(newPos, 0.0, 1.0);
	TexCoord = (alphabet.atlasChar[charIndex[gl_InstanceID]].texScale * aTexCoord) + alphabet.atlasChar[charIndex[gl_InstanceID]].texOffset;
} 