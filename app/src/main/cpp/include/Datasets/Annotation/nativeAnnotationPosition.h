#ifndef  NATIVEANNOTATIONPOSITION_INC
#define  NATIVEANNOTATIONPOSITION_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeInitPtr(JNIEnv* jenv, jclass jcls, jlong jannPtr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeSetXYZHeaderInteger(JNIEnv* jenv, jclass jcls, jlong jptr, jint x, jint y, jint z);
    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeSetXYZHeaderString(JNIEnv* jenv, jclass jcls, jlong jptr, jstring x, jstring y, jstring z);
}

#endif
