//////////////////////////////////////////
//	C++ Threads
//	Naughty Panda @ 2021
//////////////////////////////////////////

#include <iostream>
#include <ostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <vector>

//////////////////////////////////////////
//	6.1
//////////////////////////////////////////

class ThreadSafeCout : public std::ostringstream {

private:
	static std::mutex _mut;

public:
	ThreadSafeCout() = default;

	template <typename T>
	ThreadSafeCout& operator << (const T& in) {
		std::lock_guard<std::mutex> lock(_mut);
		std::cout << in;
		return *this;
	}
};

std::mutex ThreadSafeCout::_mut;

int main() {

	//////////////////////////////////////////
	//	6.1
	//////////////////////////////////////////

	ThreadSafeCout pcout;

	std::vector<std::thread> threads(10);

	for (size_t i = 0; i < 10; ++i) {
		threads.emplace_back([&, i]
		{
			pcout << i << " thread with id: " << std::this_thread::get_id() << '\n';
		});
	}

	for (auto& th : threads) {
		if(th.joinable()) th.join();
	}
	
	return 0;
}
