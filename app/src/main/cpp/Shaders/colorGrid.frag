#version 300 es
precision lowp float;

in vec4 outGeomColor;

out vec4 fragColor;

void main()
{
    fragColor = outGeomColor;
}
