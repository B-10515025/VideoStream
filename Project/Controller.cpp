#include "Controller.h"
void Controller::init() {
	render = new Render();
	decoder = new Decoder(render);
	router = new Router(decoder, this);
	encoder = new Encoder(router);
	capture = new Capture(render, encoder);
}
void Controller::live(int port) {
	if (router->setPort(port)) {
		cout << "start live.\n";
		render->run();
		decoder->stop();
		router->run();
		encoder->run();
		capture->run();
	}
}
void Controller::watch(string ip, int port, int outPort) {
	if (router->link(ip, port, outPort)) {
		cout << "start watch.\n";
		render->run();
		decoder->run();
		router->run();
		encoder->stop();
		capture->stop();
	}
}
void Controller::leave() {
	cout << "leave.\n";
	render->stop();
	decoder->stop();
	router->stop();
	encoder->stop();
	capture->stop();
}