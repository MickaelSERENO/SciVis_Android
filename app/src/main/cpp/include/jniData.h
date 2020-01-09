#ifndef  JNIDATA_INC
#define  JNIDATA_INC

#include <jni.h>
#include <cstdint>

namespace sereno
{
    extern JavaVM* javaVM;
    extern JNIEnv* jniMainThread;

    extern jclass    jVFVSurfaceViewClass;
    extern jmethodID jVFVSurfaceView_setCurrentAction;

    extern jclass    jBitmapClass;
    extern jmethodID jBitmap_createBitmap;

    extern jclass    jBitmapConfigClass;
    extern jfieldID  jBitmapConfig_ARGB;
    extern jobject   jBitmapConfigARGB;

    extern jclass    jDatasetClass;
    extern jmethodID jDataset_getNbSubDataset;
    extern jmethodID jDataset_getSubDataset;
    extern jmethodID jDataset_onLoadDataset;
    extern jmethodID jDataset_onLoadCPCPTexture;
    extern jmethodID jDataset_onLoad1DHistogram;

    extern jclass    jSubDatasetClass;
    extern jmethodID jSubDataset_setRotation;
    extern jmethodID jSubDataset_setPosition;
    extern jmethodID jSubDataset_setScale;
    extern jmethodID jSubDataset_onSnapshotEvent;
    extern jmethodID jSubDataset_getCanBeModified;

    extern jclass    jHeadsetStatusClass;
    extern jfieldID  jHeadsetStatus_position;
    extern jfieldID  jHeadsetStatus_rotation;
    extern jfieldID  jHeadsetStatus_id;
    extern jfieldID  jHeadsetStatus_color;
    extern jfieldID  jHeadsetStatus_currentAction;

    extern jclass    jHeadsetBindingInfoMessageClass;
    extern jmethodID jHeadsetBindingInfoMessage_getHeadsetID;

    extern jclass    jPointFieldDescClass;
    extern jmethodID jPointFieldDesc_constructor;
}

extern "C"
{
    jint JNI_OnLoad(JavaVM* vm, void* reserved);
    void JNI_OnUnload(JavaVM *vm, void *reserved);
}

/* \brief  Get a new JNI Environment object
 * \param shouldDetach[out] pointer to a value permitting to know if the called has to Detach (see DetachCurrentThread) or not the JNIEnv got. Must not be NULL
 * \return   NULL on error, the jniEnv on success.*/
JNIEnv* getJNIEnv(bool* shouldDetach);

/* \brief Create an ARGB java Bitmap using Pixel data
 * \param pixels the ARGB pixels array. Each int value should contain ARGB data. Length: width*height
 * \param width the bitmap width.
 * \param height the bitmap height
 * \param env the jni environment to use.*/
jobject createjARGBBitmap(uint32_t* pixels, uint32_t width, uint32_t height, JNIEnv* env);

/** \brief Create a JNI Float Array from a C++ array
 * \param values the float array to transmit
 * \param size the float array size
 * \param env the jni environment to use.*/
jfloatArray createjFloatArray(float* values, size_t size, JNIEnv* env);

#endif
