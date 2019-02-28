#include "Datasets/nativeSubDataset.h"
#include "Datasets/SubDataset.h"

using namespace sereno;

JNIEXPORT bool JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeIsValid(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    SubDataset* sd = (SubDataset*)ptr;
    return sd->isValid();
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

    //Get JNI data
    jclass    bmpCls        = env->FindClass("android/graphics/Bitmap");
    jclass    bmpConfCls    = env->FindClass("androd/graphics/Bitmap$Config");
    jfieldID  bmpConfARGBID = env->GetStaticFieldID(bmpConfCls, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
    jobject   bmpConfARGB   = env->GetStaticObjectField(bmpConfCls, bmpConfARGBID);
    jmethodID createBmpID   = env->GetStaticMethodID(bmpCls, "createBitmap", "([IIILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    //Create the java array
    uint32_t  size   = snap->width * snap->height;
    jintArray pixels = env->NewIntArray(size);
    env->SetIntArrayRegion(pixels, 0, size, (const jint*)snap->pixels);
    
    //Create and fill the bitmap
    jobject   bmp = env->CallStaticObjectMethod(bmpCls, createBmpID, pixels, snap->width, snap->height, bmpConfARGB);

    return bmp;
}

JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeGetRotation(JNIEnv* jenv, jobject jobj, jlong ptr)
{
    jfloatArray arr = jenv->NewFloatArray(4);
    Quaternionf q   = ((SubDataset*)ptr)->getGlobalRotate();
    float qArr[4] = {q.w, q.x, q.y, q.z};
    jenv->SetFloatArrayRegion(arr, 0, 4, qArr);
    return arr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_SubDataset_nativeSetRangeColor(JNIEnv* jenv, jobject jobj, jlong ptr, jfloat min, jfloat max, jint mode)
{
    SubDataset* sd = (SubDataset*)ptr;
    sd->setColor(min, max, (ColorMode)mode);
}
