#version 300 es
precision mediump float;

uniform sampler2D uTexture0;
in vec2 varyUV;
out vec4 fragColor;

void main()
{
    vec4 textColor = texture2D(uTexture0, varyUV);
    fragColor      = textColor;
}

