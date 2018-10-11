#include "nativeFluidDataset.h"

JNIEXPORT jlong      JNICALL Java_com_sereno_vfs_FluidDataset_Data_initPtr(JNIEnv* jenv, jobject jobj, jstring path)
{
    const char* strPath = jenv->GetStringUTFChars(path, 0);
    FluidDataset* fd = FluidDataset::readFromFilePath(strPath);
    jenv->ReleaseStringUTFChars(path, strPath);
    return (jlong)(fd);
}

JNIEXPORT jintArray JNICALL Java_com_sereno_vfs_FluidDataset_Data_getSize(JNIEnv* jenv, jobject jobj, jlong   jptr)
{
    FluidDataset* fd     = (FluidDataset*)fd;
    jintArray     retval = jenv->NewIntArray(3);
    jenv->SetIntArrayRegion(retval, 0, 3, (const int*)fd->getGridSize());
    return retval;
}
