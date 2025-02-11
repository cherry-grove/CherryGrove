#pragma once
#include <iostream>
#include <sstream>
#include <mutex>
#include <thread>

using std::cout, std::cerr, std::endl, std::ostream, std::enable_if, std::is_function, std::stringstream, std::this_thread::get_id, std::lock_guard, std::mutex;

namespace Logger {
	void shutdown();

	bool getMode();
	void setToFile(bool _toFile);

	extern mutex loggerMutex;

	extern thread_local stringstream threadBuffer;
	struct LoggerCout {
		LoggerCout& operator<<(ostream& (*endl)(ostream&)) {
			if (!threadBuffer.str().empty()) {
				lock_guard guard(loggerMutex);
				cout << threadBuffer.str() << endl;
				threadBuffer.str("");
				threadBuffer.clear();
			}
			return *this;
		}

		template <typename T>
		typename enable_if<!is_function<T>::value, LoggerCout&>::type operator<<(const T& value) {
			auto* buffer = threadBuffer.rdbuf();
			if (threadBuffer.str().empty()) threadBuffer << "[" << get_id() << "] ";
			threadBuffer << value;
			return *this;
		}

	};
	extern LoggerCout lout;

	extern thread_local stringstream threadBufferErr;
	struct LoggerCerr {
		LoggerCerr& operator<<(ostream& (*endl)(ostream&)) {
			if (!threadBufferErr.str().empty()) {
				lock_guard guard(loggerMutex);
				cerr << threadBufferErr.str() << endl;
				threadBufferErr.str("");
				threadBufferErr.clear();
			}
			return *this;
		}

		template <typename T>
		typename enable_if<!is_function<T>::value, LoggerCerr&>::type operator<<(const T& value) {
			auto* buffer = threadBufferErr.rdbuf();
			if (threadBufferErr.str().empty()) threadBufferErr << "[" << get_id() << "] [Error] ";
			threadBufferErr << value;
			return *this;
		}

	};
	extern LoggerCerr lerr;
}