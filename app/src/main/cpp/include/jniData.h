#ifndef  JNIDATA_INC
#define  JNIDATA_INC

#include <jni.h>

namespace sereno
{
    extern JavaVM* javaVM;
}

extern "C"
{
    jint JNI_Onload(JavaVM* vm, void* reserved);
}

#endif
