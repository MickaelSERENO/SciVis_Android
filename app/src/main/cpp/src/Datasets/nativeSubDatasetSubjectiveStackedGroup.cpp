#include "Datasets/nativeSubDatasetSubjectiveStackedGroup.h"
#include "Datasets/SubDatasetGroup.h"

using namespace sereno;

JNIEXPORT jlong    JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeCreatePtr(JNIEnv *env, jclass clazz, jlong basePtr)
{
    SubDataset* base = (SubDataset*)basePtr;
    SubDatasetSubjectiveStackedLinkedGroup* sdg = new SubDatasetSubjectiveStackedLinkedGroup(base);
    return (jlong)(sdg);
}

JNIEXPORT void     JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeSetGap(JNIEnv *env, jclass clazz, jlong sdgPtr, jfloat gap)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    sdg->setGap(gap);
}

JNIEXPORT jfloat   JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetGap(JNIEnv *env, jclass clazz, jlong sdgPtr)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    return sdg->getGap();
}

JNIEXPORT void     JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeSetMerge(JNIEnv *env, jclass clazz, jlong sdgPtr, jboolean merge)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    sdg->setMerge(merge);
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetMerge(JNIEnv *env, jclass clazz, jlong sdgPtr)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    return sdg->getMerge();
}

JNIEXPORT void     JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeSetStackingMethod(JNIEnv *env, jclass clazz, jlong sdgPtr, jint stack)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    sdg->setStackingMethod((StackingEnum)stack);
}

JNIEXPORT jint     JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetStackingMethod(JNIEnv *env, jclass clazz, jlong sdgPtr)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    return (jint)sdg->getStackingMethod();
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeAddSubjectiveSubDataset(JNIEnv *env, jclass clazz, jlong sdgPtr, jlong sdStackedPtr, jlong sdLinkedPtr)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    SubDataset* sdStacked = (SubDataset*)sdStackedPtr;
    SubDataset* sdLinked  = (SubDataset*)sdLinkedPtr;

    return sdg->addSubjectiveSubDataset(sdStacked, sdLinked);
}

JNIEXPORT jlongArray JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetSubjectiveSubDatasets(JNIEnv *jenv, jclass clazz, jlong sdgPtr)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    const std::list<std::pair<SubDataset*, SubDataset*>>& pairs = sdg->getLinkedSubDatasets();

    jlongArray arr = jenv->NewLongArray(pairs.size()*2);

    auto it = pairs.begin();
    for(int i = 0; i < pairs.size(); i++, it++)
    {
        jlong valPtr[2] = {(jlong)(it->first), (jlong)it->second};
        jenv->SetLongArrayRegion(arr, 2*i, 2, valPtr);
    }
    return arr;
}

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_SubDatasetSubjectiveStackedGroup_nativeGetBase(JNIEnv *env, jclass clazz, jlong sdgPtr)
{
    SubDatasetSubjectiveStackedLinkedGroup* sdg = (SubDatasetSubjectiveStackedLinkedGroup*)sdgPtr;
    return (jlong)sdg->getBase();
}