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

    /* \brief  Get the number of sub datasets the Dataset "ptr" owns
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param ptr the Dataset ptr 
     * \return  ptr->getNbSubDatasets() */
	JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_Dataset_nativeGetNbSubDatasets(JNIEnv* jenv, jclass jcls, jlong ptr);

    /* \brief  Get the native pointer of the SubDataset #i
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param ptr the Dataset ptr 
     * \param i the indice of the subdataset prompted
     * \return the native pointer of the SubDataset. 0 if i is invalid*/
    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Dataset_nativeGetSubDataset(JNIEnv* jenv, jclass jcls, jlong ptr, jint i);

    /* \brief  Remove a subdataset in a given dataset
     * \param jenv the JNIEnvironment
     * \param jobj the Java Object calling this function
     * \param ptr the Dataset ptr 
     * \param sdPtr the native subdataset pointer*/
    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Dataset_nativeRemoveSubDataset(JNIEnv* jenv, jclass jcls, jlong ptr, jlong sdPtr);
}

#endif   /* ----- #ifndef NATIVEDATASET_INC  ----- */
