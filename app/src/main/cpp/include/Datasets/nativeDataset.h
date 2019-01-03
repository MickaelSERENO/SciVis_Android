#ifndef  NATIVEDATASET_INC
#define  NATIVEDATASET_INC

#include "Datasets/Dataset.h"
#include <jni.h>

using namespace sereno;

extern "C"
{
    /* \brief Delete a BinaryDataset object owned by Java
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param ptr the Dataset ptr */
	JNIEXPORT void JNICALL Java_com_sereno_vfs_Data_Dataset_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong ptr);
}

#endif   /* ----- #ifndef NATIVEDATASET_INC  ----- */
