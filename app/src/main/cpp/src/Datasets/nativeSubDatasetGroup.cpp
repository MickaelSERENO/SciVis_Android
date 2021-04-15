#include "Datasets/nativeSubDatasetGroup.h"
#include "Datasets/SubDatasetGroup.h"

using namespace sereno;

JNIEXPORT void       JNICALL Java_com_sereno_vfv_Data_SubDatasetGroup_nativeDeletePtr(JNIEnv *env, jclass clazz, jlong sdgPtr)
{
    SubDatasetGroup* sdg = (SubDatasetGroup*)sdgPtr;
    delete sdg;
}

JNIEXPORT jlongArray JNICALL Java_com_sereno_vfv_Data_SubDatasetGroup_nativeGetSubDatasets(JNIEnv *jenv, jclass clazz, jlong sdgPtr)
{
    SubDatasetGroup* sdg = (SubDatasetGroup*)sdgPtr;
    jlongArray arr = jenv->NewLongArray(sdg->getSubDatasets().size());

    auto it = sdg->getSubDatasets().begin();
    for(int i = 0; i < sdg->getSubDatasets().size(); i++, it++)
    {
        jlong valPtr = (jlong)(*it);
        jenv->SetLongArrayRegion(arr, i, 1, &valPtr);
    }
    return arr;
}

JNIEXPORT jboolean   JNICALL Java_com_sereno_vfv_Data_SubDatasetGroup_nativeRemoveSubDatasets(JNIEnv *env, jclass clazz, jlong sdgPtr, jlong sdPtr)
{
    SubDatasetGroup* sdg = (SubDatasetGroup*)sdgPtr;
    SubDataset*      sd  = (SubDataset*)sdPtr;

    return sdg->removeSubDataset(sd);
}

JNIEXPORT void       JNICALL Java_com_sereno_vfv_Data_SubDatasetGroup_nativeUpdateSubDatasets(JNIEnv *env, jclass clazz, jlong sdgPtr)
{
    SubDatasetGroup* sdg = (SubDatasetGroup*)sdgPtr;
    sdg->updateSubDatasets();
}