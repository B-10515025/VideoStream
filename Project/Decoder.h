#pragma once
#include "Component.h"
#include "Render.h"
#include "DataType.h"
#include <opencv2/opencv.hpp>
extern "C" {
#include "libavformat/avformat.h"
}
class Decoder: public Component<H264> {
public:
    Decoder(Render*);
public:
    void start();
    bool wait();
    void process();
    void end();
private:
    Render* render;
    AVCodecContext* avContext;
    AVFrame* avFrame;
    AVPacket* avPacket;
    int init();
    int decode(uint8_t*, int, cv::Mat&);
};