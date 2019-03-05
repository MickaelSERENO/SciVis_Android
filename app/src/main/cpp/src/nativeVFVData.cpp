#include "nativeVFVData.h"

#include <memory>
#include "VFVData.h"
#include "jniData.h"

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

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeSetCurrentData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx)
{
    VFVData* data = (VFVData*)ptr;
    data->setCurrentData(dataIdx);
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

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnRangeColorChange(JNIEnv* env, jobject instance, jlong ptr, jfloat min, jfloat max, jint mode, jlong sd)
{
    VFVData* data = (VFVData*)ptr;
    data->onRangeColorChange(min, max, (ColorMode)(mode), (SubDataset*)sd);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnRotationChange(JNIEnv* env, jobject instance, jlong ptr, jlong sd)
{
    VFVData* data = (VFVData*)ptr;
    data->onRotationChange((SubDataset*)sd);
}
