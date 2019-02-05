#version 300 es
precision mediump float;

in vec4 outGeomColor;

out vec4 fragColor;

void main()
{
    fragColor = outGeomColor;
}
