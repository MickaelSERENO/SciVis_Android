#ifndef  UTILS_INC
#define  UTILS_INC

#include <strings.h>
#include <android/log.h>
#include <cstring>

#ifndef __FILENAME__
#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)
#endif

#ifndef LOG_TAG
/* \brief The LOG_TAG being displayed in the android logging system. Can be defined before including this file (or using #undef and #define */
#define LOG_TAG "Vector_Field_Visualization" /* <The log tag for android logcat if no default LOG_TAG*/
#endif

/* \brief print an INFO in android logger
 * It will use LOG_TAG as the tag (can be defined before importing this file)
 * \param __VA_ARGS__ the arguments to print */
#define LOG_INFO(fmt, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, "%s:%d " fmt, __FILENAME__, __LINE__, ## __VA_ARGS__) /* <Print an info  on the android log */

/* \brief print a DEBUG in android logger
 * It will use LOG_TAG as the tag (can be defined before importing this file)
 * \param __VA_ARGS__ the arguments to print */
#define LOG_ERROR(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s:%d " fmt, __FILENAME__, __LINE__, ## __VA_ARGS__) /* <Print an error on the android log */
#define LOG_DEBUG(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s:%d " fmt, __FILENAME__, __LINE__, ## __VA_ARGS__) /* <Print a  debug on the android log */


/* \brief Set a field of a Java Object from C++ value
 * Before calling it, pay attention to look at if jenv is defined (JNIEnv*) 
 * \param obj the jobject
 * \param name the field name (Java field name)
 * \param type the type field name
 * \param signature the signature of the type. Basically it is the same as the type of type != object
 * \param value the new value*/
#define SET_JNI_FIELD(obj, name, type, signature, value) \
	jfieldID name##_fid = jenv->GetFieldID(cls, #name, signature); \
	env->Set##type##Field(obj, name##_fid, value);

/* \brief Get a field from a Java Object to a C++ value
 * Before calling it, pay attention to look at if jenv is defined (JNIEnv*) 
 * \param obj the jobject
 * \param name the field name (Java field name)
 * \param type the type field name
 * \param signature the signature of the type. Basically it is the same as the type of type != object
 * \param value[out] variable to store the java value*/
#define GET_JNI_FIELD(obj, name, type, signature, value) \
	jfieldID name##_fid = jenv->GetFieldID(cls, #name, signature); \
	value = env->Get##type##Field(obj, name##_fid);

/* \brief Convert a uint8 ptr (4 value) to a uint32_t
 * \param data the uint8_t ptr
 * \return the uint32_t */
inline uint32_t uint8ToUint32(uint8_t* data)
{
    return (data[0] << 24) + (data[1] << 16) +
           (data[2] << 8 ) + (data[3]);
}

/* \brief Convert a uint8 ptr (4 value) to a float
 * \param data the uint8_t ptr
 * \return the float */
inline float uint8ToFloat(uint8_t* data)
{
    uint32_t t = uint8ToUint32(data);
    return *(float*)&t;
}

#endif
