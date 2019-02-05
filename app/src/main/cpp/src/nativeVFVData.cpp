#include "nativeVFVData.h"

#include <memory>
#include "VFVData.h"

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeCreateMainArgs(JNIEnv *env, jobject instance)
{
    VFVData* data = new VFVData();
    return (jlong)data;
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeDeleteMainArgs(JNIEnv *env, jobject instance, jlong ptr)
{
    VFVData* data = new VFVData();
    delete data;
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeSetCurrentData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx)
{
    VFVData* data = (VFVData*)ptr;
    data->setCurrentData(dataIdx);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddBinaryDataset(JNIEnv* env, jobject instance, jlong ptr, jlong jData)
{
    VFVData* data = (VFVData*)ptr;
    data->addBinaryData(*((std::shared_ptr<BinaryDataset>*)jData));
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddVTKDataset(JNIEnv* env, jobject instance, jlong ptr, jlong parserPtr, jlongArray ptFieldValues, jlongArray cellFieldValues)
{
    VFVData* data = (VFVData*)ptr;
    std::shared_ptr<VTKParser>* parser = (std::shared_ptr<VTKParser>*)(parserPtr);
    data->addVTKData(std::shared_ptr<VTKDataset>(new VTKDataset(*parser, jlongArrayToVector<VTKFieldValue>(env, ptFieldValues), 
                                                                jlongArrayToVector<VTKFieldValue>(env, cellFieldValues)))); 
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx)
{
    VFVData* data = (VFVData*)ptr;
    data->removeData(dataIdx);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnRangeColorChange(JNIEnv* env, jobject instance, jlong ptr, jfloat min, jfloat max, jint mode)
{
    VFVData* data = (VFVData*)ptr;
    data->onRangeColorChange(min, max, (ColorMode)(mode));
}

JNIEXPORT jobject JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeGetSnapshot(JNIEnv* env, jobject instance, jlong ptr)
{
    VFVData* data = (VFVData*)ptr;

    //Get JNI data
    jclass    bmpCls        = env->FindClass("android/graphics/Bitmap");
    jclass    bmpConfCls    = env->FindClass("androd/graphics/Bitmap$Config");
    jfieldID  bmpConfARGBID = env->GetStaticFieldID(bmpConfCls, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
    jobject   bmpConfARGB   = env->GetStaticObjectField(bmpConfCls, bmpConfARGBID);
    jmethodID createBmpID   = env->GetStaticMethodID(bmpCls, "createBitmap", "([IIILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    //Create the bitmap
    data->lockSnapshot();
        //If no data, no object shall be returned
        if(data->getSnapshotPixels() == NULL)
            return NULL;

        //Create the java array
        uint32_t  size   = data->getSnapshotWidth()*data->getSnapshotHeight();
        jintArray pixels = env->NewIntArray(size);
        env->SetIntArrayRegion(pixels, 0, size, (const jint*)data->getSnapshotPixels());
        
        //Create and fill the bitmap
        jobject   bmp    = env->CallStaticObjectMethod(bmpCls, createBmpID, pixels, data->getSnapshotWidth(), data->getSnapshotHeight(), bmpConfARGB);
    data->unlockSnapshot();

    return bmp;
}
