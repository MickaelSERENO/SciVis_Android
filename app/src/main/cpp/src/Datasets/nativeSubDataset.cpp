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

JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetColorMode(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    if(sd->getTransferFunction())
        return sd->getTransferFunction()->getColorMode();
    return RAINBOW;
}

JNIEXPORT jobject JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetSnapshot(JNIEnv* env, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    if(sd == NULL)
        return NULL;

    Snapshot* snap = sd->getSnapshot();
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
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetScale(JNIEnv* jenv, jobject jobj, jlong ptr, jfloatArray s)
{
    float* sArr = jenv->GetFloatArrayElements(s, 0);

    SubDataset* sd = (SubDataset*)ptr;
    sd->setScale(glm::vec3(sArr[0], sArr[1], sArr[2]));
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

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetTFType(JNIEnv* jenv, jobject jobj, jlong ptr, jint tfType)
{
    SubDataset* sd = (SubDataset*)ptr;

    //Save color mode and delete the old tf
    ColorMode colorMode = RAINBOW;
    if(sd->getTransferFunction())
    {
        colorMode = sd->getTransferFunction()->getColorMode();
        delete sd->getTransferFunction();
    }

    //Create the new TF
    TF* tf = NULL;
    if(tfType == TF_GTF)
        tf = new GTF(sd->getParent()->getPointFieldDescs().size(), colorMode);
    else if(tfType == TF_TRIANGULAR_GTF)
        tf = new TriangularGTF(sd->getParent()->getPointFieldDescs().size()+1, colorMode);
    else
        LOG_ERROR("Type %d unknown for transfer function...\n", tfType);

    sd->setTransferFunction(tf);
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetGTFRanges(JNIEnv* jenv, jobject jobj, jlong ptr, jint tfType, jintArray jpIDs, jfloatArray jcenters, jfloatArray jscales)
{
    SubDataset* sd = (SubDataset*)ptr;

    if(sd->getTransferFunction() == NULL)
    {
        LOG_ERROR("No transfer function to use...\n");
        return;
    }

    //Parse java values
    int*   pIDs = jenv->GetIntArrayElements(jpIDs, 0);
    float* centerPID = jenv->GetFloatArrayElements(jcenters, 0);
    float* scalePID  = jenv->GetFloatArrayElements(jscales, 0);

    int size = jenv->GetArrayLength(jpIDs);

    float* center = (float*)malloc(sizeof(float)*size);
    float* scale  = (float*)malloc(sizeof(float)*size);

    for(int i = 0; i < size; i++)
    {
        uint32_t tfID = sd->getParent()->getTFIndiceFromPointFieldID(pIDs[i]);
        if(tfID != (uint32_t)-1 && tfID < size)
        {
            center[tfID] = centerPID[i];
            scale[tfID] = scalePID[i];
        }
        else
        {
            LOG_ERROR("Error, cannot update TF");
            return;
        }
    }

    //Update transfer function
    if(tfType == TF_GTF)
    {
        GTF* gtf = reinterpret_cast<GTF*>(sd->getTransferFunction());
        if(size == gtf->getDimension())
        {
            gtf->setCenter(center);
            gtf->setScale(scale);
        }
    }

    else if(tfType == TF_TRIANGULAR_GTF)
    {
        TriangularGTF* gtf = reinterpret_cast<TriangularGTF*>(sd->getTransferFunction());
        if(size == gtf->getDimension() -1)
        {
            gtf->setCenter(center);
            gtf->setScale(scale);
        }
    }

    //Free variables
    free(center);
    free(scale);
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetColorMode(JNIEnv* jenv, jobject jobj, jlong ptr, int mode)
{
    SubDataset* sd = (SubDataset*)ptr;

    if(sd->getTransferFunction() == NULL)
    {
        LOG_ERROR("No transfer function to use...\n");
        return;
    }

    sd->getTransferFunction()->setColorMode((ColorMode)mode);
}

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeClone(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    return (jlong)(new SubDataset(*(SubDataset*)ptr));
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeFree(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)(ptr);
    if(sd->getTransferFunction())
        delete sd->getTransferFunction();
    delete sd;
}
