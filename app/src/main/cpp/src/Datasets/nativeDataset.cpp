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

JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_Dataset_nativeGetNbTimesteps(JNIEnv* jenv, jclass jcls, jlong ptr)
{
    std::shared_ptr<Dataset>* d = (std::shared_ptr<Dataset>*)ptr;
    return (*d)->getNbTimesteps();
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

    (updateID) ? ((*dataset)->addSubDataset(sd)) : ((*dataset)->addSubDatasetWithID(sd));
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
        jstring jname = jenv->NewStringUTF(descs[i].name.c_str());
        jobject jobj  = jenv->NewObject(jPointFieldDescClass, jPointFieldDesc_constructor, descs[i].id, jname, descs[i].minVal, descs[i].maxVal, descs[i].values.size() != 0);
        jenv->SetObjectArrayElement(jArr, i, jobj);
        jenv->DeleteLocalRef(jobj);
        jenv->DeleteLocalRef(jname);
    }

    return jArr;
}

JNIEXPORT jobject JNICALL Java_com_sereno_vfv_Data_Dataset_nativeGetMetadata(JNIEnv* jenv, jclass jcls, jlong ptr)
{
    //Fetch pointers
    std::shared_ptr<Dataset>* dataset = (std::shared_ptr<Dataset>*)ptr;
    Dataset* d = dataset->get();
    const DatasetMetadata& md = d->getMetadata();

    //Create metadata object
    jobject jmd        = jenv->NewObject(jDatasetMetadataClass, jDatasetMetadata_constructor);

    //Coastline
    jstring jcoastline = jenv->NewStringUTF(md.coastlinePath.c_str());
    jenv->SetObjectField(jmd, jDatasetMetadata_coastline, jcoastline);

    //Timesteps
    jobjectArray jPerTimesteps = jenv->NewObjectArray(md.perTimestepMetadata.size(), jDatasetMetaData_PerTimestepMetadataClass, 0);
    for(int i = 0; i < md.perTimestepMetadata.size(); i++)
    {
        jobject jtimestep = jenv->NewObject(jDatasetMetaData_PerTimestepMetadataClass, jDatasetMetaData_PerTimestepMetadata_constructor);
        jstring jdate = jenv->NewStringUTF(md.perTimestepMetadata[i].date.c_str());
        jenv->SetObjectField(jtimestep, jDatasetMetadata_PerTimestepMetadata_date, jdate);
        jenv->SetObjectArrayElement(jPerTimesteps, i, jtimestep);

        jenv->DeleteLocalRef(jtimestep);
        jenv->DeleteLocalRef(jdate);
    }
    jenv->SetObjectField(jmd, jDatasetMetadata_perTimesteps, jPerTimesteps);

    jenv->DeleteLocalRef(jcoastline);
    jenv->DeleteLocalRef(jPerTimesteps);

    return jmd;
}