#pragma once

#include <thread>
#include <mutex>
#include <deque>

#include "Message.hpp"

// Credit https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/BiggerProjects/Networking/Parts1%262/net_tsqueue.h


template<typename T>
class TSQueue {
public:
	TSQueue() = default;
	TSQueue(const TSQueue<T>&) = delete;
	virtual ~TSQueue() { clear(); }

protected:
	std::mutex muxQueue;
	std::deque<T> deqQueue;
	std::condition_variable cvBlocking;
	std::mutex muxBlocking;

public:
	const T& front() {
		std::scoped_lock lock(muxQueue);
		return deqQueue.front();
    }

	const T& back() {
	    std::scoped_lock lock(muxQueue);
		return deqQueue.back();
	}

	T pop_front() {
		std::scoped_lock lock(muxQueue);
		auto t = std::move(deqQueue.front());
		deqQueue.pop_front();
		return t;
	}

	T pop_back() {
		std::scoped_lock lock(muxQueue);
		auto t = std::move(deqQueue.back());
		deqQueue.pop_back();
		return t;
	}

	void push_back(const T& item) {
		std::scoped_lock lock(muxQueue);
		deqQueue.emplace_back(std::move(item));

		std::unique_lock<std::mutex> ul(muxBlocking);
		cvBlocking.notify_one();
	}

	void push_front(const T& item) {
		std::scoped_lock lock(muxQueue);
		deqQueue.emplace_front(std::move(item));

		std::unique_lock<std::mutex> ul(muxBlocking);
		cvBlocking.notify_one();
	}

	bool empty() {
		std::scoped_lock lock(muxQueue);
		return deqQueue.empty();
	}
	
    size_t count() {
		std::scoped_lock lock(muxQueue);
		return deqQueue.size();
	}

	void clear() {
		std::scoped_lock lock(muxQueue);
		deqQueue.clear();
	}

	void wait() {
		while (empty()) {
			std::unique_lock<std::mutex> ul(muxBlocking);
			cvBlocking.wait(ul);
		}
	}

	T get(std::size_t i) {
		std::scoped_lock lock(muxQueue);
		return deqQueue.at(i);
	} 

	void eraseItem(const T& item) {
		std::scoped_lock(muxQueue);
		deqQueue.erase(std::remove(deqQueue.begin(), deqQueue.end(), item), deqQueue.end());
	}

	void eraseNthItem(std::size_t idx) {
		std::scoped_lock lock(muxQueue);
		deqQueue.erase(deqQueue.begin() + idx);
	}

	void lock() {
		muxQueue.lock();
	}

	void unlock() {
		muxQueue.unlock();
	}

	typename std::deque<T>::iterator begin() {
		return deqQueue.begin();
	}

	typename std::deque<T>::iterator end() {
		return deqQueue.end();
	}
};