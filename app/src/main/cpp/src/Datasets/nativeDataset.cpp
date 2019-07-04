#include "Datasets/nativeDataset.h"

#include <memory>
#include "Datasets/Dataset.h"

using namespace sereno;

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Dataset_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<Dataset>* d = (std::shared_ptr<Dataset>*)jptr;
    delete d;
}


JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_Dataset_nativeGetNbSubDatasets(JNIEnv* jenv, jclass jcls, jlong ptr)
{
    std::shared_ptr<Dataset>* d = (std::shared_ptr<Dataset>*)ptr;
    return (*d)->getNbSubDatasets();
}

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Dataset_nativeGetSubDataset(JNIEnv* jenv, jclass jcls, jlong ptr, jint i)
{
    std::shared_ptr<Dataset>* d = (std::shared_ptr<Dataset>*)ptr;
    return (jlong)(*d)->getSubDataset(i);
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Dataset_nativeRemoveSubDataset(JNIEnv* jenv, jclass jcls, jlong ptr, jlong sdPtr)
{
    std::shared_ptr<Dataset>* d = (std::shared_ptr<Dataset>*)ptr;
    (*d)->removeSubDataset((SubDataset*)sdPtr);
}
