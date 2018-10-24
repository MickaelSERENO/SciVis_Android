precision mediump float;

attribute vec3 vPosition;
attribute vec4 vColor;
attribute vec2 vTextureCoord;
attribute vec3 vNormal;

uniform mat4 uMVP;

varying vec4 varyColor;
varying vec2 varyTextureCoord;

void main()
{
	varyColor         = vec4(vNormal/2.0 + vec3(0.5), 1.0);
	varyTextureCoord  = vTextureCoord;

	gl_Position       = uMVP*vec4(vPosition, 1.0);
}
