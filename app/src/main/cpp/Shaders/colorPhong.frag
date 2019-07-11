#version 300 es

precision mediump float;
precision mediump sampler2D;

uniform vec3 uColor;
uniform vec3 uLightColor;
uniform vec4 uPhongCoeffs; //[ambient, diffuse, specular, shininess]  = [ka, kd, ks, alpha]

in vec3 varyNormal;
in vec4 varyPosition;
in vec3 varyLightDir;

out vec4 fragColor;

void main()
{
    vec3 ambient  = uPhongCoeffs.x * uColor * uLightColor;
    vec3 diffuse  = uPhongCoeffs.y * max(0.0, dot(varyNormal, -varyLightDir)) * uColor * uLightColor;
    vec3 specular = uPhongCoeffs.z * pow(max(0.0, dot(reflect(-varyLightDir, varyNormal), normalize(vec3(0, 0, -1) - varyPosition.xyz))), uPhongCoeffs.w) * uLightColor;

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}

