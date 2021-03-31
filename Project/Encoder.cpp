#include "Encoder.h"
#include <iostream>
using namespace std;
Encoder::Encoder(Router* router_) {
    router = router_;
}
void Encoder::start() {
    EncodeConfig config;
    if (init(config) < 0) {
        cout << "encoder init failed.\n";
        stop();
    }
}
bool Encoder::wait() {
    return getSize() == 0;
}
void Encoder::process() {
    cv::Mat frame;
    frame = getData();
    if (encode(frame)) {
        H264 h264;
        h264.size = avPacket->size;
        h264.data = new uint8_t[avPacket->size];
        if (h264.data) {
            memcpy(h264.data, avPacket->data, h264.size);
            router->receive(h264);
        }
        else {
            cout << "allocate packet failed.\n";
        }
    }
    else {
        cout << "encode failed.\n";
    }
}
void Encoder::end() {
    if (avContext) {
        avcodec_free_context(&avContext);
    }
    if (avFrame) {
        av_frame_free(&avFrame);
    }
    if (avPacket) {
        av_packet_free(&avPacket);
    }
    while (getSize()) {
        getData();
    }
}
int Encoder::init(EncodeConfig config) {
    AVCodec* avCoder;
    avCoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!avCoder) {
        return -1;
    }
    avContext = avcodec_alloc_context3(avCoder);
    if (!avContext) {
        return -1;
    }
    avContext->width = config.width;
    avContext->height = config.height;
    avContext->time_base = { 1, config.frame_rate };
    avContext->framerate = { config.frame_rate, 1 };
    avContext->gop_size = config.gop_size;
    avContext->max_b_frames = config.max_b_frames;
    avContext->pix_fmt = AV_PIX_FMT_YUV420P;
    avContext->codec_id = AV_CODEC_ID_H264;
    avContext->codec_type = AVMEDIA_TYPE_VIDEO;
    AVDictionary* dict = nullptr;
    av_dict_set(&dict, "preset", "veryfast", 0);
    av_dict_set(&dict, "tune", "zerolatency", 0);
    av_dict_set(&dict, "profile", "main", 0);
    avFrame = av_frame_alloc();
    avPacket = av_packet_alloc();
    if (!avFrame || !avPacket) {
        return -1;
    }
    av_init_packet(avPacket);
    avFrame->format = avContext->pix_fmt;
    avFrame->width = avContext->width;
    avFrame->height = avContext->height;
    if (av_frame_get_buffer(avFrame, 0) < 0) {
        return -1;
    }
    if (av_frame_make_writable(avFrame) < 0) {
        return -1;
    }
    frameIndex = 0;
    return avcodec_open2(avContext, avCoder, &dict);
}
AVPacket* Encoder::encode(const cv::Mat& src) {
    if (src.empty()) {
        return NULL;
    }
    cv::Mat temp, dist;
    cv::resize(src, temp, cv::Size(avContext->width, avContext->height));
    cv::cvtColor(temp, dist, cv::COLOR_BGR2YUV_I420);
    avFrame->data[0] = dist.data;
    avFrame->data[1] = dist.data + avContext->width * avContext->height;
    avFrame->data[2] = dist.data + avContext->width * avContext->height * 5 / 4;
    avFrame->pts = frameIndex++;
    if (avcodec_send_frame(avContext, avFrame) >= 0) {
        if (avcodec_receive_packet(avContext, avPacket) == 0) {
            avPacket->stream_index = avFrame->pts;
            return avPacket;
        }
    }
    return NULL;
}