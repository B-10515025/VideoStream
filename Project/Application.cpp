#include "Application.h"
#include <iostream>
#include <string>
using namespace std;
void Application::start() {
	controller.init();
	string cmd, ip;
	int port, outPort;
	cout << "command: ";
	while (cin >> cmd) {
		if (cmd == "live") {
			cout << "port: ";
			cin >> port;
			controller.live(port);
		}
		else if (cmd == "watch") {
			cout << "ip: ";
			cin >> ip;
			cout << "port: ";
			cin >> port;
			cout << "out port: ";
			cin >> outPort;
			controller.watch(ip, port, outPort);
		}
		else if (cmd == "leave") {
			controller.leave();
		}
		else {
			cout << "command not found.\n";
		}
		cout << "command: ";
	}
}