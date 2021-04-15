#ifndef  NATIVEANNOTATIONLOGCONTAINER_INC
#define  NATIVEANNOTATIONLOGCONTAINER_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT void       JNICALL Java_com_sereno_vfv_Data_SubDatasetGroup_nativeDeletePtr(JNIEnv *env, jclass clazz, jlong sdgPtr);
    JNIEXPORT jlongArray JNICALL Java_com_sereno_vfv_Data_SubDatasetGroup_nativeGetSubDatasets(JNIEnv *env, jclass clazz, jlong sdgPtr);
    JNIEXPORT jboolean   JNICALL Java_com_sereno_vfv_Data_SubDatasetGroup_nativeRemoveSubDatasets(JNIEnv *env, jclass clazz, jlong sdgPtr, jlong sdPtr);
    JNIEXPORT void       JNICALL Java_com_sereno_vfv_Data_SubDatasetGroup_nativeUpdateSubDatasets(JNIEnv *env, jclass clazz, jlong sdgPtr);
}
#endif