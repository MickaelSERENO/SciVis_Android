#version 300 es
precision mediump float;

in vec3 vPosition;
in vec4 vColor;


out VS_OUT
{
    vec4 outVSColor;
}vsOut;

void main()
{
	gl_Position = vec4(vPosition, 1.0);
    vsOut.outVSColor = vColor;
}
