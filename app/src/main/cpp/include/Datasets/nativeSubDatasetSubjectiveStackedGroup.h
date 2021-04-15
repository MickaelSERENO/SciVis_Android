#ifndef  NATIVEANNOTATIONLOGCONTAINER_INC
#define  NATIVEANNOTATIONLOGCONTAINER_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT jlong      JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeCreatePtr(JNIEnv *env, jclass clazz, jlong basePtr);
    JNIEXPORT void       JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeSetGap(JNIEnv *env, jclass clazz, jlong sdgPtr, jfloat gap);
    JNIEXPORT jfloat     JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetGap(JNIEnv *env, jclass clazz, jlong sdgPtr);
    JNIEXPORT void       JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeSetMerge(JNIEnv *env, jclass clazz, jlong sdgPtr, jboolean merge);
    JNIEXPORT jboolean   JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetMerge(JNIEnv *env, jclass clazz, jlong sdgPtr);
    JNIEXPORT void       JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeSetStackingMethod(JNIEnv *env, jclass clazz, jlong sdgPtr, jint stack);
    JNIEXPORT jint       JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetStackingMethod(JNIEnv *env, jclass clazz, jlong sdgPtr);
    JNIEXPORT jboolean   JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeAddSubjectiveSubDataset(JNIEnv *env, jclass clazz, jlong sdgPtr, jlong sdStackedPtr, jlong sdLinkedPtr);
    JNIEXPORT jlongArray JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetSubjectiveSubDatasets(JNIEnv *env, jclass clazz, jlong sdgPtr);
}
#endif