#ifndef  NATIVEGLSURFACEVIEW_INC
#define  NATIVEGLSURFACEVIEW_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT int JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetColorMode(JNIEnv* jenv, jobject instance, jlong ptr);

    JNIEXPORT float JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetTimestep(JNIEnv* jenv, jobject instance, jlong ptr);

    JNIEXPORT float JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetMinClipping(JNIEnv* jenv, jobject instance, jlong ptr);

    JNIEXPORT float JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetMaxClipping(JNIEnv* jenv, jobject instance, jlong ptr);

    JNIEXPORT int JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetDimension(JNIEnv* jenv, jobject instance, jlong ptr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeSetColorMode(JNIEnv* jenv, jobject instance, jlong ptr, jint mode);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeSetTimestep(JNIEnv* jenv, jobject instance, jlong ptr, jfloat time);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeSetClipping(JNIEnv* jenv, jobject instance, jlong ptr, jfloat min, jfloat max);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeDeleteTF(JNIEnv* jenv, jobject instance, jlong ptr);

    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_TF_GTFData_nativeCreatePtr(JNIEnv* jenv, jobject instance, jlong datasetPtr, jboolean enableGradient, jint colorMode);

    JNIEXPORT void  JNICALL Java_com_sereno_vfv_Data_TF_GTFData_nativeUpdateRanges(JNIEnv* jenv, jobject instance, jlong ptr, jlong datasetPtr, jboolean enableGradient, jint jcolorMode,
                                                                                   jintArray jpIDs, jfloatArray jcenters, jfloatArray jscale);

    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_TF_MergeTFData_nativeCreatePtr(JNIEnv* jenv, jobject instance, jfloat t, jlong tf1Ptr, jlong tf2Ptr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_MergeTFData_nativeSetInterpolationParameter(JNIEnv* jenv, jobject instance, jlong ptr, jfloat t);
}

#endif
