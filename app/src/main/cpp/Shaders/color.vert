precision mediump float;

attribute vec3 vPosition;
attribute vec4 vColor;
attribute vec2 vTextureCoord;

uniform mat4 uMVP;

varying vec4 varyColor;
varying vec2 varyTextureCoord;
varying vec4 varyModelPosition;
varying vec4 varyPosition;

void main()
{
	gl_Position       = /*uMVP * */vec4(vPosition, 1.0);
	varyModelPosition = vec4(vPosition, 1.0);
	varyPosition      = gl_Position;
	varyColor         = vColor;
	varyTextureCoord  = vTextureCoord;
}
