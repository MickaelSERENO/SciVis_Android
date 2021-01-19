#ifndef  NATIVEANNOTATIONPOSITION_INC
#define  NATIVEANNOTATIONPOSITION_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeInitPtr(JNIEnv* jenv, jclass jcls, jlong jannPtr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeSetColor(JNIEnv* jenv, jclass jcls, jlong jptr, jfloat r, jfloat g, jfloat b, jfloat a);
    JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeGetColor(JNIEnv* jenv, jclass jcls, jlong jptr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeSetXYZHeader_Integer(JNIEnv* jenv, jclass jcls, jlong jptr, jint x, jint y, jint z);
    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeSetXYZHeader_String(JNIEnv* jenv, jclass jcls, jlong jptr, jstring x, jstring y, jstring z);
}

#endif
