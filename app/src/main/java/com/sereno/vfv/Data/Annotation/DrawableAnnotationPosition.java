package com.sereno.vfv.Data.Annotation;

public class DrawableAnnotationPosition extends DrawableAnnotationLogComponent
{
    protected DrawableAnnotationPosition(AnnotationPosition pos)
    {
        super(nativeInitPtr(pos.getPtr()));
    }

    static native long nativeInitPtr(long posPtr);
}