#include "nativeTransferFunction.h"
#include "TransferFunction/GTF.h"
#include "TransferFunction/TriangularGTF.h"
#include "TransferFunction/MergeTF.h"
#include "Datasets/Dataset.h"
#include "utils.h"

using namespace sereno;

JNIEXPORT int JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetColorMode(JNIEnv* jenv, jobject instance, jlong ptr)
{
    std::shared_ptr<TF>* tf = (std::shared_ptr<TF>*)ptr;
    if(tf)
        return (int)((*tf)->getColorMode());
    return 0;
}

JNIEXPORT float JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetTimestep(JNIEnv* jenv, jobject instance, jlong ptr)
{
    std::shared_ptr<TF>* tf = (std::shared_ptr<TF>*)ptr;
    if(tf)
        return (*tf)->getCurrentTimestep();
    return 0;
}


JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeSetColorMode(JNIEnv* jenv, jobject instance, jlong ptr, jint mode)
{
    std::shared_ptr<TF>* tf = (std::shared_ptr<TF>*)ptr;
    if(tf)
        (*tf)->setColorMode((ColorMode)mode);
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeSetTimestep(JNIEnv* jenv, jobject instance, jlong ptr, jfloat time)
{
    std::shared_ptr<TF>* tf = (std::shared_ptr<TF>*)ptr;
    if(tf)
        (*tf)->setCurrentTimestep(time);
}

JNIEXPORT float JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetMinClipping(JNIEnv* jenv, jobject instance, jlong ptr)
{
    std::shared_ptr<TF>* tf = (std::shared_ptr<TF>*)ptr;
    if(tf)
        return (*tf)->getMinClipping();
    return 0.0f;
}

JNIEXPORT float JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeGetMaxClipping(JNIEnv* jenv, jobject instance, jlong ptr)
{
    std::shared_ptr<TF>* tf = (std::shared_ptr<TF>*)ptr;
    if(tf)
        return (*tf)->getMaxClipping();
    return 1.0f;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeSetClipping(JNIEnv* jenv, jobject instance, jlong ptr, jfloat min, jfloat max)
{
    std::shared_ptr<TF>* tf = (std::shared_ptr<TF>*)ptr;
    if(tf)
        (*tf)->setClipping(min, max);
}


JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_TransferFunction_nativeDeleteTF(JNIEnv* jenv, jobject instance, jlong ptr)
{
    std::shared_ptr<TF>* tf = (std::shared_ptr<TF>*)ptr;
    if(tf)
        delete tf;
}

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_TF_GTFData_nativeCreatePtr(JNIEnv* jenv, jobject instance, jlong datasetPtr, jboolean enableGradient, jint colorMode)
{
    std::shared_ptr<Dataset>* dataset = (std::shared_ptr<Dataset>*)datasetPtr;
    std::shared_ptr<TF>* res = nullptr;

    //Create the correct transfer function
    if(enableGradient)
    {
        TriangularGTF* gtf = new TriangularGTF((*dataset)->getPointFieldDescs().size()+1, (ColorMode)colorMode);
        res = new std::shared_ptr<TF>(gtf);
    }

    else
    {
        GTF* gtf = new GTF((*dataset)->getPointFieldDescs().size(), (ColorMode)colorMode);
        res = new std::shared_ptr<TF>(gtf);
    }

    return (long)res;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_GTFData_nativeUpdateRanges(JNIEnv* jenv, jobject instance, jlong ptr, jlong datasetPtr, jboolean enableGradient, jint jcolorMode,
                                                                              jintArray jpIDs, jfloatArray jcenters, jfloatArray jscale)
{
    if(!ptr)
        return;

    std::shared_ptr<Dataset>* dataset = (std::shared_ptr<Dataset>*)datasetPtr;

    //Parse java values
    int*   pIDs = jenv->GetIntArrayElements(jpIDs, 0);
    float* centerPID = jenv->GetFloatArrayElements(jcenters, 0);
    float* scalePID  = jenv->GetFloatArrayElements(jscale,   0);

    int size = jenv->GetArrayLength(jpIDs);

    float* center = (float*)malloc(sizeof(float)*size);
    float* scale  = (float*)malloc(sizeof(float)*size);

    for(int i = 0; i < size; i++)
    {
        uint32_t tfID = (*dataset)->getTFIndiceFromPointFieldID(pIDs[i]);
        if(tfID != (uint32_t)-1 && tfID < size)
        {
            center[tfID] = centerPID[i];
            scale[tfID] = scalePID[i];
        }

        else
        {
            LOG_INFO("Error, cannot update TF");
            goto freeObj;
        }
    }

    //Update transfer function
    if(enableGradient)
    {
        std::shared_ptr<TriangularGTF>* gtf = (std::shared_ptr<TriangularGTF>*)ptr;
        (*gtf)->setCenter(center);
        (*gtf)->setScale(scale);
    }

    else
    {
        std::shared_ptr<GTF>* gtf = (std::shared_ptr<GTF>*)ptr;
        (*gtf)->setCenter(center);
        (*gtf)->setScale(scale);
    }

//Free variables
freeObj:
    jenv->ReleaseIntArrayElements(jpIDs, pIDs, JNI_ABORT);
    jenv->ReleaseFloatArrayElements(jcenters, centerPID, JNI_ABORT);
    jenv->ReleaseFloatArrayElements(jscale, scalePID, JNI_ABORT);

    free(center);
    free(scale);
}

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_TF_MergeTFData_nativeCreatePtr(JNIEnv* jenv, jobject instance, jfloat t, jlong tf1Ptr, jlong tf2Ptr)
{
    std::shared_ptr<TF>* tf1 = (std::shared_ptr<TF>*)tf1Ptr;
    std::shared_ptr<TF>* tf2 = (std::shared_ptr<TF>*)tf2Ptr;
    MergeTF* tf = new MergeTF(*tf1, *tf2, t);
    return (jlong)(new std::shared_ptr<MergeTF>(tf));
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_TF_MergeTFData_nativeSetInterpolationParameter(JNIEnv* jenv, jobject instance, jlong ptr, jfloat t)
{
    std::shared_ptr<MergeTF>* tf = (std::shared_ptr<MergeTF>*)ptr;
    if(tf)
        (*tf)->setInterpolationParameter(t);
}
