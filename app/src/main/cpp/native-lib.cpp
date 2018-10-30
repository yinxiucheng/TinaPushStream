#include <jni.h>
#include <string>
#include "librtmp/rtmp.h"
#include <x264.h>
extern "C" JNIEXPORT jstring

JNICALL
Java_com_tina_pushstream_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    RTMP_Alloc();
    x264_picture_t *p = new x264_picture_t;
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_tina_pushstream_live_LivePusher_native_1init(JNIEnv *env, jobject instance) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_com_tina_pushstream_live_LivePusher_native_1start(JNIEnv *env, jobject instance,
                                                       jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    // TODO

    env->ReleaseStringUTFChars(path_, path);
}