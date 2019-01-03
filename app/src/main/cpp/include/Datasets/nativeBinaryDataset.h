#ifndef  NATIVEBINARYDATASET_INC
#define  NATIVEBINARYDATASET_INC

#include <jni.h>
#include <Datasets/BinaryDataset.h>

using namespace sereno;

extern "C"
{
    /* \brief Create a BinaryDataset object
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param path the path to read at containing the data
     * \return the pointer as a long */
	JNIEXPORT jlong JNICALL Java_com_sereno_vfs_Data_BinaryDataset_nativeInitPtr(JNIEnv* jenv, jclass jcls, jstring path);

    /* \brief Get the size of the dataset
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param jptr the BinaryDataset C++ pointer
     * \return the size as an array (array.length == 3) */
    JNIEXPORT jintArray JNICALL Java_com_sereno_vfs_Data_BinaryDataset_nativeGetSize(JNIEnv* jenv, jclass jcls, jlong jptr);
}

#endif   /* ----- #ifndef NATIVEBINARYDATASET_INC  ----- */
