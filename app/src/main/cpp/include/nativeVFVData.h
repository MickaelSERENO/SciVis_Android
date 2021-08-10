#ifndef NATIVEVFVDATA_INC
#define NATIVEVFVDATA_INC

#include <jni.h>

extern "C"
{
    enum {DATASET_TYPE_VTK = 0, DATASET_TYPE_VECTOR_FIELD = 1, DATASET_TYPE_CLOUD_POINT};

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

    JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnSetSelection(JNIEnv* env, jobject instance, jlong ptr, jboolean s);

    /* \brief Function called from Java in order to set the current data being displayed
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param sdPtr the SubDataset native pointer to display*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeChangeCurrentSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr);

    /* \brief Function called from Java in order to add a new vector field data in the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param jbd the java VectorField Dataset object
     * \param ptr the VFVData ptr
     * \param jData VectorFieldDataset*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddVectorFieldDataset(JNIEnv* env, jobject instance, jobject jbd, jlong ptr, jlong jData);

    /* \brief Function called from Java in order to add a new cloud point data in the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param jcp the java CloudPoint Dataset object
     * \param ptr the VFVData ptr
     * \param jData CloudPointDataset ptr*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddCloudPointDataset(JNIEnv* env, jobject instance, jobject jcp, jlong ptr, jlong jData);

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
     * \param sdPtr the subdataset ptr*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr);

    /* \brief Function called from Java in order to remove an existing data on the cpp memory application
     * Note that this function is asynchronous between the main cpp thread and the java UI thread
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param datasetPtr the dataset ptr
     * \param datasetType the type of the dataset (VTK, VectorField, etc.)*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveDataset(JNIEnv* env, jobject instance, jlong ptr, jlong datasetPtr, jint datasetType);

    /* \brief Function called from Java in order to update the tablet's location
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param pos the tablet's position
     * \param rot the tablet's rotation*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnSetLocation(JNIEnv* env, jobject instance, jlong ptr, jfloatArray jPos, jfloatArray jRot);
    
    /* \brief Function called from Java in order to update the tablet scale
     * \param env the JNI environment
     * \param instance the Java object calling this function*/
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnSetTabletScale(JNIEnv* env, jobject instance, jlong ptr, jfloat scale, jfloat width, jfloat height, jfloat posx, jfloat posy);

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

    /* \brief  Poll a native event from C++ for the UI
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \return native pointer to pass to NativeEvent java constructor*/
    JNIEXPORT jlong JNICALL Java_com_sereno_gl_VFVSurfaceView_nativePollEvent(JNIEnv* env, jobject instance, jlong ptr);

    /* \brief  Bind the SubDataset C++ object to its Java counter part
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param sdPtr the SubDataset C++ ptr to bind
     * \param javaSD the Java jobject to bind*/
    JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeBindSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr, jobject javaSD);

    /* \brief Function called when a new SubDataset object has been added via the Java interface
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param sdPtr the SubDataset C++ ptr to add*/
    JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnAddSubDataset(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr);

    /* \brief Function called when a Transfer Function of a SubDataset object has been updated
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the VFVData ptr
     * \param sdPtr the SubDataset C++ ptr associated*/
    JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnTFUpdated(JNIEnv* env, jobject instance, jlong ptr, jlong sdPtr);

    JNIEXPORT void JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnSetCurrentAction(JNIEnv* env, jobject instance, jlong ptr, jint action);
}

#endif
