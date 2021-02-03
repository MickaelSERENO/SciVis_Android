#include "Datasets/Annotation/nativeAnnotationLogContainer.h"
#include "Datasets/Annotation/AnnotationLogContainer.h"
#include <memory>

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeInitPtr(JNIEnv* jenv, jclass jcls, jboolean hasHeaders)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = new std::shared_ptr<AnnotationLogContainer>(new AnnotationLogContainer(hasHeaders));
    return (jlong)ptr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    delete ptr;
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeParseCSV(JNIEnv* jenv, jclass jcls, jlong jptr, jstring path)
{
    const char* cPath = jenv->GetStringUTFChars(path, 0);
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    bool res = (*ptr)->readFromCSV(cPath);
    jenv->ReleaseStringUTFChars(path, cPath);
    return res;
}

JNIEXPORT jobjectArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeGetHeaders(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;

    jobjectArray ret = jenv->NewObjectArray((*ptr)->getHeaders().size(), jenv->FindClass("java/lang/String"), NULL);
    for(uint32_t i=0; i<(*ptr)->getHeaders().size(); i++)
        jenv->SetObjectArrayElement(ret, i, jenv->NewStringUTF((*ptr)->getHeaders()[i].c_str()));

    return ret;
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeSetTimeHeaderInt(JNIEnv* jenv, jclass jcls, jlong jptr, int header)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    return (*ptr)->setTimeInd(header);
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeSetTimeHeaderString(JNIEnv* jenv, jclass jcls, jlong jptr, jstring header)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    const char* cHeader = jenv->GetStringUTFChars(header, 0);
    bool res = (*ptr)->setTimeHeader(std::string(cHeader));
    jenv->ReleaseStringUTFChars(header, cHeader);
    return res;
}

JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeGetNbColumns(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    return (*ptr)->getNbColumns();
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeHasHeaders(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    return (*ptr)->hasHeader();
}

JNIEXPORT jintArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeGetRemainingHeaders(JNIEnv* jenv, jclass jcls, jlong jptr)
{

    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    std::vector<uint32_t> remaining((*ptr)->getRemainingHeaders());
    jintArray arr = jenv->NewIntArray(remaining.size());
    jenv->SetIntArrayRegion(arr, 0, remaining.size(), (jint*)remaining.data());
    return arr;
}

JNIEXPORT jintArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeGetConsumedHeaders(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    const std::vector<uint32_t>& consumed = (*ptr)->getAssignedHeaders();
    jintArray arr = jenv->NewIntArray(consumed.size());
    jenv->SetIntArrayRegion(arr, 0, consumed.size(), (jint*)consumed.data());
    return arr;
}

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativeInitAnnotationPosition(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogContainer>* ptr = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    return (jlong)(new std::shared_ptr<AnnotationPosition>((*ptr)->buildAnnotationPositionView()));
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogContainer_nativePushAnnotationPosition(JNIEnv* jenv, jclass jcls, jlong jptr, jlong jposPtr)
{
    std::shared_ptr<AnnotationLogContainer>* ptr    = (std::shared_ptr<AnnotationLogContainer>*)jptr;
    std::shared_ptr<AnnotationPosition>*     posPtr = (std::shared_ptr<AnnotationPosition>*)jposPtr;

    return (*ptr)->parseAnnotationPosition(*posPtr) == 0;
}
