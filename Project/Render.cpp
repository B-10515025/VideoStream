#include "Render.h"
void Render::start() {
	srand(time(NULL));
	name = to_string(rand());
}
bool Render::wait() {
	return getSize() == 0;
}
void Render::process() {
	shared_ptr<cv::Mat> frame;
	frame = getData();
	cv::imshow(name, *frame);
	cv::waitKey(1);
}
void Render::end() {
	while (getSize()) {
		getData();
	}
}