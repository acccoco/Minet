#version 330 core
out vec4 FragColor;

in vec3 TexVec;

uniform samplerCube texture_sky;

void main()
{
    vec3 color = texture(texture_sky, TexVec).rgb;

#ifdef HDR_INPUT
    // hdr tonemap
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));
#endif

    FragColor = vec4(color, 1.0);
}
