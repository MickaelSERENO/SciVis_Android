#include "Datasets/Annotation/nativeAnnotationLogComponent.h"
#include "Datasets/Annotation/AnnotationLogComponent.h"
#include <memory>

using namespace sereno;

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogComponent_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogComponent>* ptr = (std::shared_ptr<AnnotationLogComponent>*)jptr;
    delete ptr;
}

JNIEXPORT jintArray JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationLogComponent_nativeGetHeaders(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<AnnotationLogComponent>* ptr = (std::shared_ptr<AnnotationLogComponent>*)jptr;

    std::vector<int32_t> res((*ptr)->getHeaders());
    jintArray arr = jenv->NewIntArray(res.size());
    jenv->SetIntArrayRegion(arr, 0, res.size(), (jint*)res.data());

    return arr;
}
