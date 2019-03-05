#include "jniData.h"

namespace sereno
{
    JavaVM*   javaVM                      = NULL;
    JNIEnv*   jniMainThread               = NULL;

    jclass    jBitmapClass                = 0;
    jmethodID jBitmap_createBitmap        = 0;

    jclass    jBitmapConfigClass          = 0;
    jfieldID  jBitmapConfig_ARGB          = 0;
    jobject   jBitmapConfigARGB           = 0;

    jclass    jDatasetClass               = 0;
    jmethodID jDataset_getNbSubDataset    = 0;
    jmethodID jDataset_getSubDataset      = 0;

    jclass    jSubDatasetClass            = 0;
    jmethodID jSubDataset_onRotationEvent = 0;
    jmethodID jSubDataset_onSnapshotEvent = 0;
}

using namespace sereno;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    sereno::javaVM = vm;

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return -1;

    //Load classes
    jclass bmpCls     = env->FindClass("android/graphics/Bitmap");
    jBitmapClass      = (jclass)env->NewGlobalRef(bmpCls);
    env->DeleteLocalRef(bmpCls);

    jclass bmpConfCls  = env->FindClass("android/graphics/Bitmap$Config");
    jBitmapConfigClass = (jclass)env->NewGlobalRef(bmpConfCls);
    env->DeleteLocalRef(bmpConfCls);

    jclass datasetCls = env->FindClass("com/sereno/vfv/Data/Dataset");
    jDatasetClass     = (jclass)env->NewGlobalRef(datasetCls);
    env->DeleteLocalRef(datasetCls);

    jclass subDatasetCls = env->FindClass("com/sereno/vfv/Data/SubDataset");
    jSubDatasetClass     = (jclass)env->NewGlobalRef(subDatasetCls);
    env->DeleteLocalRef(subDatasetCls);

    //Load methods
    jDataset_getNbSubDataset    = env->GetMethodID(jDatasetClass, "getNbSubDataset", "()I");
    jDataset_getSubDataset      = env->GetMethodID(jDatasetClass, "getSubDataset", "(I)Lcom/sereno/vfv/Data/SubDataset;");

    jSubDataset_onRotationEvent = env->GetMethodID(jSubDatasetClass, "onRotationEvent", "(FFF)V");
    jSubDataset_onSnapshotEvent = env->GetMethodID(jSubDatasetClass, "onSnapshotEvent", "(Landroid/graphics/Bitmap;)V");

    //Load fields
    jBitmapConfig_ARGB   = env->GetStaticFieldID(jBitmapConfigClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
    jBitmap_createBitmap = env->GetStaticMethodID(jBitmapClass, "createBitmap", "([IIILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    
    //Load static object
    jobject bmpConfARGB = env->GetStaticObjectField(jBitmapConfigClass, jBitmapConfig_ARGB);
    jBitmapConfigARGB     = env->NewGlobalRef(bmpConfARGB);
    env->DeleteLocalRef(bmpConfARGB);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv* env;
    if(vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return;

    env->DeleteGlobalRef(jBitmapClass);
    env->DeleteGlobalRef(jBitmapConfigClass);
    env->DeleteGlobalRef(jDatasetClass);
    env->DeleteGlobalRef(jSubDatasetClass);
    env->DeleteGlobalRef(jBitmapConfigARGB);
}
