#version 300 es

precision highp float;

in vec3 varyUV;

uniform sampler3D uTexture0;
uniform sampler2D uTexture1;

out vec4 fragColor;

void main()
{
    vec2 v    = texture(uTexture0, varyUV).rg;
    fragColor = texture(uTexture1, vec2(v.x, min(1.0, v.y*3.0)));
    fragColor.a = 1.0;
}
