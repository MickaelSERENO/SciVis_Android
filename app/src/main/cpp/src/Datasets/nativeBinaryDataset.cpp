#include "Datasets/nativeBinaryDataset.h"
#include <memory>

using namespace sereno;

JNIEXPORT jlong      JNICALL Java_com_sereno_vfs_Data_BinaryDataset_nativeInitPtr(JNIEnv* jenv, jclass jcls, jstring path)
{
    const char* strPath = jenv->GetStringUTFChars(path, 0);
    std::shared_ptr<BinaryDataset>* fd = new std::shared_ptr<BinaryDataset>(BinaryDataset::readFromFilePath(strPath));
    jenv->ReleaseStringUTFChars(path, strPath);
    return (jlong)(fd);
}

JNIEXPORT jintArray JNICALL Java_com_sereno_vfs_Data_BinaryDataset_nativeGetSize(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<BinaryDataset>* fd = (std::shared_ptr<BinaryDataset>*)jptr;
    jintArray     retval = jenv->NewIntArray(3);
    jenv->SetIntArrayRegion(retval, 0, 3, (const int*)(*fd)->getGridSize());
    return retval;
}
