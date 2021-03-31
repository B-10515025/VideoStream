#include "Router.h"
#include "Controller.h"
Router::Router(Decoder* decoder_,Controller* controller_) {
    decoder = decoder_;
    controller = controller_;
    living = true;
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        cout << "WSAStartup failed.\n";
    }
}
bool Router::setPort(int port) {
    entry = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (entry < 0) {
        cout << "create socket failed.\n";
        return false;
    }
    SOCKADDR_IN addr;
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (::bind(entry, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "bind Socket failed.\n";
        closesocket(entry);
        return false;
    }
    return true;
}
bool Router::link(string ip, int port,int outPort) {
    upStream = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (upStream < 0) {
        cout << "create socket failed.\n";
        return false;
    }
    SOCKADDR_IN addr;
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    if (connect(upStream, (const sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "connect error.\n";
        closesocket(upStream);
        return false;
    }
    living = false;
    return setPort(outPort);
}
void Router::start() {
    lock_guard<mutex> lock(vecMtx);
    downStream.clear();
    thread t(&Router::server, this);
    t.detach();
}
bool Router::wait() {
    if (living) {
        return getSize() == 0;
    }
    else {
        H264 h264;
        if (recv(upStream, (char*)&h264.size, sizeof(h264.size), MSG_WAITALL)) {
            h264.data = new uint8_t[h264.size];
            if (recv(upStream, (char*)h264.data, h264.size, MSG_WAITALL)) {
                pushData(h264);
            }
            else {
                controller->leave();
            }
        }
        else {
            controller->leave();
        }
        return false;
    }
}
void Router::process() {
    lock_guard<mutex> lock(vecMtx);
    H264 h264;
    if (getSize()) {
        h264 = getData();
        char* buffer;
        buffer = new char[sizeof(h264.size) + h264.size];
        memcpy(buffer, &h264.size, sizeof(h264.size));
        memcpy(buffer + sizeof(h264.size), h264.data, h264.size);
        int i = 0, bytes;
        while (i < downStream.size()) {
            bytes = send(downStream[i], buffer, sizeof(h264.size) + h264.size, 0);
            if (bytes < 0) {
                downStream.erase(downStream.begin() + i);
                cout << "socket: " << downStream[i] << " disconnect.\n";
            }
            else {
                i++;
            }
        }
        decoder->receive(h264);
    }
}
void Router::end() {
    lock_guard<mutex> lock(vecMtx);
    living = true;
    closesocket(upStream);
    closesocket(entry);
    for (int i = 0; i < downStream.size(); i++) {
        closesocket(downStream[i]);
    }
    downStream.clear();
    while (getSize()) {
        getData();
    }
}
void Router::server() {
    SOCKADDR_IN addr;
    SOCKET down;
    int addrlen = sizeof(addr);
    listen(entry, 128);
    while (isActive()) {
        down = accept(entry, (struct sockaddr*)&addr, &addrlen);
        vecMtx.lock();
        downStream.push_back(down);
        vecMtx.unlock();
        cout << inet_ntoa(addr.sin_addr) << " connect." << endl;
    }
}