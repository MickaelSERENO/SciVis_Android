#include "jniData.h"

namespace sereno
{
    JavaVM* javaVM = NULL;
}

jint JNI_Onload(JavaVM* vm, void* reserved)
{
    sereno::javaVM = vm;
    return JNI_VERSION_1_4;
}
