#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;


struct letterStruct
{
	vec2 charSize;
	vec2 charBearing;
	vec2 textSize;
	vec2 textBearing;
};

struct charStruct
{
	int index;
	int pad1;
	int pad2;
	int pad3;
	vec3 Offset;
	int pad4;
};

layout(std140) uniform uboAlphabetBlock
{
	letterStruct letters[96];
}Alphabet;

layout(std140) uniform uboTextBlock
{
	charStruct chars[256];
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// uniform int index;
// uniform vec3 Offset;

void main()
{
	int CharIndex = chars[gl_InstanceID].index;
	vec3 CharOffset = chars[gl_InstanceID].Offset;

	vec2 bPos = aPos;
	bPos *= Alphabet.letters[CharIndex].charSize;
	bPos += Alphabet.letters[CharIndex].charBearing;

	vec2 bTexCoord = aTexCoord;
	bTexCoord *= Alphabet.letters[CharIndex].textSize;
	bTexCoord += Alphabet.letters[CharIndex].textBearing;

	gl_Position = projection * view * model * vec4((vec3(bPos,0.0) + CharOffset), 1.0f);
	TexCoord = bTexCoord;
}

