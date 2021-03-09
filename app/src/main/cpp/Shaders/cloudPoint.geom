#version 320 es
precision mediump float;

layout(points) in;
layout(triangle_strip, max_vertices=24) out;

in  vec4 v2gColor[];

uniform float uPointSize;
uniform mat4  uMVP;

out vec4 varyColor;

void main() 
{    
    float f = uPointSize / 2.0; //half size
    vec4 vc[8] = vec4[8](
                   uMVP*vec4(-f, -f, -f, 0.0f),  //0
                   uMVP*vec4(-f, -f, +f, 0.0f),  //1
                   uMVP*vec4(-f, +f, -f, 0.0f),  //2
                   uMVP*vec4(-f, +f, +f, 0.0f),  //3
                   uMVP*vec4(+f, -f, -f, 0.0f),  //4
                   uMVP*vec4(+f, -f, +f, 0.0f),  //5
                   uMVP*vec4(+f, +f, -f, 0.0f),  //6
                   uMVP*vec4(+f, +f, +f, 0.0f)); //7

    varyColor = v2gColor[0];

    //Face 1: left
    gl_Position = (gl_in[0].gl_Position + vc[0]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[1]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[2]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[3]);
    EmitVertex();
    EndPrimitive();

    //Face 2: front
    gl_Position = (gl_in[0].gl_Position + vc[0]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[2]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[4]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[6]);
    EmitVertex();
    EndPrimitive();

    //Face 3: right
    gl_Position = (gl_in[0].gl_Position + vc[4]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[6]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[5]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[7]);
    EmitVertex();
    EndPrimitive();

    //Face 4: back
    gl_Position = (gl_in[0].gl_Position + vc[7]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[3]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[5]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[1]);
    EmitVertex();
    EndPrimitive();

    //Face 5: top
    gl_Position = (gl_in[0].gl_Position + vc[2]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[3]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[6]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[7]);
    EmitVertex();
    EndPrimitive();

    //Face 6: bottom
    gl_Position = (gl_in[0].gl_Position + vc[0]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[4]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[1]);
    EmitVertex();
    gl_Position = (gl_in[0].gl_Position + vc[5]);
    EmitVertex();
    EndPrimitive();
}    
