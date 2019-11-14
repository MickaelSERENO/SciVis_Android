#include "jniData.h"
#include <iostream>

namespace sereno
{
    JavaVM*   javaVM                                  = NULL;
    JNIEnv*   jniMainThread                           = NULL;

    jclass    jVFVSurfaceViewClass                    = 0;
    jmethodID jVFVSurfaceView_setCurrentAction        = 0;

    jclass    jBitmapClass                            = 0;
    jmethodID jBitmap_createBitmap                    = 0;

    jclass    jBitmapConfigClass                      = 0;
    jfieldID  jBitmapConfig_ARGB                      = 0;
    jobject   jBitmapConfigARGB                       = 0;

    jclass    jDatasetClass                           = 0;
    jmethodID jDataset_getNbSubDataset                = 0;
    jmethodID jDataset_getSubDataset                  = 0;
    jmethodID jDataset_onLoadDataset                  = 0;
    jmethodID jDataset_onLoadCPCPTexture              = 0;

    jclass    jSubDatasetClass                        = 0;
    jmethodID jSubDataset_setRotation                 = 0;
    jmethodID jSubDataset_setPosition                 = 0;
    jmethodID jSubDataset_setScale                    = 0;
    jmethodID jSubDataset_onSnapshotEvent             = 0;

    jclass    jHeadsetStatusClass                     = 0;
    jfieldID  jHeadsetStatus_position                 = 0;
    jfieldID  jHeadsetStatus_rotation                 = 0;
    jfieldID  jHeadsetStatus_id                       = 0;
    jfieldID  jHeadsetStatus_color                    = 0;
    jfieldID  jHeadsetStatus_currentAction            = 0;

    jclass    jHeadsetBindingInfoMessageClass         = 0;
    jmethodID jHeadsetBindingInfoMessage_getHeadsetID = 0;

    jclass    jPointFieldDescClass                    = 0;
    jmethodID jPointFieldDesc_constructor             = 0;
}

using namespace sereno;

jclass getJNIClassGlobalReference(JNIEnv* env, const char* name)
{
    jclass cls = env->FindClass(name);
    jclass res = (jclass)env->NewGlobalRef(cls);
    env->DeleteLocalRef(cls);
    return res;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    sereno::javaVM = vm;

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return -1;

    //Load classes
    jVFVSurfaceViewClass            = getJNIClassGlobalReference(env, "com/sereno/gl/VFVSurfaceView");
    jBitmapClass                    = getJNIClassGlobalReference(env, "android/graphics/Bitmap");
    jBitmapConfigClass              = getJNIClassGlobalReference(env, "android/graphics/Bitmap$Config");
    jDatasetClass                   = getJNIClassGlobalReference(env, "com/sereno/vfv/Data/Dataset");
    jSubDatasetClass                = getJNIClassGlobalReference(env, "com/sereno/vfv/Data/SubDataset");
    jHeadsetStatusClass             = getJNIClassGlobalReference(env, "com/sereno/vfv/Network/HeadsetsStatusMessage$HeadsetStatus");
    jHeadsetBindingInfoMessageClass = getJNIClassGlobalReference(env, "com/sereno/vfv/Network/HeadsetBindingInfoMessage");
    jPointFieldDescClass            = getJNIClassGlobalReference(env, "com/sereno/vfv/Data/PointFieldDesc");

    //Load methods
    jVFVSurfaceView_setCurrentAction = env->GetMethodID(jVFVSurfaceViewClass, "setCurrentAction", "(I)V");

    jDataset_getNbSubDataset    = env->GetMethodID(jDatasetClass, "getNbSubDataset",      "()I");
    jDataset_getSubDataset      = env->GetMethodID(jDatasetClass, "getSubDataset",        "(I)Lcom/sereno/vfv/Data/SubDataset;");
    jDataset_onLoadDataset      = env->GetMethodID(jDatasetClass, "onLoadDataset", "(Z)V");
    jDataset_onLoadCPCPTexture  = env->GetMethodID(jDatasetClass, "onLoadCPCPTexture", "(Landroid/graphics/Bitmap;II)V");

    jSubDataset_setRotation     = env->GetMethodID(jSubDatasetClass, "setRotation", "([F)V");
    jSubDataset_setPosition     = env->GetMethodID(jSubDatasetClass, "setPosition", "([F)V");
    jSubDataset_setScale        = env->GetMethodID(jSubDatasetClass, "setScale",    "([F)V");
    jSubDataset_onSnapshotEvent = env->GetMethodID(jSubDatasetClass, "onSnapshotEvent", "(Landroid/graphics/Bitmap;)V");

    jHeadsetBindingInfoMessage_getHeadsetID = env->GetMethodID(jHeadsetBindingInfoMessageClass, "getHeadsetID", "()I");

    jPointFieldDesc_constructor = env->GetMethodID(jPointFieldDescClass, "<init>", "(IFFZ)V");

    //Load fields
    jBitmapConfig_ARGB   = env->GetStaticFieldID(jBitmapConfigClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
    jBitmap_createBitmap = env->GetStaticMethodID(jBitmapClass, "createBitmap", "([IIILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    jHeadsetStatus_position      = env->GetFieldID(jHeadsetStatusClass, "position",      "[F");
    jHeadsetStatus_rotation      = env->GetFieldID(jHeadsetStatusClass, "rotation",      "[F");
    jHeadsetStatus_id            = env->GetFieldID(jHeadsetStatusClass, "id",            "I");
    jHeadsetStatus_color         = env->GetFieldID(jHeadsetStatusClass, "color",         "I");
    jHeadsetStatus_currentAction = env->GetFieldID(jHeadsetStatusClass, "currentAction", "I");
    
    //Load static object
    jobject bmpConfARGB = env->GetStaticObjectField(jBitmapConfigClass, jBitmapConfig_ARGB);
    jBitmapConfigARGB   = env->NewGlobalRef(bmpConfARGB);
    env->DeleteLocalRef(bmpConfARGB);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv* env;
    if(vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return;

    //Delete global references
    env->DeleteGlobalRef(jVFVSurfaceViewClass);
    env->DeleteGlobalRef(jBitmapClass);
    env->DeleteGlobalRef(jBitmapConfigClass);
    env->DeleteGlobalRef(jDatasetClass);
    env->DeleteGlobalRef(jSubDatasetClass);
    env->DeleteGlobalRef(jBitmapConfigARGB);
    env->DeleteGlobalRef(jHeadsetStatusClass);
    env->DeleteGlobalRef(jHeadsetBindingInfoMessageClass);
    env->DeleteGlobalRef(jPointFieldDescClass);
}


JNIEnv* getJNIEnv(bool* shouldDetach)
{
    *shouldDetach = false;
    JNIEnv* env;
    int status = javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if(status == JNI_EDETACHED)
    {
        if (javaVM->AttachCurrentThread(&env, NULL) != 0) 
        {
            std::cerr << "Failed to attach" << std::endl;
            return NULL;
        }
        *shouldDetach = true;
        return env;
    }

    else if(status == JNI_OK)
        return env;

    else
    {
        std::cerr << "Error at getting the JNI Environment. Error: " << status << std::endl;
        return NULL;
    }
}

jobject createjARGBBitmap(uint32_t* pixels, uint32_t width, uint32_t height, JNIEnv* env)
{
    //Get the java environment if needed
    bool shouldDetach;
    if(env == NULL)
        env = getJNIEnv(&shouldDetach);

    //Create the java array
    uint32_t  size   = width * height;
    jintArray jPixels = env->NewIntArray(size);

    jint* jPixelsPtr  = env->GetIntArrayElements(jPixels, 0);

    //Transform RGBA to ARGB
    for(uint32_t j = 0; j < height; j++)
        for(uint32_t i = 0; i < width; i++)
        {
            uint32_t ind = i+(height-1-j)*width;
            uint8_t a = (pixels[ind] >> 24);
            uint8_t b = (pixels[ind] >> 16);
            uint8_t g = (pixels[ind] >> 8);
            uint8_t r =  pixels[ind];

            jPixelsPtr[i+j*width] = (a << 24) + (r << 16) +
                                          (g << 8)  + b;
        }

    env->ReleaseIntArrayElements(jPixels, (jint*)jPixelsPtr, 0);

    //Create and fill the bitmap
    jobject  bmp = env->CallStaticObjectMethod(jBitmapClass, jBitmap_createBitmap, jPixels, width, height, jBitmapConfigARGB);

    env->DeleteLocalRef(jPixels); //Delete local reference

    if(shouldDetach)
        javaVM->DetachCurrentThread();

    return bmp;
}