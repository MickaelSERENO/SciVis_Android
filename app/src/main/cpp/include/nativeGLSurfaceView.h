#ifndef  NATIVEGLSURFACEVIEW_INC
#define  NATIVEGLSURFACEVIEW_INC

#include <jni.h>
#include <dlfcn.h>
#include <unistd.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <string>
#include "utils.h"
#include "GLSurfaceViewData.h"

using namespace sereno;

extern "C"
{
    /* \brief Prototype of the main function
     * \param surfaceData the data handled both by Java and C++
     * \param arg the data sent by Java.*/
    typedef void(*GLSurfaceViewMain)(GLSurfaceViewData* surfaceData, void* arg);

    /* \brief Initialize the internal data state of the native thread handling the GLSurfaceView
     * \param jenv the JNI environment variable
     * \param jobj the java object
     * \return the GLSurfaceViewData as a jlong */
    JNIEXPORT jlong JNICALL Java_com_sereno_gl_GLSurfaceView_nativeInitInternalData(JNIEnv* jenv, jobject jobj);

    /* \brief Initialize the internal data state of the native thread handling the GLSurfaceView
     * \param jenv the JNI environment variable
     * \param jobj the java object
     * \param data the data shared by GLSurfaceView and the application. See GLSurfaceViewData
     * \param mainLibrary the name of the main library
     * \param mainFunction the name of the main function
     * \param arg the argument to pass to the main function*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeMain(JNIEnv* jenv, jobject jobj, jlong data, 
                                                                            jstring mainLibrary, jstring mainFunction, jlong arg);

    /* \brief Initialize the internal data state of the native thread handling the GLSurfaceView
     * \param jenv the JNI environment variable
     * \param jobj the java object
     * \param data the data shared by GLSurfaceView and the application. See GLSurfaceViewData
     * \param jNativeWindow the native windows from android*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnSurfaceCreated(JNIEnv* jenv, jobject jobj, 
                                                                                        jlong data, jobject jNativeWindow);

    /* \brief Initialize the internal data state of the native thread handling the GLSurfaceView
     * \param jenv the JNI environment variable
     * \param jobj the java object
     * \param data the data shared by GLSurfaceView and the application. See GLSurfaceViewData*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnSurfaceDestroyed(JNIEnv* jenv, jobject jobj,
                                                                                      jlong data);

    /* \brief Initialize the internal data state of the native thread handling the GLSurfaceView
     * \param jenv the JNI environment variable
     * \param jobj the java object
     * \param data the data shared by GLSurfaceView and the application. See GLSurfaceViewData
     * \param jformat the surface format
     * \param width the surface width
     * \param height the surface height*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnSurfaceChanged(JNIEnv* jenv, jobject jobj, 
                                                                                    jlong data, jint jformat, jint width, jint height);
}

#endif
