//
// Created by xiucheng yin on 2018/10/30.
//

#ifndef TINAPUSHSTREAM_VIDEOCHANNEL_H
#define TINAPUSHSTREAM_VIDEOCHANNEL_H

#include <inttypes.h>
#include <x264.h>
#include <pthread.h>
#include "librtmp/rtmp.h"


class VideoChannel {

    typedef void (*VideoCallback)(RTMPPacket *packet);

public:
    VideoChannel();

    ~VideoChannel();

    //创建x264编码器
    void setVideoEncInfo(int width, int height, int fps, int bitrate);

    void encodeData(int8_t *data);

    void setVideoCallback(VideoCallback callback);

private:
    pthread_mutex_t mutex;
    int mWidth;
    int mHeight;
    int mFps;
    int mBitrate;
    x264_t *videoCodec = 0;
    //图片
    x264_picture_t *pic_in = 0;
    int ySize;
    int uvSize;
    int index = 0;
    VideoCallback callback;

    void sendSpsPps(uint8_t *sps, uint8_t *pps, int len, int pps_len);

    void sendFrame(int type, int payload, uint8_t *p_payload);
};


#endif //TINAPUSHSTREAM_VIDEOCHANNEL_H
