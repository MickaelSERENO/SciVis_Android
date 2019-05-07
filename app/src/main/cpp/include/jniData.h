#ifndef  JNIDATA_INC
#define  JNIDATA_INC

#include <jni.h>

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

    extern jclass    jSubDatasetClass;
    extern jmethodID jSubDataset_setRotation;
    extern jmethodID jSubDataset_setPosition;
    extern jmethodID jSubDataset_setScale;
    extern jmethodID jSubDataset_onSnapshotEvent;

    extern jclass    jHeadsetStatusClass;
    extern jfieldID  jHeadsetStatus_position;
    extern jfieldID  jHeadsetStatus_rotation;
    extern jfieldID  jHeadsetStatus_id;
    extern jfieldID  jHeadsetStatus_color;
    extern jfieldID  jHeadsetStatus_currentAction;

    extern jclass    jHeadsetBindingInfoMessageClass;
    extern jmethodID jHeadsetBindingInfoMessage_getHeadsetID;
}

extern "C"
{
    jint JNI_OnLoad(JavaVM* vm, void* reserved);
    void JNI_OnUnload(JavaVM *vm, void *reserved);
}

#endif
