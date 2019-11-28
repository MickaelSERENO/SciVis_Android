#version 300 es

precision mediump float;
precision mediump sampler2D;

uniform sampler2D uTexture0;

in vec2 varyUV;
out vec4 fragColor;

void main()
{
    vec4 textColor = texture(uTexture0, varyUV);
    fragColor      = vec4(vec3(1.0)-textColor.rrr, 1.0); //Inverse scale
}
