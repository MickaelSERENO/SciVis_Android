#version 300 es
precision mediump float;

uniform vec4 uMaskColor;
uniform bool uUseTexture;
uniform sampler2D uTexture0;

in vec2 varyUV;
in vec4 varyColor;

out vec4 fragColor;

void main()
{
	if(uUseTexture)
	{
		vec4 textColor = texture2D(uTexture0, varyUV);

		if(uMaskColor[3] == 0.0 && textColor[3] == 0.0)
			discard;
		else if(uMaskColor == textColor)
			discard;
        fragColor = textColor;
	}
    else
    {
        if(varyColor.a == 0.0)
            discard;
        fragColor = varyColor;
    }
}
