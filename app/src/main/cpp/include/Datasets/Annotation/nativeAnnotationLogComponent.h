#ifndef  NATIVEANNOTATIONLOGCOMPONENT_INC
#define  NATIVEANNOTATIONLOGCOMPONENT_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogComponent_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr);

    JNIEXPORT jintArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogComponent_nativeGetHeaders(JNIEnv* jenv, jclass jcls, jlong jptr);
}

#endif
