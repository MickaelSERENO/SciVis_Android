precision mediump float;

attribute vec3 vPosition;
attribute vec2 vTextureCoord;

varying vec2 varyTextureCoord;
uniform mat4 uMVP;

void main()
{
	varyTextureCoord  = vTextureCoord;
	gl_Position       = uMVP*vec4(vPosition, 1.0);
}
