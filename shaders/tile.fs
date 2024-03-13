#version 330 core
#define GLM_E         2.71828182845904523536028747135266250   /* e           */
#define GLM_PI        3.14159265358979323846264338327950288   /* pi          */
#define GLM_PI_2      1.57079632679489661923132169163975144   /* pi/2        */
#define GLM_PI_4      0.785398163397448309615660845819875721  /* pi/4        */

out vec4 FragColor;

in vec2 TexCoord;
in vec3 TexColor;

struct projection_params
{
    int type;
    float p1;
    float p2;
    float p3;
    float p4;
    float p5;
    float p6;
    float p7;
};

uniform sampler2D MAP_TEXTURE;

layout(std140) uniform uboSourceEllipsoid {vec2 source_Ellipsoid;};
layout(std140) uniform uboTargetEllipsoid {vec2 target_Ellipsoid;};
layout(std140) uniform uboSourceProjection {projection_params source_projection;};
layout(std140) uniform uboTargetProjection {projection_params target_projection;};

vec2 web_merc_forward(vec2 E, projection_params P, vec2 I){
    return vec2(
        P.p4 + (E.x * (I.x - P.p2)),
        P.p5 + (E.x * log(tan(GLM_PI_4 + I.y / 2.0f)))
    );
}
vec2 web_merc_reverse(vec2 E, projection_params P, vec2 I){
    return vec2(
        ((I.x - P.p4) / E.x) + P.p2, 
        GLM_PI_2 - (2.0f * atan(pow(GLM_E, (P.p5 - I.y) / E.x)))
    );
}



void main()
{
    vec2 worldCoord = vec2(0.0, 0.0);
    vec2 WindowCoord = TexCoord;
    switch (target_projection.type) {
case 0:
    worldCoord = TexCoord;
    break;
case 1:
    worldCoord = web_merc_reverse(target_Ellipsoid, target_projection, WindowCoord);
    break;

}

    vec2 MapCoord = vec2(0.0, 0.0);
    switch (source_projection.type) {
case 0:
    MapCoord = worldCoord;
    break;
case 1:
    MapCoord = web_merc_forward(source_Ellipsoid, source_projection, worldCoord);
    break;
}

    // FragColor = vec4(TexColor, 1.0);
    FragColor = texture(MAP_TEXTURE, MapCoord);
}

