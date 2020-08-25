#ifndef  NATIVEGLSURFACEVIEW_INC
#define  NATIVEGLSURFACEVIEW_INC

#include <jni.h>

extern "C"
{
    JNIEXPORT int JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetColorMode(JNIEnv* jenv, jobject instance, jlong ptr);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeSetColorMode(JNIEnv* jenv, jobject instance, jlong ptr, jint mode);

    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeDeleteTF(JNIEnv* jenv, jobject instance, jlong ptr);

    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_TF_GTFData_nativeCreatePtr(JNIEnv* jenv, jobject instance, jlong datasetPtr, jboolean enableGradient, jint colorMode);

    JNIEXPORT void  JNICALL Java_com_sereno_vfv_Data_TF_GTFData_nativeUpdateRanges(JNIEnv* jenv, jobject instance, jlong ptr, jlong datasetPtr, jboolean enableGradient, jint jcolorMode,
                                                                                   jintArray jpIDs, jfloatArray jcenters, jfloatArray jscale);

    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_TF_MergeTFData_nativeCreatePtr(JNIEnv* jenv, jobject instance, jfloat t, jlong tf1Ptr, jlong tf2Ptr);
}

#endif
