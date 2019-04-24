#version 300 es

precision lowp float;
precision mediump sampler2D;

in float varyUV;

uniform sampler2D uTexture0;

out vec4 fragColor;

void main()
{
    fragColor = texture(uTexture0, vec2(varyUV, 0));
}
