#version 300 es

precision mediump sampler2D;
precision mediump float;

uniform vec4 uMaskColor;
uniform bool uUseTexture;

uniform sampler2D uTexture;
uniform vec4      uUniColor;

in vec2 varyUV;
out vec4 fragColor;

void main()
{
	if(uUseTexture)
	{
		vec4 textColor = texture(uTexture, varyUV);

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
