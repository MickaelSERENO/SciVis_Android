#ifndef  NATIVEVTKDATASET_INC
#define  NATIVEVTKDATASET_INC

#include <jni.h>

extern "C"
{
    /* \brief Function called from Java in order to add a new VTK dataset in the cpp memory application
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param parserPtr the VTKParser native ptr
     * \param ptFieldValues the point VTKFieldValue to take account of
     * \param cellieldValues the cell VTKFieldValue to take account of*/
    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_VTKDataset_nativeInitPtr(JNIEnv* env, jobject instance, jlong parserPtr, jlongArray ptFieldValues, jlongArray cellFieldValues);
}

#endif
