#version 320 es

precision mediump float;

in vec4 varyColor;
out vec4 fragColor;

void main()
{
    fragColor = varyColor;
}
