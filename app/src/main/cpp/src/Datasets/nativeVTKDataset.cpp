#include "Datasets/nativeVTKDataset.h"
#include "Datasets/VTKDataset.h"
#include "VTKParser.h"
#include "utils.h"
#include <memory>

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_VTKDataset_nativeInitPtr(JNIEnv* env, jobject instance, jlong parserPtr, jlongArray ptFieldValues, jlongArray cellFieldValues)
{
    std::shared_ptr<VTKParser>* parser = (std::shared_ptr<VTKParser>*)(parserPtr);
    return (jlong)(new std::shared_ptr<VTKDataset>(new VTKDataset(*parser, jlongArrayToVector<VTKFieldValue>(env, ptFieldValues), 
                                                                  jlongArrayToVector<VTKFieldValue>(env, cellFieldValues)))); 
}
