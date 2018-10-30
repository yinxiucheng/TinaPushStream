#include <jni.h>
#include <string>
#include "librtmp/rtmp.h"
#include <x264.h>
#include "macro.h"
#include "safe_queue.h"
#include "VideoChannel.h"


SafeQueue<RTMPPacket *> packets;

VideoChannel *videoChannel = 0;
uint32_t start_time;
int readyPushing = 0;

int isStart = 0;
pthread_t pid;

void releasePackets(RTMPPacket *&packet) {
    if (packet) {
        RTMPPacket_Free(packet);
        delete packet;
        packet = 0;
    }
}

void callback(RTMPPacket *packet) {
    if (packet) {
        //设置时间戳
        packet->m_nTimeStamp = RTMP_GetTime() - start_time;
        //这里往队列里 塞数据，在start中 pop取数据然后发出去
        packets.push(packet);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_tina_pushstream_live_LivePusher_native_1init(JNIEnv *env, jobject instance) {
    //准备一个Video编码器的工具类 ：进行编码
    videoChannel = new VideoChannel;
    videoChannel->setVideoCallback(callback);
    //准备一个队列,打包好的数据 放入队列，在线程中统一的取出数据再发送给服务器
    packets.setReleaseCallback(releasePackets);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_tina_pushstream_live_LivePusher_native_1setVideoEncInfo(JNIEnv *env, jobject instance,
                                                                 jint width, jint height, jint fps,
                                                                 jint bitrate) {

    if (videoChannel) {
        videoChannel->setVideoEncInfo(width, height, fps, bitrate);
    }
}


void *start(void *args) {
    char *url = static_cast<char *>(args);
    RTMP *rtmp = 0;
    do {
        rtmp = RTMP_Alloc();
        if (!rtmp) {
            LOGE("rtmp创建失败");
            break;
        }
        RTMP_Init(rtmp);
        //设置超时时间 5s
        rtmp->Link.timeout = 5;
        int ret = RTMP_SetupURL(rtmp, url);
        if (!ret) {
            LOGE("rtmp设置地址失败:%s", url);
            break;
        }
        //开启输出模式
        RTMP_EnableWrite(rtmp);
        ret = RTMP_Connect(rtmp, 0);
        if (!ret) {
            LOGE("rtmp连接地址失败:%s", url);
            break;
        }
        ret = RTMP_ConnectStream(rtmp, 0);
        if (!ret) {
            LOGE("rtmp连接流失败:%s", url);
            break;
        }

        //准备好了 可以开始推流了
        readyPushing = 1;
        //记录一个开始推流的时间
        start_time = RTMP_GetTime();
        packets.setWork(1);
        RTMPPacket *packet = 0;
        //循环从队列取包 然后发送
        while (isStart) {
            packets.pop(packet);
            if (!isStart) {
                break;
            }
            if (!packet) {
                continue;
            }
            // 给rtmp的流id
            packet->m_nInfoField2 = rtmp->m_stream_id;
            //发送包 1:加入队列发送
            ret = RTMP_SendPacket(rtmp, packet, 1);
            releasePackets(packet);
            if (!ret) {
                LOGE("发送数据失败");
                break;
            }
        }
        releasePackets(packet);
    } while (0);
    if (rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
    delete url;
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tina_pushstream_live_LivePusher_native_1start(JNIEnv *env, jobject instance,
                                                       jstring path_) {
    if (isStart) {
        return;
    }
    const char *path = env->GetStringUTFChars(path_, 0);
    char *url = new char[strlen(path) + 1];
    strcpy(url, path);
    isStart = 1;
    //启动线程
    pthread_create(&pid, 0, start, url);

    env->ReleaseStringUTFChars(path_, path);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_tina_pushstream_live_LivePusher_native_1pushVideo(JNIEnv *env, jobject instance,
                                                           jbyteArray data_) {
    if (!videoChannel || !readyPushing) {
        return;
    }
    jbyte *data = env->GetByteArrayElements(data_, NULL);

    videoChannel->encodeData(data);

    env->ReleaseByteArrayElements(data_, data, 0);
}