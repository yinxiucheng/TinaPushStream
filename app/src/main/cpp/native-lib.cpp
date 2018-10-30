#include <jni.h>
#include <string>
#include "librtmp/rtmp.h"
extern "C" JNIEXPORT jstring

JNICALL
Java_com_tina_pushstream_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    RTMP_Alloc();
    return env->NewStringUTF(hello.c_str());
}
