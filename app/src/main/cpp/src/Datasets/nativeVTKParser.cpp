#include "Datasets/nativeVTKParser.h"

#include <memory>
#include "utils.h"
#include "VTKParser.h"

using namespace sereno;

/**
 * \brief Create a jlongArray (java long array) from a vector of VTKFieldValue pointers
 * \param jenv the JNI environment variable
 * \param fieldValues the array of field values to convert
 * \return java jlong array containing pointer to const VTKFieldValue*
 */
jlongArray fieldValuesToJLongArray(JNIEnv* jenv, const std::vector<const VTKFieldValue*>& fieldValues)
{
    jlongArray arr = jenv->NewLongArray(fieldValues.size());
    auto data = fieldValues.data();
    for(uint32_t i = 0; i < fieldValues.size(); i++)
        jenv->SetLongArrayRegion(arr, i, 1, (const jlong*)(data+i));
    return arr;
}

JNIEXPORT long JNICALL Java_com_sereno_vfv_Data_VTKParser_nativeInitPtr(JNIEnv* jenv, jclass jcls, jstring path)
{
    const char* cPath = jenv->GetStringUTFChars(path, NULL);
    auto ptr = new std::shared_ptr<VTKParser>(new VTKParser(cPath));
    if(!(*ptr)->parse())
        LOG_ERROR("Could not parse %s file\n", cPath);
    jenv->ReleaseStringUTFChars(path, cPath);

    return (jlong)ptr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_VTKParser_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong ptr)
{
    auto p = (std::shared_ptr<VTKParser>*)(ptr);
    delete p;
}

JNIEXPORT jlongArray JNICALL Java_com_sereno_vfv_Data_VTKParser_nativeGetPointFieldValues(JNIEnv* jenv, jclass jcls, jlong ptr)
{
    std::shared_ptr<VTKParser>* p = (std::shared_ptr<VTKParser>*)(ptr);
    std::vector<const VTKFieldValue*> fieldValues = (*p)->getPointFieldValueDescriptors();

    return fieldValuesToJLongArray(jenv, fieldValues);
}

JNIEXPORT jlongArray JNICALL Java_com_sereno_vfv_Data_VTKParser_nativeGetCellFieldValues(JNIEnv* jenv, jclass jcls, jlong ptr)
{
    std::shared_ptr<VTKParser>* p = (std::shared_ptr<VTKParser>*)(ptr);
    std::vector<const VTKFieldValue*> fieldValues = (*p)->getCellFieldValueDescriptors();

    return fieldValuesToJLongArray(jenv, fieldValues);
}

JNIEXPORT jstring JNICALL Java_com_sereno_vfv_Data_VTKFieldValue_nativeGetName(JNIEnv* jenv, jclass jcls, jlong ptr)
{
    return jenv->NewStringUTF(((const VTKFieldValue*)(ptr))->name.c_str());
}
