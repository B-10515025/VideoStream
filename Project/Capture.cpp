#include "Capture.h"
#include <iostream>
#include <memory>
using namespace std;
Capture::Capture(Render* render_, Encoder* encoder_) {
	render = render_;
	encoder = encoder_;
}
void Capture::start() {
	videoCapture = cv::VideoCapture(0);
}
bool Capture::wait() {
	return false;
}
void Capture::process() {
	shared_ptr<cv::Mat> frame = make_shared<cv::Mat>();
	videoCapture >> *frame;
	if (!frame->empty()) {
		render->receive(frame);
		encoder->receive(frame);
	}
}
void Capture::end() {
	videoCapture.release();
}