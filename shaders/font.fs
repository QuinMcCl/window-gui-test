#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sdfTexture;
uniform vec3 textColor;

const float smoothing = 1.0/16.0;
void main()
{    



    // // Bilinear sampling of the distance field
    // vec3 s = texture2D(sdfTexture, TexCoord).rgb;
    // // Acquire the signed distance
    // float d = median(s.r, s.g, s.b) - 0.5;
    // // Weight between inside and outside (anti-aliasing)
    // float w = clamp(d/fwidth(d) + 0.5, 0.0, 1.0);
    // // Combining the background and foreground color
    // // gl_FragColor = mix(outsideColor, insideColor, w);

    // float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, texture(sdfTexture, TexCoord).r);
    float alpha = texture(sdfTexture, TexCoord).r;
    if (alpha < 0.5)
        discard;
    FragColor = vec4(textColor, 1.0);
}