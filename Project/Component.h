#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <iostream>
using namespace std;
template<class queueType>
class Component {
public:
	void receive(queueType input) {
		if (active) {
			lock_guard<mutex> lock(sign);
			pushData(input);
			condition.notify_all();
		}
	}
	void run() {
		if (!active) {
			active = true;
			task = new thread(&Component::loop, this);
			task->detach();
		}
	}
	void stop() {
		if (active) {
			active = false;
			condition.notify_all();
		}
	}
	bool isActive() {
		return active;
	}
protected:
	void loop() {
		start();
		while (active) {
			unique_lock<mutex> lock(sign);
			while (wait() && active) {
				condition.wait(lock);
			}
			lock.unlock();
			if (active) {
				process();
			}
		}
		end();
		delete task;
	}
	void pushData(queueType input) {
		lock_guard<mutex> lock(mtx);
		data.push(input);
	}
	queueType getData() {
		lock_guard<mutex> lock(mtx);
		queueType ret;
		ret = data.front();
		data.pop();
		return ret;
	}
	int getSize() {
		lock_guard<mutex> lock(mtx);
		int ret;
		ret = data.size();
		return ret;
	}
	virtual void start() = 0;
	virtual bool wait() = 0;
	virtual void process() = 0;
	virtual void end() = 0;
private:
	atomic_bool active;
	thread *task;
	condition_variable condition;
	mutex mtx, sign;
	queue<queueType> data;
};