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

//////////////////////////////////////////
//	6.2
//////////////////////////////////////////

size_t CalculatePrime(const size_t& num) {

	size_t nResCount{ 1 };
	size_t nLargest{ 0 };

	for (size_t i = 3; nResCount < num; ++i) {

		size_t div_count(0);
		size_t div_min(3);
		size_t div_max(sqrt(i) < 5 ? 5 : static_cast<size_t>(sqrt(i)));

		for (size_t div = div_min; div <= div_max; div += 2) {

			if (i <= div_min || i == div_max) break;
			if (i % div == 0 || i % 2 == 0) ++div_count;
		}

		if (div_count == 0) {
			++nResCount;
			nLargest = i;
			std::cout << '#' << nResCount << " prime number: " << i << '\n';
		}
	}

	return nLargest;
}

int main() {

	std::cout << "CPU threads: " << std::thread::hardware_concurrency() << '\n';

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

	//////////////////////////////////////////
	//	6.2
	//////////////////////////////////////////

	size_t PrimeNumToFind{ 10'000 };
	size_t CalcResult{ 0 };

	std::thread PrimeCalculator([&] { CalcResult = CalculatePrime(std::ref(PrimeNumToFind)); });
	PrimeCalculator.join();

	std::cout << "\nPrime number #" << PrimeNumToFind << ": " << CalcResult;

	return 0;
}
