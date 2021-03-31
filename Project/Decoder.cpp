#include "Decoder.h"
Decoder::Decoder(Render* render_) {
    render = render_;
}
void Decoder::start() {
    if (init() < 0) {
        cout << "decoder init failed.\n";
        stop();
    }
}
bool Decoder::wait() {
    return getSize() == 0;
}
void Decoder::process() {
    cv::Mat frame;
    H264 h264;
    h264 = getData();
    if (decode(h264.data, h264.size, frame) == 0) {
        render->receive(frame);
        delete[] h264.data;
    }
    else {
        cout << "decode failed.\n";
    }
}
void Decoder::end() {
    avcodec_close(avContext);
    av_free(avContext);
    av_frame_free(&avFrame);
    while (getSize()) {
        getData();
    }
}
int Decoder::init()
{
    AVCodec* avCoder;
    avCoder = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!avCoder) {
        return -1;
    }
    avContext = avcodec_alloc_context3(avCoder);
    if (!avContext) {
        return -1;
    }
    avContext->pix_fmt = AV_PIX_FMT_YUV420P;
    avContext->codec_id = AV_CODEC_ID_H264;
    avContext->codec_type = AVMEDIA_TYPE_VIDEO;
    avFrame = av_frame_alloc();
    avPacket = av_packet_alloc();
    if (!avFrame || !avPacket) {
        return -1;
    }
    av_init_packet(avPacket);
    return avcodec_open2(avContext, avCoder, NULL);
}
int Decoder::decode(uint8_t* data, int size, cv::Mat& dist)
{
    if (size <= 0) {
        return -1;
    }
    avPacket->data = data;
    avPacket->size = size;
    if (avcodec_send_packet(avContext, avPacket) >= 0) {
        if (avcodec_receive_frame(avContext, avFrame) == 0) {
            dist.create(avFrame->height * 3 / 2, avFrame->width, CV_8UC1);
            memcpy(dist.data, avFrame->data[0], avFrame->width * avFrame->height);
            memcpy(dist.data + avFrame->width * avFrame->height, avFrame->data[1], avFrame->width * avFrame->height / 4);
            memcpy(dist.data + avFrame->width * avFrame->height * 5 / 4, avFrame->data[2], avFrame->width * avFrame->height / 4);
            cv::cvtColor(dist, dist, cv::COLOR_YUV2BGR_I420);
            return 0;
        }
    }
    return -1;
}