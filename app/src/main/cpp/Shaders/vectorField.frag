#version 300 es

precision lowp float;

in float varyUV;

uniform sampler2D uTexture0;

out vec4 fragColor;

void main()
{
    fragColor = texture2D(uTexture0, vec2(varyUV, 0));
}
