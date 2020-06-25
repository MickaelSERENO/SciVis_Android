#include "nativeVFVData.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "VFVData.h"
#include "jniData.h"
#include "HeadsetStatus.h"
#include "utils.h"

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeCreateMainArgs(JNIEnv *env, jobject instance)
{
    VFVData* data = new VFVData(env->NewGlobalRef(instance));
    return (jlong)data;
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeDeleteMainArgs(JNIEnv *env, jobject instance, jlong ptr)
{
    VFVData* data = (VFVData*)ptr;
    env->DeleteGlobalRef(data->getJavaObj());
    delete data;
}

JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnSetSelection(JNIEnv* env, jobject instance, jlong ptr, jboolean s)
{
    VFVData* data = (VFVData*)ptr;
    data->setSelection(s);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeChangeCurrentSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr)
{
    VFVData* data = (VFVData*)ptr;
    data->setCurrentSubDataset((SubDataset*)sdPtr);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddVectorFieldDataset(JNIEnv* env, jobject instance, jobject bd, jlong ptr, jlong jData)
{
    VFVData* data = (VFVData*)ptr;
    data->addVectorFieldData(*((std::shared_ptr<VectorFieldDataset>*)jData), bd);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddCloudPointDataset(JNIEnv* env, jobject instance, jobject cp, jlong ptr, jlong jData)
{
    VFVData* data = (VFVData*)ptr;
    data->addCloudPointData(*((std::shared_ptr<CloudPointDataset>*)jData), cp);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr)
{
    VFVData* data = (VFVData*)ptr;
    data->onRemoveSubDataset((SubDataset*)sdPtr);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveDataset(JNIEnv* env, jobject instance, jlong ptr, jlong datasetPtr, jint datasetType)
{
    VFVData* data = (VFVData*)ptr;
    if(datasetType == DATASET_TYPE_VTK)
        data->onRemoveDataset(*((std::shared_ptr<VTKDataset>*)datasetPtr));
    else if(datasetType == DATASET_TYPE_VECTOR_FIELD)
        data->onRemoveDataset(*((std::shared_ptr<VectorFieldDataset>*)datasetPtr));
    else if(datasetType == DATASET_TYPE_CLOUD_POINT)
        data->onRemoveDataset(*((std::shared_ptr<CloudPointDataset>*)datasetPtr));
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddVTKDataset(JNIEnv* env, jobject instance, jobject vtk, jlong ptr, jlong jData)
{
    VFVData* data = (VFVData*)ptr;
    data->addVTKData(*((std::shared_ptr<VTKDataset>*)jData), vtk);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnSetLocation(JNIEnv* env, jobject instance, jlong ptr, jfloatArray jPos, jfloatArray jRot)
{
    VFVData* data = (VFVData*)ptr;
    jfloat* jPosArr = env->GetFloatArrayElements(jPos, 0);
    jfloat* jRotArr = env->GetFloatArrayElements(jRot, 0);
    glm::vec3 pos;
    Quaternionf rot;
    pos.x = jPosArr[0];
    pos.y = jPosArr[1];
    pos.z = jPosArr[2];
    rot.x = jRotArr[0];
    rot.y = jRotArr[1];
    rot.z = jRotArr[2];
    rot.w = jRotArr[3];
    data->onSetLocation(pos, rot);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnSetTabletScale(JNIEnv* env, jobject instance, jlong ptr, jfloat scale, jfloat width, jfloat height, jfloat posx, jfloat posy)
{
    VFVData* data = (VFVData*)ptr;
    data->onSetTabletScale(scale, width, height, posx, posy);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnRotationChange(JNIEnv* env, jobject instance, jlong ptr, jlong sd)
{
    VFVData* data = (VFVData*)ptr;
    data->onRotationChange((SubDataset*)sd);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnPositionChange(JNIEnv* env, jobject instance, jlong ptr, jlong sd)
{
    VFVData* data = (VFVData*)ptr;
    data->onPositionChange((SubDataset*)sd);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnScaleChange(JNIEnv* env, jobject instance, jlong ptr, jlong sd)
{
    VFVData* data = (VFVData*)ptr;
    data->onScaleChange((SubDataset*)sd);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeUpdateHeadsetsStatus(JNIEnv* env, jobject instance, jlong ptr, jobjectArray jheadsetsStatus)
{
    VFVData* data = (VFVData*)ptr;

    if(jheadsetsStatus == NULL)
    {
        std::vector<HeadsetStatus>* hs = new std::vector<HeadsetStatus>();
        data->updateHeadsetsStatus(std::shared_ptr<std::vector<HeadsetStatus>>(hs));
        return;
    }

    jsize nbHS = env->GetArrayLength(jheadsetsStatus);
    std::vector<HeadsetStatus>* hs = new std::vector<HeadsetStatus>(nbHS);

    for(jsize i = 0; i < nbHS; i++)
    {
        //Fetch java object
        jobject jstatus = env->GetObjectArrayElement(jheadsetsStatus, i);

        jfloatArray jPos = (jfloatArray)env->GetObjectField(jstatus, jHeadsetStatus_position);
        jfloatArray jRot = (jfloatArray)env->GetObjectField(jstatus, jHeadsetStatus_rotation);

        jfloat* jPosArr = env->GetFloatArrayElements(jPos, 0);
        jfloat* jRotArr = env->GetFloatArrayElements(jRot, 0);

        //Set values
        HeadsetStatus current;
        (*hs)[i].id            = env->GetIntField(jstatus, jHeadsetStatus_id);
        (*hs)[i].color         = env->GetIntField(jstatus, jHeadsetStatus_color);
        (*hs)[i].currentAction = (HeadsetCurrentAction)env->GetIntField(jstatus, jHeadsetStatus_currentAction);

        //Position
        for(int j = 0; j < 3; j++)
            (*hs)[i].position[j] = jPosArr[j];
        //Rotation
        for(int j = 0; j < 4; j++)
            (*hs)[i].rotation[j] = jRotArr[j];
        LOG_INFO("Rotation : %f %f %f %f", jRotArr[0], jRotArr[1], jRotArr[2], jRotArr[3]);
        env->ReleaseFloatArrayElements(jPos, jPosArr, 0);
        env->ReleaseFloatArrayElements(jRot, jRotArr, 0);

        //Delete local references
        env->DeleteLocalRef(jPos);
        env->DeleteLocalRef(jRot);
        env->DeleteLocalRef(jstatus);

        hs->push_back(current);
    }

    data->updateHeadsetsStatus(std::shared_ptr<std::vector<HeadsetStatus>>(hs));
    LOG_INFO("HEADSET exit");
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeUpdateBindingInformation(JNIEnv* env, jobject instance, jlong ptr, jobject info)
{
    int headsetID = -1;
    if(info != NULL)
        headsetID = env->CallIntMethod(info, jHeadsetBindingInfoMessage_getHeadsetID);

    VFVData* data = (VFVData*)ptr;
    data->setHeadsetID(headsetID);
}

JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeBindSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr, jobject javaSD)
{
    VFVData* data = (VFVData*)ptr;
    SubDataset* sd = (SubDataset*)sdPtr;

    if(sd != NULL)
        data->bindSubDatasetJava(sd, javaSD);
}

JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnAddSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr)
{
    VFVData* data = (VFVData*)ptr;
    SubDataset* sd = (SubDataset*)sdPtr;

    if(sd != NULL)
        data->addSubDatasetFromJava(sd);
}

JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnTFUpdated(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr)
{
    VFVData* data = (VFVData*)ptr;
    SubDataset* sd = (SubDataset*)sdPtr;

    if(sd != NULL)
        data->onTFChange(sd);
}
