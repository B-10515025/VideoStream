#pragma once
#include "Component.h"
#include <mutex>
#include <queue>
#include <opencv2/opencv.hpp>
class Render: public Component<cv::Mat> {
protected:
	void start();
	bool wait();
	void process();
	void end();
private:
	string name;
};
