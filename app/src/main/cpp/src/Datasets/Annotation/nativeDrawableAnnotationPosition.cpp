#include "Datasets/Annotation/nativeDrawableAnnotationPosition.h"
#include "Datasets/Annotation/DrawableAnnotationPosition.h"
#include <glm/gtc/type_ptr.hpp>

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationPosition_nativeInitPtr(JNIEnv* jenv, jclass jcls, jlong jcontainerPtr, jlong jposPtr)
{
    std::shared_ptr<AnnotationLogContainer>*     ctn = (std::shared_ptr<AnnotationLogContainer>*)jcontainerPtr;
    std::shared_ptr<AnnotationPosition>*         pos = (std::shared_ptr<AnnotationPosition>*)jposPtr;
    std::shared_ptr<DrawableAnnotationPosition>* ptr = new std::shared_ptr<DrawableAnnotationPosition>(new DrawableAnnotationPosition(*ctn, *pos));

    return (jlong)ptr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationPosition_nativeSetColor(JNIEnv* jenv, jclass jcls, jlong jptr, jfloat r, jfloat g, jfloat b, jfloat a)
{
    std::shared_ptr<DrawableAnnotationPosition>* ptr = (std::shared_ptr<DrawableAnnotationPosition>*)jptr;
    (*ptr)->setColor(glm::vec4(r, g, b, a));
}

JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationPosition_nativeGetColor(JNIEnv* jenv, jclass jcls, jlong jptr)
{

    std::shared_ptr<DrawableAnnotationPosition>* ptr = (std::shared_ptr<DrawableAnnotationPosition>*)jptr;
    jfloatArray arr = jenv->NewFloatArray(4);
    jenv->SetFloatArrayRegion(arr, 0, 4, glm::value_ptr((*ptr)->getColor()));
    return arr;
}
