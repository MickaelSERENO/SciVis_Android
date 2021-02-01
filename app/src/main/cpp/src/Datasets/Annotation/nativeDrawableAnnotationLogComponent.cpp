#include "Datasets/Annotation/nativeDrawableAnnotationLogComponent.h"
#include "Datasets/Annotation/DrawableAnnotationLogComponent.h"
#include <memory>
#include <glm/gtc/type_ptr.hpp>

using namespace sereno;

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeSetEnableTime(JNIEnv* jenv, jclass jcls, jlong jptr, jboolean t)
{
    std::shared_ptr<DrawableAnnotationLogComponent>* ptr = (std::shared_ptr<DrawableAnnotationLogComponent>*)jptr;
    (*ptr)->setEnableTime(t);
}

JNIEXPORT jboolean JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeGetEnableTime(JNIEnv* jenv, jclass jcls, jlong jptr)
{
    std::shared_ptr<DrawableAnnotationLogComponent>* ptr = (std::shared_ptr<DrawableAnnotationLogComponent>*)jptr;
    return (*ptr)->getEnableTime();
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeSetColor(JNIEnv* jenv, jclass jcls, jlong jptr, jfloat r, jfloat g, jfloat b, jfloat a)
{

    std::shared_ptr<DrawableAnnotationLogComponent>* ptr = (std::shared_ptr<DrawableAnnotationLogComponent>*)jptr;
    (*ptr)->setColor(glm::vec4(r, g, b, a));
}

JNIEXPORT jfloatArray JNICALL Java_com_sereno_vfv_Data_Annotation_DrawableAnnotationLogComponent_nativeGetColor(JNIEnv* jenv, jclass jcls, jlong jptr)
{

    std::shared_ptr<DrawableAnnotationLogComponent>* ptr = (std::shared_ptr<DrawableAnnotationLogComponent>*)jptr;
    jfloatArray arr = jenv->NewFloatArray(4);
    jenv->SetFloatArrayRegion(arr, 0, 4, glm::value_ptr((*ptr)->getColor()));
    return arr;
}
