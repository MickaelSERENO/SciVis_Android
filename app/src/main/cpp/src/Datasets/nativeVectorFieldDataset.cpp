#include "Datasets/nativeVectorFieldDataset.h"

#include <memory>
#include "Datasets/VectorFieldDataset.h"

using namespace sereno;

JNIEXPORT jlong      JNICALL Java_com_sereno_vfv_Data_VectorFieldDataset_nativeInitPtr(JNIEnv* jenv, jclass jcls, jstring path)
{
    const char* strPath = jenv->GetStringUTFChars(path, 0);
    std::shared_ptr<VectorFieldDataset>* fd = new std::shared_ptr<VectorFieldDataset>(VectorFieldDataset::readFromFilePath(strPath));
    jenv->ReleaseStringUTFChars(path, strPath);
    return (jlong)(fd);
}

JNIEXPORT jintArray JNICALL Java_com_sereno_vfv_Data_VectorFieldDataset_nativeGetSize(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<VectorFieldDataset>* fd = (std::shared_ptr<VectorFieldDataset>*)jptr;
    jintArray     retval = jenv->NewIntArray(3);
    jenv->SetIntArrayRegion(retval, 0, 3, (const int*)(*fd)->getGridSize());
    return retval;
}
