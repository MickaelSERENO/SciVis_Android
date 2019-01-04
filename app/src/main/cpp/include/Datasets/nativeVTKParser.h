#ifndef  NATIVEVTKPARSER_INC
#define  NATIVEVTKPARSER_INC

#include <jni.h>

extern "C"
{
    /** \brief  Initialize the native pointer
     * \param jenv the JNI environment variable
     * \param jcls the Java class calling this object
     * \param path the VTK file object path
     *
     * \return   the native C++ handle (type : std::shared_ptr<VTKParser>)
     */
	JNIEXPORT long JNICALL Java_com_sereno_vfv_Data_VTKParser_nativeInitPtr(JNIEnv* jenv, jclass jcls, jstring path);

    /** \brief  Delete the native pointer
     * \param jenv the JNI environment variable
     * \param jcls the Java class calling this object
     * \param ptr the native C++ handle (type : std::shared_ptr<VTKParser>)
     */
    JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_VTKParser_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong ptr);

    /** \brief  Get the point field values for this dataset
     * \param jenv the JNI environment variable
     * \param jcls the Java class calling this object
     * \param ptr the native C++ handle (type : std::shared_ptr<VTKParser>)
     *
     * \return the jlongArray containing all the C++ handles of VTKFieldValue
     */
    JNIEXPORT jlongArray JNICALL Java_com_sereno_vfv_Data_VTKParser_nativeGetPointFieldValues(JNIEnv* jenv, jclass jcls, jlong ptr);

    /** \brief  Get the cell field values for this dataset
     * \param jenv the JNI environment variable
     * \param jcls the Java class calling this object
     * \param ptr the native C++ handle (type : std::shared_ptr<VTKParser>)
     *
     * \return the jlongArray containing all the C++ handles of VTKFieldValue
     */
    JNIEXPORT jlongArray JNICALL Java_com_sereno_vfv_Data_VTKParser_nativeGetCellFieldValues(JNIEnv* jenv, jclass jcls, jlong ptr);

    /** \brief  Get the name of a VTKFieldValue
     * \param jenv the JNI environment variable
     * \param jcls the Java class calling this object
     * \param ptr the native C++ handle (type : const VTKFieldValue)
     *
     * \return the jstring containin the VTKFieldValue name
     */
    JNIEXPORT jstring JNICALL Java_com_sereno_vfv_Data_VTKFieldValue_nativeGetName(JNIEnv* jenv, jclass jcls, jlong ptr);
}

#endif
