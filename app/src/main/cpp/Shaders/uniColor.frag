precision mediump float;

uniform vec4 uMaskColor;
uniform bool uUseTexture;

uniform sampler2D uTexture;
uniform vec4      uUniColor;

varying vec2 varyTextureCoord;

void main()
{
	if(uUseTexture)
	{
		vec4 textColor = texture2D(uTexture, varyTextureCoord);

		if(uMaskColor[3] == 0.0 && textColor[3] == 0.0)
			discard;
		else if(uMaskColor == textColor)
			discard;
        gl_FragColor = textColor;
	}

    else
    {
        if(uUniColor.a == 0.0)
            discard;
        gl_FragColor = uUniColor;
    }
}
