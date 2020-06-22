#version 320 es
layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

in  vec4 v2gColor[];

uniform float uPointSize;
uniform mat4  uMVP;

out vec4 varyColor;

void main() 
{    
    float f = uPointSize / 2.0; //half size
    vec4 vc[8] = vec4[8](
                   vec4(-f, -f, -f, 0.0f),  //0
                   vec4(-f, -f, +f, 0.0f),  //1
                   vec4(-f, +f, -f, 0.0f),  //2
                   vec4(-f, +f, +f, 0.0f),  //3
                   vec4(+f, -f, -f, 0.0f),  //4
                   vec4(+f, -f, +f, 0.0f),  //5
                   vec4(+f, +f, -f, 0.0f),  //6
                   vec4(+f, +f, +f, 0.0f)); //7

    const int VERT_ORDER[24] = int[24](0,1,2,3,  // left
                                       0,2,4,6,  // front  
                                       4,6,5,7,  // right
                                       7,3,5,1,  // back
                                       2,3,6,7,  // top
                                       0,4,1,5); // bottom

    varyColor = v2gColor[0];

    // Build the CUBE tile by submitting triangle strip vertices
    for(int j = 0; j < 6; j++)
    {
        for(int k = 0; k < 4; k++) 
        {
            gl_Position = uMVP*(gl_in[0].gl_Position + vc[VERT_ORDER[4*j+k]]);
            EmitVertex();
        }
        EndPrimitive();
    }
}    
