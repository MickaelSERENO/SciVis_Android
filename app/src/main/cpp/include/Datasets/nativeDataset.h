#ifndef  NATIVEDATASET_INC
#define  NATIVEDATASET_INC

#include <jni.h>

extern "C"
{
    /* \brief Delete a BinaryDataset object owned by Java
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param ptr the Dataset ptr */
	JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Dataset_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong ptr);
}

#endif   /* ----- #ifndef NATIVEDATASET_INC  ----- */
