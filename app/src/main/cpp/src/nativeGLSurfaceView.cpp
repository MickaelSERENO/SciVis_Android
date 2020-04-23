#include "nativeGLSurfaceView.h"

#include <dlfcn.h>
#include <unistd.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <string>
#include "utils.h"

JNIEXPORT jlong JNICALL Java_com_sereno_gl_GLSurfaceView_nativeInitInternalData(JNIEnv* jenv, jobject jobj, jstring dataPath)
{
    const char* cDataPath = jenv->GetStringUTFChars(dataPath, 0);

    GLSurfaceViewData* data = new GLSurfaceViewData(cDataPath);
    jenv->ReleaseStringUTFChars(dataPath, cDataPath);

    return (jlong)data;
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeMain(JNIEnv* jenv, jobject jobj,
                                                                        jlong data, jstring mainLibrary, jstring mainFunction, jobject jNativeWindow, jlong arg)
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
        ANativeWindow* nativeWindow = ANativeWindow_fromSurface(jenv, jNativeWindow);
        mainFunc((GLSurfaceViewData*)data, nativeWindow, (void*)arg);
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
    GLSurfaceViewData* pData = (GLSurfaceViewData*)data;
    pData->changeSurfaceSize(width, height);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnSurfaceDestroyed(JNIEnv* jenv, jobject jobj, 
                                                                                  jlong data)
{
    GLSurfaceViewData* pData    = (GLSurfaceViewData*)data;
    pData->renderer.destroySurface();
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnTouchEvent(JNIEnv* jenv, jobject jobj,
                                                                            jlong data, jint action, jint finger, jfloat x, jfloat y)
{
    GLSurfaceViewData* pData = (GLSurfaceViewData*)data;
    pData->touchEvent(action, finger, x, y);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnHidden(JNIEnv* jenv, jobject jobj, jlong data)
{
    GLSurfaceViewData* pData = (GLSurfaceViewData*)data;
    pData->visibilityEvent(false);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnVisible(JNIEnv* jenv, jobject jobj, jlong data)
{
    GLSurfaceViewData* pData = (GLSurfaceViewData*)data;
    pData->visibilityEvent(true);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnStartSelection(JNIEnv* jenv, jobject jobj, jlong data)
{
    GLSurfaceViewData* pData = (GLSurfaceViewData*)data;
    pData->selectionEvent(true);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeCloseEvent(JNIEnv* jenv, jobject jobj, jlong data)
{
    GLSurfaceViewData* pData = (GLSurfaceViewData*)data;
    pData->closeEvent();
}
