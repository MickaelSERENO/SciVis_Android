#ifndef  UTILS_INC
#define  UTILS_INC

#include <strings.h>
#include <android/log.h>
#include <jni.h>
#include <cstring>
#include <vector>

#define ENUM_BODY(name, value)                  \
    name value,

#define DEFINE_ENUM(name, list)                 \
    typedef enum name                           \
    {                                           \
        list(ENUM_BODY)                         \
    }name;

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
#define LOG_WARNING(fmt, ...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "%s:%d " fmt, __FILENAME__, __LINE__, ## __VA_ARGS__) /* <Print an error on the android log */
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

/**
 * \brief  Convert a jlongArray to a std::vector element
 * @tparam T the element in the jlongArray
 * \param env the jni environment
 * \param arr the array to read
 * \return   the std::vector<T*> object*/
template <typename T>
inline std::vector<T*> jlongArrayToVector(JNIEnv* env, jlongArray arr)
{
    T** values = (T**)env->GetLongArrayElements(arr, NULL);
    auto nbElem = env->GetArrayLength(arr);
    std::vector<T*> res(nbElem);
    for(int i = 0; i < nbElem; i++)
        res[i] = values[i];
    return res;
}

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


#if __cplusplus > 201703L
    /**
     * \brief  Create meta nested for loop. The most outer part of the for loop is at indice==Dim-1 
     *
     * @tparam Dim the dimension of the for loop
     * @tparam Callable the functor class callable
     * \param start array of where to start along each dimension
     * \param end array of where to finish along each dimension
     * \param c the function to call
     *
     * \return   
     */
    template<size_t Dim, class Callable>
    constexpr void metaForLoop(const size_t* start, const size_t* end, Callable&& c)
    {
        static_assert(Dim > 0);

        for(size_t i = start[Dim]; i != end[Dim]; i++)
        {
            if constexpr(Dim == 1)
                c(i);
            else
            {
                auto bindAnArgument = [i, &c](auto... args)
                {
                    c(i, args...);
                };
                meta_for_loop<Dim-1>(begin, end, bindAnArgument);
            }
        }
    }
#endif

#endif
