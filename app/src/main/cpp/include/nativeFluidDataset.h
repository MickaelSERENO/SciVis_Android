#ifndef  NATIVEFLUIDDATASET_INC
#define  NATIVEFLUIDDATASET_INC

#include <jni.h>
#include <FluidDataset.h>

using namespace sereno;

extern "C"
{
    /* \brief Create a FluidDataset object
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param path the path to read at containing the data
     * \return the pointer as a long */
	JNIEXPORT jlong JNICALL Java_com_sereno_vfs_Data_FluidDataset_nativeInitPtr(JNIEnv* jenv, jobject jobj, jstring path);

    /* \brief Delete a FluidDataset object owned by Java
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param ptr the FluidDataset ptr */
	JNIEXPORT jlong JNICALL Java_com_sereno_vfs_Data_FluidDataset_nativeDelPtr(JNIEnv* jenv, jobject jobj, jlong ptr);

    /* \brief Get the size of the dataset
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param jptr the FluidDataset C++ pointer
     * \return the size as an array (array.length == 3) */
    JNIEXPORT jintArray JNICALL Java_com_sereno_vfs_Data_FluidDataset_nativeGetSize(JNIEnv* jenv, jobject jobj, jlong   jptr);
}

#endif   /* ----- #ifndef NATIVEFLUIDDATASET_INC  ----- */
