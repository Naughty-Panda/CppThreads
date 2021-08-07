//////////////////////////////////////////
//	C++ Threads
//	Naughty Panda @ 2021
//////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <ostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <vector>
#include <set>
#include <ppl.h>
#include <concurrent_vector.h>
#include <algorithm>
#include <execution>
#include <chrono>

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

//////////////////////////////////////////
//	6.3
//////////////////////////////////////////

struct Item {

private:
	std::string _name{ "" };
	size_t _value{ 0 };

public:
	Item(const char* name, const size_t& val) : _name(name), _value(val) {}

	friend bool operator < (const Item& lhs, const Item& rhs) { return lhs._value < rhs._value; }
	friend std::ostream& operator << (std::ostream& ostr, const Item& it) { return ostr << std::quoted(it._name) << " for " << it._value << '$'; }
};

void BuyFromStore(std::vector<Item>& shop, std::set<Item>& house, std::mutex& key) {

	if (shop.empty()) return;

	std::lock_guard lg(key);

	for (size_t i = 0; i < 2; ++i) {

		std::cout << "\n[+] Buying item: " << shop.back();
		house.insert(std::move(shop.back()));
		shop.pop_back();
	}
}

void StealFromHouse(std::set<Item>& house, std::mutex& key) {

	if (house.empty()) return;

	std::lock_guard lg(key);

	std::cout << "\n[-] Stealing item: " << *std::prev(house.end());
	house.erase(std::prev(house.end()));
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

	size_t PrimeNumToFind{ 1'000 };
	size_t CalcResult{ 0 };

	std::thread PrimeCalculator([&] { CalcResult = CalculatePrime(std::ref(PrimeNumToFind)); });
	PrimeCalculator.join();

	std::cout << "\nPrime number #" << PrimeNumToFind << ": " << CalcResult;

	//////////////////////////////////////////
	//	6.3
	//////////////////////////////////////////

	std::vector<Item> item_store{ {"TV", 500}, {"Radio", 200}, {"Sofa", 300}, {"Mirror", 150}, {"Table", 250}, {"Desk", 175}, {"Fan", 225}, {"Laptop", 850}, {"Phone", 95}, {"Stereo", 425} };
	
	std::cout << "\n\nItems in store:\n";
	std::copy(item_store.begin(), item_store.end(), std::ostream_iterator<Item>(std::cout, "; "));
	std::cout << '\n';

	std::set<Item> house;
	std::mutex key;

	std::thread master([&]
	{
		for (size_t i = 0; i < 5; ++i) {

			BuyFromStore(item_store, house, key);
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(3s);
		}
	});

	std::thread theif([&]
	{
		for (size_t i = 0; i < 10; ++i) {

			StealFromHouse(house, key);
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(2s);
		}
	});

	master.join();
	theif.join();

	return 0;
}
