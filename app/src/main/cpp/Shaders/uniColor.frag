#version 300 es
precision mediump float;

uniform vec4 uMaskColor;
uniform bool uUseTexture;

uniform sampler2D uTexture;
uniform vec4      uUniColor;

in vec2 varyTextureCoord;
out vec4 fragColor;

void main()
{
	if(uUseTexture)
	{
		vec4 textColor = texture2D(uTexture, varyTextureCoord);

		if(uMaskColor[3] == 0.0 && textColor[3] == 0.0)
			discard;
		else if(uMaskColor == textColor)
			discard;
        fragColor = textColor;
	}

    else
    {
        if(uUniColor.a == 0.0)
            discard;
        fragColor = uUniColor;
    }
}
