#include "nativeVFVData.h"

#include <memory>
#include <vector>
#include "VFVData.h"
#include "jniData.h"
#include "HeadsetStatus.h"
#include "utils.h"

using namespace sereno;

static std::vector<jobject> jniGetSubDatasets(JNIEnv* env, jobject dataset)
{
    std::vector<jobject> subDatasets;
    jint nbSubDataset = env->CallIntMethod(dataset, jDataset_getNbSubDataset);

    for(int i = 0; i < nbSubDataset; i++)
    {
        jobject cur = env->CallObjectMethod(dataset, jDataset_getSubDataset, i);
        subDatasets.push_back(env->NewGlobalRef(cur));
        env->DeleteLocalRef(cur);
    }

    return subDatasets;
}

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

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeChangeCurrentSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr)
{
    VFVData* data = (VFVData*)ptr;
    data->setCurrentSubDataset((SubDataset*)sdPtr);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddBinaryDataset(JNIEnv* env, jobject instance, jobject bd, jlong ptr, jlong jData)
{
    VFVData* data = (VFVData*)ptr;
    std::vector<jobject> subDatasets = jniGetSubDatasets(env, bd);
    data->addBinaryData(*((std::shared_ptr<BinaryDataset>*)jData), subDatasets);
}


JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx)
{
    VFVData* data = (VFVData*)ptr;
    data->removeData(dataIdx);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddVTKDataset(JNIEnv* env, jobject instance, jobject vtk, jlong ptr, jlong jData)
{
    VFVData* data = (VFVData*)ptr;
    std::vector<jobject> subDatasets = jniGetSubDatasets(env, vtk);
    data->addVTKData(*((std::shared_ptr<VTKDataset>*)jData), subDatasets);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnClampingChange(JNIEnv* env, jobject instance, jlong ptr, jfloat min, jfloat max, jlong sd)
{
    VFVData* data = (VFVData*)ptr;
    data->onClampingChange(min, max, (SubDataset*)sd);
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
    if(jheadsetsStatus == NULL)
        return;

    LOG_INFO("HEADSET entry");
    VFVData* data = (VFVData*)ptr;

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
    int headsetID = env->CallIntMethod(info, jHeadsetBindingInfoMessage_getHeadsetID);

    VFVData* data = (VFVData*)ptr;
    data->setHeadsetID(headsetID);
}
