#include "Datasets/Annotation/nativeAnnotationLogComponent.h"
#include "Datasets/Annotation/AnnotationLogComponent.h"
#include <memory>

using namespace sereno;

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogComponent_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogComponent>* ptr = (std::shared_ptr<AnnotationLogComponent>*)jptr;
    delete ptr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogComponent_nativeSetEnableTime(JNIEnv* jenv, jclass jcls, jlong jptr, jboolean t)
{
    std::shared_ptr<AnnotationLogComponent>* ptr = (std::shared_ptr<AnnotationLogComponent>*)jptr;
    (*ptr)->setEnableTime(t);
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogComponent_nativeGetEnableTime(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogComponent>* ptr = (std::shared_ptr<AnnotationLogComponent>*)jptr;
    return (*ptr)->getEnableTime();
}
