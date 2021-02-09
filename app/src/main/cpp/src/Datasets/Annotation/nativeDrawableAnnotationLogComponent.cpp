#include "Datasets/Annotation/nativeDrawableAnnotationLogComponent.h"
#include "Datasets/Annotation/DrawableAnnotationLogComponent.h"
#include <memory>
#include <glm/gtc/type_ptr.hpp>

using namespace sereno;

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeDelPtr(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<DrawableAnnotationLogComponent<AnnotationLogComponent>>* ptr = (std::shared_ptr<DrawableAnnotationLogComponent<AnnotationLogComponent>>*)jptr;
    delete ptr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeSetEnableTime(JNIEnv* jenv, jclass jcls, jlong jptr, jboolean t)
{
    std::shared_ptr<DrawableAnnotationLogComponent<AnnotationLogComponent>>* ptr = (std::shared_ptr<DrawableAnnotationLogComponent<AnnotationLogComponent>>*)jptr;
    (*ptr)->setEnableTime(t);
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeGetEnableTime(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<DrawableAnnotationLogComponent<AnnotationLogComponent>>* ptr = (std::shared_ptr<DrawableAnnotationLogComponent<AnnotationLogComponent>>*)jptr;
    return (*ptr)->getEnableTime();
}
