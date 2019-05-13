#include "Datasets/nativeSubDataset.h"
#include "Datasets/SubDataset.h"
#include "jniData.h"
#include "utils.h"

using namespace sereno;

JNIEXPORT bool JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeIsValid(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    return sd->isValid();
}

JNIEXPORT jfloat JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetMinClampingColor(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    return sd->getMinClamping();
}

JNIEXPORT jfloat JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetMaxClampingColor(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    return sd->getMaxClamping();
}

JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetColorMode(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    if(sd->getTransferFunction())
        return sd->getTransferFunction()->getColorMode();
    return RAINBOW;
}

JNIEXPORT jfloat JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetMinAmplitude(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    return sd->getMinAmplitude();
}

JNIEXPORT jfloat JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetMaxAmplitude(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    return sd->getMaxAmplitude();
}

JNIEXPORT jobject JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetSnapshot(JNIEnv* env, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    Snapshot* snap = sd->getSnapshot();
    if(snap == NULL)
        return NULL;

    //Create the java array
    uint32_t  size   = snap->width * snap->height;
    jintArray pixels = env->NewIntArray(size);

    jint* pixelsPtr  = env->GetIntArrayElements(pixels, 0);

    //Transform RGBA to ARGB
    for(uint32_t j = 0; j < snap->height; j++)
        for(uint32_t i = 0; i < snap->width; i++)
        {
            uint32_t ind = i+(snap->height-1-j)*snap->width;
            uint8_t a = (snap->pixels[ind] >> 24);
            uint8_t b = (snap->pixels[ind] >> 16);
            uint8_t g = (snap->pixels[ind] >> 8);
            uint8_t r =  snap->pixels[ind];

            pixelsPtr[i+j*snap->width] = (a << 24) + (r << 16) +
                                         (g << 8)  + b;
        }
    env->ReleaseIntArrayElements(pixels, (jint*)pixelsPtr, 0);

    //Create and fill the bitmap
    jobject  bmp = env->CallStaticObjectMethod(jBitmapClass, jBitmap_createBitmap, pixels, snap->width, snap->height, jBitmapConfigARGB);

    env->DeleteLocalRef(pixels); //Delete local reference
    return bmp;
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

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetClamping(JNIEnv* jenv, jobject jobj, jlong ptr, jfloat min, jfloat max)
{
    SubDataset* sd = (SubDataset*)ptr;
    sd->setClamping(min, max);
}

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeClone(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    return (jlong)(new SubDataset(*(SubDataset*)ptr));
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeFree(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    delete ((SubDataset*)(ptr));
}
