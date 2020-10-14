#include "Datasets/nativeSubDataset.h"
#include "Datasets/SubDataset.h"
#include "Datasets/Dataset.h"
#include "TransferFunction/GTF.h"
#include "TransferFunction/TriangularGTF.h"
#include "TransferFunction/TFType.h"
#include "jniData.h"
#include "utils.h"

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeCreateNewSubDataset(JNIEnv* jenv, jclass jcls, jlong datasetPtr, jint id, jstring name)
{
    const char* cName = jenv->GetStringUTFChars(name, 0);
    SubDataset* sd = new SubDataset(((std::shared_ptr<Dataset>*)datasetPtr)->get(), cName, id);
    jenv->ReleaseStringUTFChars(name, cName);

    return (jlong)sd;
}


JNIEXPORT bool JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeIsValid(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    return sd->isValid();
}

JNIEXPORT jobject JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetSnapshot(JNIEnv* env, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    if(sd == NULL)
        return NULL;

    std::shared_ptr<const Snapshot> snap = sd->getSnapshot();
    if(snap == NULL)
        return NULL;

    return createjARGBBitmap(snap->pixels, snap->width, snap->height, env);
}

JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetRotation(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    jfloatArray arr = jenv->NewFloatArray(4);
    const Quaternionf& q   = ((SubDataset*)ptr)->getGlobalRotate();
    float qArr[4] = {q.w, q.x, q.y, q.z};
    jenv->SetFloatArrayRegion(arr, 0, 4, qArr);
    return arr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetRotation(JNIEnv* jenv, jobject jobj, jlong ptr, jfloatArray q)
{
    float* qArr = jenv->GetFloatArrayElements(q, 0);

    SubDataset* sd = (SubDataset*)ptr;
    sd->setGlobalRotate(Quaternionf(qArr[1], qArr[2], qArr[3], qArr[0]));
    jenv->ReleaseFloatArrayElements(q, qArr, JNI_ABORT);

}

JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetPosition(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    jfloatArray arr = jenv->NewFloatArray(4);
    glm::vec3   p   = ((SubDataset*)ptr)->getPosition();
    float pArr[3] = {p[0], p[1], p[2]};
    jenv->SetFloatArrayRegion(arr, 0, 3, pArr);
    return arr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetPosition(JNIEnv* jenv, jobject jobj, jlong ptr, jfloatArray p)
{
    float* pArr = jenv->GetFloatArrayElements(p, 0);

    SubDataset* sd = (SubDataset*)ptr;
    sd->setPosition(glm::vec3(pArr[0], pArr[1], pArr[2]));

    jenv->ReleaseFloatArrayElements(p, pArr, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetScale(JNIEnv* jenv, jobject jobj, jlong ptr, jfloatArray s)
{
    float* sArr = jenv->GetFloatArrayElements(s, 0);

    SubDataset* sd = (SubDataset*)ptr;
    sd->setScale(glm::vec3(sArr[0], sArr[1], sArr[2]));

    jenv->ReleaseFloatArrayElements(s, sArr, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetVolumetricMask(JNIEnv* jenv, jobject jobj, jlong ptr, jbyteArray m)
{
    jbyte* mArr = jenv->GetByteArrayElements(m, 0);
    uint32_t size = jenv->GetArrayLength(m);

    SubDataset* sd = (SubDataset*)ptr;

    if(size == sd->getVolumetricMaskSize())
        memcpy(sd->getVolumetricMask(), mArr, size);
    else
        LOG_INFO("Error, the size of the java-size volumetric mask %d is different than the size of the C++ subdataset volumetric mask %d", size, (uint32_t)sd->getVolumetricMaskSize());

    jenv->ReleaseByteArrayElements(m, mArr, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeResetVolumetricMask(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    sd->resetVolumetricMask(false);
}

JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetScale(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    jfloatArray arr = jenv->NewFloatArray(3);
    glm::vec3   s   = ((SubDataset*)ptr)->getScale();
    float sArr[3] = {s[0], s[1], s[2]};
    jenv->SetFloatArrayRegion(arr, 0, 3, sArr);
    return arr;
}

JNIEXPORT jstring JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetName(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    const std::string& name = sd->getName();
    return jenv->NewStringUTF(name.c_str());
}

JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetID(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    return sd->getID();
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetTF(JNIEnv* jenv, jobject jobj, jlong ptr, jint type, jlong tfPtr)
{
    SubDataset* sd = (SubDataset*)ptr;
    if(tfPtr == 0)
        sd->setTransferFunction(nullptr);
    else
        sd->setTransferFunction(*((std::shared_ptr<TF>*)(tfPtr)));
}

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeClone(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    return (jlong)(new SubDataset(*(SubDataset*)ptr));
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeDelPtr(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)(ptr);
    delete sd;
}