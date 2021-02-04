#ifndef  NATIVEANNOTATIONLOGCONTAINER_INC
#define  NATIVEANNOTATIONLOGCONTAINER_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeInitPtr(JNIEnv* jenv, jclass jcls, jboolean hasHeaders);
    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeParseCSV(JNIEnv* jenv, jclass jcls, jlong jptr, jstring path);
    JNIEXPORT jobjectArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeGetHeaders(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeSetTimeHeaderInt(JNIEnv* jenv, jclass jcls, jlong jptr, int header);
    JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeSetTimeHeaderString(JNIEnv* jenv, jclass jcls, jlong jptr, jstring header);
    JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeGetNbColumns(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeHasHeaders(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT jintArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeGetRemainingHeaders(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT jintArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeGetConsumedHeaders(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeInitAnnotationPosition(JNIEnv* jenv, jclass jcls, jlong jptr);
    JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativePushAnnotationPosition(JNIEnv* jenv, jclass jcls, jlong jptr, jlong posPtr);
    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeRemoveAnnotationPosition(JNIEnv* jenv, jclass jcls, jlong jptr, jlong posPtr);
}

#endif
