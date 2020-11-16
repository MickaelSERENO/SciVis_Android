#ifndef  NATIVESUBDATASET_INC
#define  NATIVESUBDATASET_INC

#include <jni.h>

extern "C"
{
	JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeCreateNewSubDataset(JNIEnv* jenv, jclass jcls, jlong datasetPtr, jint id, jstring name);

	JNIEXPORT bool JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeIsValid(JNIEnv* jenv, jobject jobj, jlong ptr);

    JNIEXPORT jobject JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetSnapshot(JNIEnv* jenv, jobject jobj, jlong ptr);

	JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetRotation(JNIEnv* jenv, jobject jobj, jlong ptr);

	JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetRotation(JNIEnv* jenv, jobject jobj, jlong ptr, jfloatArray q);

	JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetPosition(JNIEnv* jenv, jobject jobj, jlong ptr);

	JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetPosition(JNIEnv* jenv, jobject jobj, jlong ptr, jfloatArray p);

	JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetScale(JNIEnv* jenv, jobject jobj, jlong ptr);

	JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetScale(JNIEnv* jenv, jobject jobj, jlong ptr, jfloatArray s);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetVolumetricMask(JNIEnv* jenv, jobject jobj, jlong ptr, jbyteArray m);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeResetVolumetricMask(JNIEnv* jenv, jobject jobj, jlong ptr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeEnableVolumetricMask(JNIEnv* jenv, jobject jobj, jlong ptr, jboolean b);

	JNIEXPORT jstring JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetName(JNIEnv* jenv, jobject jobj, jlong ptr);

    JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetID(JNIEnv* jenv, jobject jobj, jlong ptr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetTF(JNIEnv* jenv, jobject jobj, jlong ptr, jint type, jlong tfPtr);

    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeClone(JNIEnv* jenv, jobject jobj, jlong ptr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeDelPtr(JNIEnv* jenv, jobject jobj, jlong ptr);
}

#endif
