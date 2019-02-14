#version 300 es
#extension GL_OES_geometry_shader : enable

precision lowp float;

layout(points) in;
layout(triangle_strip, max_vertices = 14) out;

uniform mat4 uMVP;
uniform vec3 uSpacing;

out vec4 outGeomColor;

in VS_OUT
{
    vec4 outVSColor;
}gsIn[];

void main()
{

    /*----------------------------------------------------------------------------*/
    /*--------------------------Determine cube position---------------------------*/
    /*----------------------------------------------------------------------------*/
    outGeomColor = gsIn[0].outVSColor;
    vec4 cubePos[8];

    vec3 spacing = uSpacing;

    //Back
    cubePos[0] = gl_in[0].gl_Position + vec4(0,         0,         0, 0.0);
    cubePos[1] = gl_in[0].gl_Position + vec4(spacing.x, 0,         0, 0.0);
    cubePos[2] = gl_in[0].gl_Position + vec4(0,         spacing.y, 0, 0.0);
    cubePos[3] = gl_in[0].gl_Position + vec4(spacing.x, spacing.y, 0, 0.0);

    //Front
    cubePos[4] = gl_in[0].gl_Position + vec4(0,         0,         spacing.z, 0.0);
    cubePos[5] = gl_in[0].gl_Position + vec4(spacing.x, 0,         spacing.z, 0.0);
    cubePos[6] = gl_in[0].gl_Position + vec4(0,         spacing.y, spacing.z, 0.0);
    cubePos[7] = gl_in[0].gl_Position + vec4(spacing.x, spacing.y, spacing.z, 0.0);

    for(int i = 0; i < 8; i++)
        cubePos[i] = uMVP*cubePos[i];

    int ind[14];
    ind[0]  = 6; //FTL
    ind[1]  = 7; //FTR
    ind[2]  = 4; //FBL
    ind[3]  = 5; //FBR
    ind[4]  = 1; //BBR
    ind[5]  = 7; //FTR
    ind[6]  = 3; //BTR
    ind[7]  = 6; //FTL
    ind[8]  = 2; //BTL
    ind[9]  = 4; //FBL
    ind[10] = 0; //BBL
    ind[11] = 1; //BBR
    ind[12] = 2; //BTL
    ind[13] = 3; //BTR

    for(int i = 0; i < 14; i++)
    {
        gl_Position = cubePos[ind[i]];
        EmitVertex();
    }
    EndPrimitive();
}
