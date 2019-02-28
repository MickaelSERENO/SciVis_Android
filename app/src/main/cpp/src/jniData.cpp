#include "jniData.h"

namespace sereno
{
    JavaVM*   javaVM                      = NULL;
    JNIEnv*   jniMainThread               = NULL;

    jclass    jDatasetClass               = 0;
    jmethodID jDataset_getNbSubDataset    = 0;
    jmethodID jDataset_getSubDataset      = 0;

    jclass    jSubDatasetClass            = 0;
    jmethodID jSubDataset_onRotationEvent = 0;
}

using namespace sereno;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    sereno::javaVM = vm;

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return -1;

    //Load classes
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

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv* env;
    if(vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return;

    env->DeleteGlobalRef(jSubDatasetClass);
}
