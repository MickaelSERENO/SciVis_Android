#include "Datasets/nativeDataset.h"
#include <memory>

using namespace sereno;

JNIEXPORT void JNICALL Java_com_sereno_vfs_Data_Dataset_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<Dataset>* d = (std::shared_ptr<Dataset>*)jptr;
    delete d;
}

