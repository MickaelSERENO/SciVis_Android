#include "Datasets/nativeCloudPointDataset.h"
#include "Datasets/CloudPointDataset.h"
#include <memory>

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_CloudPointDataset_nativeInitPtr(JNIEnv* jenv, jclass jcls, jstring path)
{
    const char* strPath = jenv->GetStringUTFChars(path, 0);
    std::shared_ptr<CloudPointDataset>* fd = new std::shared_ptr<CloudPointDataset>(new CloudPointDataset(strPath));
    jenv->ReleaseStringUTFChars(path, strPath);
    return (jlong)(fd);
}
