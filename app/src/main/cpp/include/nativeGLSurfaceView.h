#ifndef  NATIVEGLSURFACEVIEW_INC
#define  NATIVEGLSURFACEVIEW_INC

#include <jni.h>

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
     * \param dataPath the external data path
     * \return the GLSurfaceViewData as a jlong */
    JNIEXPORT jlong JNICALL Java_com_sereno_gl_GLSurfaceView_nativeInitInternalData(JNIEnv* jenv, jobject jobj, 
                                                                                    jstring dataPath);

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

    /* \brief Function called from Java when the Surface received a touch event
     * \param jenv the JNI environment variable
     * \param jobj the java object
     * \param data the data shared by GLSurfaceView and the application. See GLSurfaceViewData
     * \param action the type of the action (UP, DOWN, MOVE)
     * \param finger the finger ID touching
     * \param x the x position [-1, +1]
     * \param y the y position [-1, +1]*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnTouchEvent(JNIEnv* jenv, jobject jobj,
                                                                                jlong data, jint action, jint finger, jfloat x, jfloat y);

    /* \brief Function called from Java when the Surface is gone / invisible
     * \param jenv the JNI environment variable
     * \param jobj the java object
     * \param data the data shared by GLSurfaceView and the application. See GLSurfaceViewData*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnHidden(JNIEnv* jenv, jobject jobj, jlong data);

    /* \brief Function called from Java when the Surface is visible
     * \param jenv the JNI environment variable
     * \param jobj the java object
     * \param data the data shared by GLSurfaceView and the application. See GLSurfaceViewData*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeOnVisible(JNIEnv* jenv, jobject jobj, jlong data);

    /* \brief  Close the object
     * \param jenv the java environment
     * \param jobj the object calling this function
     * \param data the data shared by GLSurfaceView and the application. See GLSurfaceViewData */
    JNIEXPORT void  JNICALL Java_com_sereno_gl_GLSurfaceView_nativeCloseEvent(JNIEnv* jenv, jobject jobj, jlong data);
}

#endif
