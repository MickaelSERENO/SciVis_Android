#include "nativeFluidDataset.h"

using namespace sereno;

JNIEXPORT jlong      JNICALL Java_com_sereno_vfs_FluidDataset_Data_nativeInitPtr(JNIEnv* jenv, jobject jobj, jstring path)
{
    const char* strPath = jenv->GetStringUTFChars(path, 0);
    FluidDataset* fd = FluidDataset::readFromFilePath(strPath);
    jenv->ReleaseStringUTFChars(path, strPath);
    return (jlong)(fd);
}

JNIEXPORT void       JNICALL Java_com_sereno_vfs_FluidDataset_Data_nativeDelPtr(JNIEnv* jenv, jobject jobj, jlong jptr)
{
    FluidDataset* fd = (FluidDataset*)jptr;
    delete fd;
}

JNIEXPORT jintArray JNICALL Java_com_sereno_vfs_FluidDataset_Data_nativeGetSize(JNIEnv* jenv, jobject jobj, jlong jptr)
{
    FluidDataset* fd     = (FluidDataset*)jptr;
    jintArray     retval = jenv->NewIntArray(3);
    jenv->SetIntArrayRegion(retval, 0, 3, (const int*)fd->getGridSize());
    return retval;
}
