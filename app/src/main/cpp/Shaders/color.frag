precision mediump float;

uniform vec4 uMaskColor;

uniform bool uUseTexture;
uniform bool uUseUniColor;

uniform sampler2D uTexture;
uniform vec4 uUniColor;

varying vec2 varyTextureCoord;
varying vec4 varyColor;
varying vec4 varyModelPosition;
varying vec4 varyPosition;

void main()
{
	if(uUseUniColor)
		gl_FragColor = uUniColor;
	else
		gl_FragColor = varyColor;

	if(uUseTexture)
	{
		vec4 textColor = texture2D(uTexture, varyTextureCoord);

		if(uMaskColor[3] == 0.0 && textColor[3] == 0.0)
			discard;
		else if(uMaskColor == textColor)
			discard;
	}

	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
