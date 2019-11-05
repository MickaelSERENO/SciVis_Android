/*
 * This fragment shader is based on:
 * J. Heinrich and D. Weiskopf, "Continuous Parallel Coordinates," in IEEE Transactions on Visualization and Computer Graphics, vol. 15, no. 6, pp. 1531-1538, Nov.-Dec. 2009. doi: 10.1109/TVCG.2009.131

 * We look where is the current drawing (varyUV), determine the line in the density texture and gather points through this density texture (called 2D histograms)
 */

#version 300 es
precision mediump float;
precision mediump sampler2D;

in  vec2 varyUV;

uniform int uNBSamples;
uniform sampler2D uTexture0;

out vec4 fragColor; 

void main()
{
    float eta1 = varyUV.x;
    float eta2 = varyUV.y;

    //Prevent division by zero
    if(eta1 <= 1.e-8)
    {
        fragColor = vec4(0,0,0,255);
        return;
    }

    // the slope and y-intercept of the dual line
    float m = (eta1 - 1.0) / eta1;
    float b = eta2 / eta1;

	// the intersections of the dual line with the x and y-axis
    float x0 = max(0.0, (1.0 - b) / m);
    float y0 = min(1.0, b);
    float x1 = min(-b / m, 1.0);
    float y1 = max(0.0, m * 1.0 + b);

	// the length of the relevant segment of the dual line
    vec2 p0 = vec2(x0, y0);
    vec2 p1 = vec2(x1, y1);
    vec2 d = p1 - p0;
    float ld = length(d);
    int realSamples = int(ceil(ld * float(uNBSamples)));

    if(realSamples == 0) 
    {
    	fragColor = vec4(0);
        return;
    }

    float step = 1.0 / float(realSamples);
    float t = 0.0;
    float sum = 0.0;

    //Gather density
    for(int i = 0; i < realSamples; i++) 
    {
        vec4 tmp = texture(uTexture0, p0 + t * d);
        sum += tmp.r;
        t   += step;
    }

    sum /= sqrt(pow(1.0 - eta1, 2.0) + pow(eta1, 2.0));
    fragColor = vec4(sum, sum, sum, 1.0);
}
