#version 330 core
out vec4 FragColor;

in vec3 TexVec;

uniform samplerCube texture_sky;

void main()
{
    vec3 color = texture(texture_sky, TexVec).rgb;
    // hdr tonemap
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
