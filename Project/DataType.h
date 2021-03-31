#pragma once
struct EncodeConfig {
    int width = 1280 * 0.5;
    int height = 720 * 0.5;
    int frame_rate = 30;
    int gop_size = 100;
    int max_b_frames = 0;
};
struct H264 {
    int size;
    uint8_t* data;
};
enum PacketType {
    CONNECT, VIDEO
};
struct Packet {
    PacketType type;
    int size;
    char* data;
};
