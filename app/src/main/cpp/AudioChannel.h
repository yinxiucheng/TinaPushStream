//
// Created by xiucheng yin on 2018/10/31.
//

#ifndef TINAPUSHSTREAM_AUDIOCHANNEL_H
#define TINAPUSHSTREAM_AUDIOCHANNEL_H


#include <sys/types.h>
#include "faac.h"
#include "librtmp/rtmp.h"

class AudioChannel {
    typedef void (*AudioCallback)(RTMPPacket *packet);

public:
    AudioChannel();

    ~AudioChannel();

    void setAudioEncInfo(int samplesInHZ, int channels);

    void setAudioCallback(AudioCallback audioCallback);

    int getInputSamples();

    void encodeData(int8_t *data);

    RTMPPacket* getAudioTag();
private:
    AudioCallback audioCallback;
    int mChannels;
    faacEncHandle audioCodec = 0;
    u_long inputSamples;
    u_long maxOutputBytes;
    u_char *buffer = 0;
};


#endif //TINAPUSHSTREAM_AUDIOCHANNEL_H
