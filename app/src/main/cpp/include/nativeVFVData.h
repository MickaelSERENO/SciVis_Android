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
     * \param sdPtr the SubDataset native pointer to display*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeChangeCurrentSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr);

    /* \brief Function called from Java in order to add a new binary data in the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param jData BinaryDataset*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddBinaryDataset(JNIEnv* env, jobject instance, jobject jbd, jlong ptr, jlong jData);

    /* \brief Function called from Java in order to add a new VTK dataset in the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param jData the VTKDataset native pointer*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddVTKDataset(JNIEnv* env, jobject instance, jobject jvtk, jlong ptr, jlong jData);

    /* \brief Function called from Java in order to remove an existing data on the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param dataIdx the data index to remove*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx);

    /* \brief Function called from Java in order to change the displayed clipping range
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param min the minimum range
     * \param max the maximum range
     * \param sd the SubDataset being modified*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnClampingChange(JNIEnv* env, jobject instance, jlong ptr, jfloat min, jfloat max, jlong sd);

    /* \brief Function called from Java in order to update the view from a SubDataset rotation
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param sd the SubDataset being modified*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnRotationChange(JNIEnv* env, jobject instance, jlong ptr, jlong sd);

    /* \brief Function called from Java in order to update the view from a SubDataset position
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param sd the SubDataset being modified*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnPositionChange(JNIEnv* env, jobject instance, jlong ptr, jlong sd);

    /* \brief Function called from Java in order to update the view from a SubDataset scaling
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param sd the SubDataset being modified*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnScaleChange(JNIEnv* env, jobject instance, jlong ptr, jlong sd);

    /* \brief Function called from Java in order to update the current headsets status known by the native code
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param headsetsStatus the headsets status*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeUpdateHeadsetsStatus(JNIEnv* env, jobject instance, jlong ptr, jobjectArray headsetsStatus);

    /* \brief  Function called from Java in order to update the current binding information between this device and the associated headset
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param info the binding information*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeUpdateBindingInformation(JNIEnv* env, jobject instance, jlong ptr, jobject info);

    /* \brief  Function called from Java in order to initialize subdataset meta data
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param publicSD the SubDataset public states
     * \param privateSD the SubDataset private states
     * \param visibility the current SubDataset visibility (see visibility.h)*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeInitSubDatasetMetaData(JNIEnv* env, jobject instance, jlong ptr, jlong publicSD, jobject publicJObjectSD, jlong privateSD, jobject privateJObjectSD, int visibility);

    /* \brief  Function called from Java in order to set the subdataset visibility
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param sdPtr the SubDataset public or private states native pointer
     * \param visibility the new SubDataset visibility (see visibility.h)*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeSetSubDatasetVisibility(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr, int visibility);

    /* \brief  Poll a native event from C++ for the UI
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \return native pointer to pass to NativeEvent java constructor*/
    JNIEXPORT jlong JNICALL_Java_com_sereno_gl_VFVSurfaceView_nativePollEvent(JNIEnv* env, jobject instance, jlong ptr);
}

#endif
