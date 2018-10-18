#include "nativeGLSurfaceView.h"

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_gl_GLSurfaceView_nativeInitInternalData(JNIEnv* jenv, jobject jobj, jstring dataPath)
{
    const char* cDataPath = jenv->GetStringUTFChars(dataPath, 0);
    jenv->ReleaseStringUTFChars(dataPath, cDataPath);

    GLSurfaceViewData* data = new GLSurfaceViewData(cDataPath);
    return (jlong)data;
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeMain(JNIEnv* jenv, jobject jobj,
                                                                        jlong data, jstring mainLibrary, jstring mainFunction, jlong arg)
{
    //Load parameters
    const char*        libraryFile = jenv->GetStringUTFChars(mainLibrary, 0);
    const char*        cMainFunc   = jenv->GetStringUTFChars(mainFunction, 0);


    std::string       fullLibraryName = "lib" + std::string(libraryFile) + std::string(".so");
    void*             libraryHandle   = dlopen(fullLibraryName.c_str(), RTLD_GLOBAL);
    GLSurfaceViewMain mainFunc        = NULL;

    if(!libraryHandle)
    {
        LOG_ERROR("Could not load the library %s\n", fullLibraryName.c_str());
        goto error;
    }

    //Call the main function
    mainFunc = (GLSurfaceViewMain)dlsym(libraryHandle, cMainFunc);
    if(mainFunc)
    {
        mainFunc((GLSurfaceViewData*)data, (void*)arg);
    }
    else
    {
        LOG_ERROR("Error at loading main function called %s", cMainFunc);
        goto error;
    }

error:
    jenv->ReleaseStringUTFChars(mainLibrary,  libraryFile);
    jenv->ReleaseStringUTFChars(mainFunction, cMainFunc);
    if(libraryHandle)
        dlclose(libraryHandle);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnSurfaceCreated(JNIEnv* jenv, jobject jobj, 
                                                                                    jlong data, jobject jNativeWindow)
{
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(jenv, jNativeWindow);
    GLSurfaceViewData* pData    = (GLSurfaceViewData*)data;
    pData->renderer.createSurface(nativeWindow);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnSurfaceChanged(JNIEnv* jenv, jobject jobj, 
                                                                                jlong data, jint jformat, jint width, jint height)
{
    glViewport(0, 0, width, height);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnSurfaceDestroyed(JNIEnv* jenv, jobject jobj, 
                                                                                  jlong data)
{
    GLSurfaceViewData* pData    = (GLSurfaceViewData*)data;
    pData->renderer.destroySurface();
}
