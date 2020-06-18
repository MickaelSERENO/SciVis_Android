#ifndef  NATIVECLOUDPOINTDATASET_INC
#define  NATIVECLOUDPOINTDATASET_INC

#include <jni.h>

extern "C"
{
    /* \brief Create a CloudPointDataset object
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param path the path to read at containing the data
     * \return the pointer as a long */
	JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_CloudPointDataset_nativeInitPtr(JNIEnv* jenv, jclass jcls, jstring path);
}

#endif
