#include "Datasets/nativeDataset.h"

#include <memory>
#include "Datasets/Dataset.h"
#include "jniData.h"

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

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Dataset_nativeAddSubDataset(JNIEnv* jenv, jclass jcls, jlong ptr, jlong sdPtr, jboolean updateID)
{
    std::shared_ptr<Dataset>* dataset = (std::shared_ptr<Dataset>*)ptr;
    SubDataset* sd = (SubDataset*)sdPtr;

    (updateID == false) ? ((*dataset)->addSubDataset(sd)) : ((*dataset)->addSubDatasetWithID(sd));
}

JNIEXPORT jobjectArray JNICALL Java_com_sereno_vfv_Data_Dataset_nativeGetPointFieldDescs(JNIEnv* jenv, jclass jcls, jlong ptr)
{
    //Fetch pointers
    std::shared_ptr<Dataset>* dataset = (std::shared_ptr<Dataset>*)ptr;
    Dataset* d = dataset->get();
    const std::vector<PointFieldDesc>& descs = d->getPointFieldDescs();

    //Create and initialize the array
    jobjectArray jArr = jenv->NewObjectArray(descs.size(), jPointFieldDescClass, NULL);
    for(uint32_t i = 0; i < descs.size(); i++)
    {
        jobject jobj = jenv->NewObject(jPointFieldDescClass, jPointFieldDesc_constructor, descs[i].id, descs[i].minVal, descs[i].maxVal, descs[i].values.get() != NULL);
        jenv->SetObjectArrayElement(jArr, i, jobj);
    }

    return jArr;
}
