#ifndef UNIT_TESTS
#define UNIT_TESTS

#include "buffer_split.hpp"

void test1() {
	unsigned char buf[4] = {'a','b','c','d'};
	auto buf_vector = chunk_char_buf(buf,sizeof(buf), 4);


	if (buf_vector.at(0).first != buf || std::memcmp(buf_vector.at(0).first, buf, sizeof(buf))) {
		std::cout << "buf_vector.at(0) != 'abcd' :" << buf_vector.at(0).first << "\n";
		std::cout << "sizeof buf :" << sizeof(buf) << "\n";
	} else {
		std::cout << "test1 OK\n";
	}
}

void test2() {
	unsigned char buf[4] = {'a','b','c','d'};
	size_t chunk_size = 2;
	auto buf_vector = chunk_char_buf(buf, sizeof(buf), chunk_size);

	if (buf_vector.at(0).first != buf || std::memcmp(buf_vector.at(0).first, buf, chunk_size)) {
		std::cout << "buf_vector.at(0) != 'ab' :" << buf_vector.at(0).first << "\n";

	}
	else if (buf_vector.at(1).first != buf+chunk_size || std::memcmp(buf_vector.at(1).first, buf+chunk_size, chunk_size)) {
		std::cout << "buf_vector.at(1) != 'cd' :" << buf_vector.at(1).first << "\n";
	} else {
		std::cout << "test2 OK\n";
	}
}

void test3() {
	unsigned char buf[5] = {'a','b','c','d','e'};
	size_t chunk_size = 2;
	auto buf_vector = chunk_char_buf(buf, sizeof(buf), chunk_size);

	bool ok = true;
	//std::cout << std::ceil(sizeof(buf)/static_cast<double>(chunk_size)) << "\n";
	for(size_t i = 0; i < std::ceil(sizeof(buf)/static_cast<double>(chunk_size)); ++i) {

		if (buf_vector.at(i).first != buf+chunk_size*i || std::memcmp(buf_vector.at(i).first, buf+chunk_size*i, chunk_size)) {
			std::cerr << "buf_vector.at("<< i << ") = : ";
				for(size_t j = 0; j < buf_vector.at(i).second; ++j) {
					std::cerr << buf_vector.at(i).first[j];
				} std::cerr << "\n";
			ok = false;
		}
	}
	if(ok) {
		std::cout << "test3 OK\n";
	}
}

#endif // UNIT_TESTS

