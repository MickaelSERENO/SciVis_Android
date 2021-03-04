#ifndef  NATIVEDRAWABLEANNOTATIONPOSITION_INC
#define  NATIVEDRAWABLEANNOTATIONPOSITION_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT jlong       JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationPosition_nativeInitPtr(JNIEnv* jenv, jclass jcls, jlong jcontainerPtr, jlong jposPtr);
    JNIEXPORT void        JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationPosition_nativeSetColor(JNIEnv* jenv, jclass jcls, jlong jptr, jfloat r, jfloat g, jfloat b, jfloat a);
    JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationPosition_nativeGetColor(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT jintArray   JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationPosition_nativeGetMappedDataIndices(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT void        JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationPosition_nativeSetMappedDataIndices(JNIEnv* jenv, jclass jcls, jlong jptr, jintArray jidx);
}

#endif   /* ----- #ifndef NATIVEDRAWABLEANNOTATIONPOSITION_INC  ----- */
