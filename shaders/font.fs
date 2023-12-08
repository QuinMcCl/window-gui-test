#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sdfTexture;
uniform vec3 textColor;

const float smoothing = 1.0/16.0;


float median(float x, float y, float z);

void main()
{    

    vec4 outsideColor = vec3(0.0, 0.0, 0.0);


    // Bilinear sampling of the distance field
    vec3 s = texture2D(sdfTexture, TexCoord).rgb;
    // Acquire the signed distance
    float d = median(s.r, s.g, s.b) - 0.5;
    // Weight between inside and outside (anti-aliasing)
    float w = clamp(d/fwidth(d) + 0.5, 0.0, 1.0);
    // Combining the background and foreground color
    FragColor = mix(outsideColor, textColor, w);

    // // float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, texture(sdfTexture, TexCoord).r);
    // float alpha = texture(sdfTexture, TexCoord).r;
    // if (alpha < 0.5)
    //     discard;
    // FragColor = vec4(textColor, 1.0);
}

float median(float x, float y, float z){
    float a = min(x,y);
    float b = max(x,y);
    float c = min(b,x);
    float d = max(a,b);
    return d;
}