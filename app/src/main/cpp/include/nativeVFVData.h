#ifndef NATIVEVFVDATA_INC
#define NATIVEVFVDATA_INC

#include <jni.h>

extern "C"
{
    /* \brief Function called from Java in order to create the main arguments list that the main function will receive
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \return the Arguments as a ptr. True type : VFVData*/
    JNIEXPORT jlong JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeCreateMainArgs(JNIEnv *env, jobject instance);

    /* \brief Function called from Java in order to free the memory allocated
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the pointer to free*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeDeleteMainArgs(JNIEnv *env, jobject instance, jlong ptr);

    /* \brief Function called from Java in order to set the current data being displayed
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param dataIdx the data index to show on screen*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeSetCurrentData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx);

    /* \brief Function called from Java in order to add a new binary data in the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param jData BinaryDataset*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddBinaryDataset(JNIEnv* env, jobject instance, jlong ptr, jlong jData);

    /* \brief Function called from Java in order to add a new VTK dataset in the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param parserPtr the VTKParser native ptr
     * \param ptFieldValues the point VTKFieldValue to take account of
     * \param cellieldValues the cell VTKFieldValue to take account of*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddVTKDataset(JNIEnv* env, jobject instance, jlong ptr, jlong parserPtr, jlongArray ptFieldValues, jlongArray cellFieldValues);

    /* \brief Function called from Java in order to remove an existing data on the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param dataIdx the data index to remove*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx);

    /* \brief Function called from Java in order to change the displayed range color
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param min the minimum range
     * \param max the maximum range
     * \param mode the color mode to apply*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnRangeColorChange(JNIEnv* env, jobject instance, jlong ptr, jfloat min, jfloat max, jint mode);

    /* \brief Function called from Java in order to get a snapshot from the main rendering frame
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \return the android.graphics.Bitmap snapshot object */
    JNIEXPORT jobject JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeGetSnapshot(JNIEnv* env, jobject instance, jlong ptr);
}

#endif
