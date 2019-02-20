#ifndef  NATIVENATIVEEVENT_INC
#define  NATIVENATIVEEVENT_INC

#include <jni.h>

extern "C"
{
    /** \brief Get the event ID
     * \param env the JNI environment
     * \param instance the Java object calling this function
     * \param ptr the NativeEvent ptr
     * \return long telling what is the type of this event*/
    JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Event_NativeEvent_nativeGetEventID(JNIEnv* env, jobject instance);
}

#endif
