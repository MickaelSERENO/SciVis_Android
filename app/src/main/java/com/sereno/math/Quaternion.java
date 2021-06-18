package com.sereno.math;

/** Quaternion class*/
public class Quaternion
{
    /** The x (i) component*/
    public float x;

    /** The y (j) component*/
    public float y;

    /** The z (k) component*/
    public float z;

    /** The w (real) component*/
    public float w;

    /** Constructor, create a unit quaternion*/
    public Quaternion()
    {
        this(0.0f, 0.0f, 0.0f, 1.0f);
    }

    /** Constructor, create a custom quaternion
     * @param _x the i component
     * @param _y the j component
     * @param _z the k component
     * @param _w the real component*/
    public Quaternion(float _x, float _y, float _z, float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    /** Constructor, create a quaternion from a axis angle
     * @param axis the 3D axis to rotate around
     * @param angle the angle to rotate to*/
    public Quaternion(float[] axis, float angle)
    {
        w = (float)Math.cos(angle/2.0f);
        float s = (float)Math.sin(angle/2.0f);
        x = axis[0]*s;
        y = axis[1]*s;
        z = axis[2]*s;
    }

    /** Get the inverse of this quaternion
     * @return the inverse of this quaternion*/
    public Quaternion getInverse()
    {
        return new Quaternion(-x, -y, -z, w);
    }

    /** Get the multiplication between "this" and x
     * @return this*x */
    public Quaternion multiplyBy(Quaternion q)
    {
        return new Quaternion(w*q.x + x*q.w + y*q.z - z*q.y,  //i part
                              w*q.y - x*q.z + y*q.w + z*q.x,  //j part
                              w*q.z + x*q.y - y*q.x + z*q.w,  //k part
                              w*q.w - x*q.x - y*q.y - z*q.z); //The real part
    }

    /** Rotate a vector v via this quaternion
     * @param v the vector to rotate. The float array should be of size 3 (or more)
     * @return a new vector in a float[3] {x, y, z} array.*/
    public float[] rotateVector(float[] v)
    {
        Quaternion invQ  = getInverse();
        Quaternion qPure = new Quaternion(v[0], v[1], v[2], 0);
        Quaternion res   = this.multiplyBy(qPure).multiplyBy(invQ);
        return new float[]{res.x, res.y, res.z};
    }

    /** Convert this quaternion to a float array
     * @return a float array containing {w, x, y, z}*/
    public float[] toFloatArray()
    {
        return new float[]{w, x, y, z};
    }

    @Override
    public boolean equals(Object obj)
    {
        if(obj == this)
            return true;

        else if(obj instanceof Quaternion)
        {
            Quaternion qObj = (Quaternion)obj;
            return x == qObj.x && y == qObj.y && z == qObj.z && w == qObj.w;
        }

        return false;
    }

    @Override
    public Object clone()
    {
        return new Quaternion(x, y, z, w);
    }
}
