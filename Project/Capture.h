#pragma once
#include "Component.h"
#include "Render.h"
#include "Encoder.h"
#include <opencv2/opencv.hpp>
class Capture: public Component<int> {
public:
	Capture(Render*, Encoder*);
protected:
	void start();
	bool wait();
	void process();
	void end();
private:
	Render* render;
	Encoder* encoder;
	cv::VideoCapture videoCapture;
	string name;
};