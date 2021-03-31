#pragma once
#include "Render.h"
#include "Decoder.h"
#include "Router.h"
#include "Encoder.h"
#include "Capture.h"
class Controller {
public:
	void init();
	void live(int);
	void watch(string, int, int);
	void leave();
private:
	Render* render;
	Decoder* decoder;
	Router* router;
	Encoder* encoder;
	Capture* capture;
};