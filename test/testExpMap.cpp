//
// @file: testExpMap.cpp
// @author: pj4dev.mit@gmail.com
// @url: https://github.com/pj4dev/custom-cpp-libs

#include "ExpiringMap.h"

#include <iostream>
#include <ctime>
#include <iterator>

typedef pj4dev::ExpiringMap<std::string, int> ExpMap;

void verbose(const ExpMap emap) {
	std::cout << "size = " << emap.size() << std::endl;
	std::cout << "hello = " << emap.get("hello") << "(left: " << emap.left("hello") << ")" << std::endl;
	std::cout << "world = " << emap.get("world") << "(left: " << emap.left("world") << ")" << std::endl;

}

int main() {
	ExpMap emap;
	emap.put("hello", 1, 500);
	emap.put("world", 2, 100);
	std::cout << "<=== after inserting 'hello' and 'world'\n";
	verbose(emap);

	emap.put("world", 2, 3000);
	sleep(1);
	std::cout << "<=== after inserting new 'world' and sleep 1s\n";
	verbose(emap);

	sleep(3);
	std::cout << "<=== after sleep 3s\n";
	verbose(emap);

	emap.put("hello", 11, 50000);
	emap.put("world", 12, 40000);
	std::cout << "<=== after add new 'hello' and 'world'\n";
	verbose(emap);

	auto keys = emap.keys();
	std::cout << "<=== after get keys\n";
	std::copy(keys.cbegin(), keys.cend(), std::ostream_iterator<decltype(*keys.cbegin())>(std::cout, " "));
	std::cout << std::endl;

	emap.erase("hello");
	std::cout << "<=== after delete hello\n";
	verbose(emap);

	sleep(2);
	std::cout << "<=== after sleep 2s\n";
	verbose(emap);

	emap.clear();
	std::cout << "<=== after clear()\n";
	verbose(emap);
}
