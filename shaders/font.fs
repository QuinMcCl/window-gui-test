#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;

uniform vec4 TextColor;

const float smoothing = 1.0/16.0;

float median(float x, float y, float z);

void main()
{    

    vec4 outsideColor = vec4(0.0, 0.0, 0.0, 0.0);
    // vec4 outsideColor = vec4(1.0, 1.0, 1.0, 1.0);
    // vec4 TextColor =    vec4(1.0, 1.0, 1.0, 1.0);


    // Bilinear sampling of the distance field
    vec3 s = texture2D(texture1, TexCoord).rgb;
    // Acquire the signed distance
    float d = median(s.r, s.g, s.b) - 0.5;
    // Weight between inside and outside (anti-aliasing)
    float w = clamp(d/fwidth(d) + 0.5, 0.0, 1.0);
    // Combining the background and foreground color
    if(w < 0.5)
        discard;
    FragColor = mix(outsideColor, TextColor, w);

    // FragColor = vec4(texture2D(texture1, TexCoord).rgb, 1.0);
}

float median(float x, float y, float z){
    return max(min(x,y),min(max(x,y),z));
}