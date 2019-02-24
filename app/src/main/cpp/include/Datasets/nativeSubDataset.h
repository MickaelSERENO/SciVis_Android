#ifndef  NATIVESUBDATASET_INC
#define  NATIVESUBDATASET_INC

#include <jni.h>

extern "C"
{
	JNIEXPORT bool JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeIsValid(JNIEnv* jenv, jobject jobj, jlong ptr);

	JNIEXPORT jfloat JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetMinAmplitude(JNIEnv* jenv, jobject jobj, jlong ptr);

	JNIEXPORT jfloat JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetMaxAmplitude(JNIEnv* jenv, jobject jobj, jlong ptr);

    JNIEXPORT jobject JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetSnapshot(JNIEnv* jenv, jobject jobj, jlong ptr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetRangeColor(JNIEnv* jenv, jobject jobj, jlong ptr, jfloat min, jfloat max, jint mode);
}

#endif
