#include "Datasets/Annotation/AnnotationPosition.h"
#include "Datasets/Annotation/nativeAnnotationPosition.h"
#include <memory>

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeInitPtr(JNIEnv* jenv, jclass jcls, jlong jannPtr)
{

    std::shared_ptr<AnnotationLog>*      ann = (std::shared_ptr<AnnotationLog>*)jannPtr;
    std::shared_ptr<AnnotationPosition>* ptr = new std::shared_ptr<AnnotationPosition>(new AnnotationPosition(ann->get()));
    return (jlong)ptr;
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeSetXYZHeaderInteger(JNIEnv* jenv, jclass jcls, jlong jptr, jint x, jint y, jint z)
{
    std::shared_ptr<AnnotationPosition>* ptr = (std::shared_ptr<AnnotationPosition>*)jptr;
    (*ptr)->setXYZIndices(x, y, z);
}

JNIEXPORT void JNICALL Java_com_sereno_vfv_Data_Annotation_AnnotationPosition_nativeSetXYZHeaderString(JNIEnv* jenv, jclass jcls, jlong jptr, jstring x, jstring y, jstring z)
{
    std::shared_ptr<AnnotationPosition>* ptr = (std::shared_ptr<AnnotationPosition>*)jptr;


    const char* cX = jenv->GetStringUTFChars(x, 0);
    const char* cY = jenv->GetStringUTFChars(y, 0);
    const char* cZ = jenv->GetStringUTFChars(z, 0);

    (*ptr)->setXYZHeaders(cX, cY, cZ);

    jenv->ReleaseStringUTFChars(x, cX);
    jenv->ReleaseStringUTFChars(y, cY);
    jenv->ReleaseStringUTFChars(z, cZ);
}
