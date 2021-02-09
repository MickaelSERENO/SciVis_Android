#ifndef  NATIVEDRAWABLEANNOTATIONLOGCOMPONENT_INC
#define  NATIVEDRAWABLEANNOTATIONLOGCOMPONENT_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT void     JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT void     JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeSetEnableTime(JNIEnv* jenv, jclass jcls, jlong jptr, jboolean t);
    JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeGetEnableTime(JNIEnv* jenv, jclass jcls, jlong jptr);
}

#endif
