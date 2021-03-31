#pragma once
#include "Component.h"
#include "Decoder.h"
#include <vector>
#include <Winsock2.h>
class Controller;
class Router: public Component<H264> {
public:
    Router(Decoder*, Controller*);
    bool setPort(int);
    bool link(string, int, int);
protected:
    void start();
    bool wait();
    void process();
    void end();
private:
    Decoder* decoder;
    Controller* controller;
    atomic_bool living;
    SOCKET upStream, entry;
    vector<SOCKET> downStream;
    mutex vecMtx;
    void server();
};