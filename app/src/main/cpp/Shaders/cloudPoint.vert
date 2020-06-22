#version 320 es
precision mediump float;

in vec3 vPosition;
in vec4 vColor;

out vec4 v2gColor;

void main()
{
	v2gColor    = vec4(vColor.rgb, 1.0);
	gl_Position = vec4(vPosition, 1.0);
}
