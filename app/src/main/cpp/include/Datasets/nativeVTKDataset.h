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

    /* \brief Function called from Java in order to get the Point field value indice in the VTKParser bound to this Dataset
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param vtkPtr the VTK Dataset native ptr (see nativeInitPtr)
     * \param valuePtr the ValueFieldValue native ptr to compare
     * \return -1 on error, otherwise the indice on this VTKFieldValue in the function VTKParser::getPointFieldValueDescriptors*/
    JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_VTKDataset_nativeGetPtFieldValueIndice(JNIEnv* env, jobject instance, jlong vtkPtr, jlong valuePtr);

    /* \brief Function called from Java in order to get the Cell field value indice in the VTKParser bound to this Dataset
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param vtkPtr the VTK Dataset native ptr (see nativeInitPtr)
     * \param valuePtr the ValueFieldValue native ptr to compare
     * \return -1 on error, otherwise the indice on this VTKFieldValue in the function VTKParser::getCellFieldValueDescriptors*/
    JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_VTKDataset_nativeGetCellFieldValueIndice(JNIEnv* env, jobject instance, jlong vtkPtr, jlong valuePtr);
}

#endif
