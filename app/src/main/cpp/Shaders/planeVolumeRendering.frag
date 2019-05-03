#version 300 es

precision highp float;
precision highp sampler2D;
precision highp sampler3D;

uniform sampler3D uTexture0;
uniform sampler2D uTexture1;

in  vec3 varyUVW;
out vec4 fragColor;

void main()
{
//    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
//    return;
    vec2 tfCoord = texture(uTexture0, varyUVW).rg;
    fragColor    = texture(uTexture1, tfCoord);
}
