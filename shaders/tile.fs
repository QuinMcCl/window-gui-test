#version 330 core
out vec4 FragColor;

#define M_PI   3.1415926535897932384626433832795
#define M_PI_2 1.5707963267948966192313216916398
#define M_E    2.7182818284590452353602874713527

in VS_OUT {
    vec3 TexCoord;
} vs_out;

// texture samplers
uniform sampler3D MapCache;

uniform struct map_params
{
    vec3 center;
    float ac;
    float gc;
    vec2 false;
};


vec3 SP_to_LL_merc(vec3 SP)
{
    B = sqrt(1 + pow( M_E * pow( cos(center.x) , 2.) , 2.) / (1 - pow(M_E,2.0)));
    A = ac * B * center.z * sqrt(1 - pow(M_E, 2.)) / (1 - pow(M_E * sin(center.x), 2.));
    T0 = tan((M_PI / 4.) - (center.x / 2.)) / pow( (1 - M_E * sin(center.x)) / (1 + M_E * sin(center.x)) , M_E / 2);
    D = B * sqrt(1 - pow(M_E, 2.)) / (cos(center.x) * sqrt(1 - pow(M_E * sin(center.x), 2.)));




    float t = tan((M_PI / 4.) - (center.x) / 2.) / pow((1. - (M_E * sin(center.x)) ) / (1. + (M_E * sin(center.x)) ), M_E / 2.);

Q = H / tB

S = (Q - 1/Q) / 2

T = (Q + 1/Q) / 2

V = sin(B (l - l o))

U = (- V cos(g o) + S sin(g o)) / T

v = A ln[(1 - U) / (1 + U)] / 2 B

}

vec3 LL_to_merc(vec3 LL)
{
    float scale = (pow(2.0, LL.z) / (2 * M_PI))
    float x = scale * (M_PI + LL.x);
    float y = scale * (M_PI - log(tan(0.5 * (M_PI_2 + LL.y))));
    float z = LL.z;

    vec3 merc = vec3(x, y, z);
    return merc;
}

vec4 texture_cache(sampler3D cache, vec3 coord)
{
    vec3 uvw = mod(coord, cache_shape) * tile_shape;
    return texture(cache, uvw);
}


void main()
{
	FragColor = texture_cache(MapCache, LL_to_merc(vs_out.TexCoord));
}

