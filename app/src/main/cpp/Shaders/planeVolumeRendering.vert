#version 300 es

precision highp float;

uniform mat4 uMVP;

in vec3 vPosition;

out vec3 varyUVW;

void main()
{
    gl_Position = uMVP*vec4(vPosition, 1.0);
    varyUVW     = vPosition + vec3(0.5 , 0.5, 0.5);
}
