package com.sereno.math;

public class Vector3
{
    public static float[] minus(float[] vecA, float[] vecB)
    {
        float[] res = new float[3];
        for(int i = 0; i < 3; i++)
            res[i] = vecA[i] - vecB[i];
        return res;
    }

    public static float[] normalise(float[] vec)
    {
        float norm = 0.0f;
        for(int i = 0; i < 3; i++)
            norm += vec[i]*vec[i];
        norm = (float)Math.sqrt(norm);

        float[] res = new float[3];
        for(int i = 0; i < 3; i++)
            res[i] = vec[i]/norm;
        return res;
    }

    public static float dotProduct(float[] vecA, float[] vecB)
    {
        float res = 0;
        for(int i = 0; i < 3; i++)
            res += vecA[i] * vecB[i];
        return res;
    }

    public static float[] crossProduct(float[] vecA, float[] vecB)
    {
        float[] res = new float[]
                {
                        vecA[1]*vecB[2] - vecA[2]*vecB[1],
                        vecA[2]*vecB[0] - vecA[0]*vecB[2],
                        vecA[0]*vecB[1] - vecA[1]*vecB[0]
                };

        return res;
    }
}
