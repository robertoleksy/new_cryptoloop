#ifndef BUFFER_SPLIT
#define BUFFER_SPLIT

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstring>

std::vector<std::vector<char>> chunk_char_vector(std::vector<char> &vec, size_t chunk_size) {
	size_t main_len = vec.size();

	assert(chunk_size > 0  && "Can not part to zero parts");
	assert(main_len > 0 && "Can not part zero size vector");

	std::vector<std::vector<char>> vec_parts;
	if(chunk_size > main_len) {
		std::cerr << "WARNING, chunk size is greater than array size : decreasing one chunk to max ["
				  << main_len << "]\n";
		chunk_size = main_len;
		vec_parts.push_back(vec);
		return vec_parts;
	}

	size_t actual_pos = 0;
	size_t parts = std::ceil(main_len/static_cast<double>(chunk_size));
	for (size_t i = 0;i < parts; ++i) {
		double end_dist = (i+1)*(main_len/static_cast<double>(parts));

		auto begin_pos = vec.begin() + actual_pos;
		actual_pos = std::ceil(end_dist);
		auto end_pos = vec.end() - ((actual_pos >= main_len) ? 0 : (main_len - actual_pos));

		vec_parts.push_back(std::vector<char>(begin_pos, end_pos));
	}
	return vec_parts;
}

std::vector<std::pair<unsigned char *,size_t>> chunk_char_buf(unsigned char * buf,size_t buf_size, size_t chunk_size) {
	size_t main_len = buf_size;

	assert(chunk_size > 0  && "Can not part to zero parts");
	assert(main_len > 0 && "Can not part zero size vector");

	std::vector<std::pair<unsigned char*, size_t>> buf_parts;
	if(chunk_size >= main_len) {
		std::cerr << "WARNING, chunk size is greater/equal than array size : decreasing one chunk to max ["
				  << main_len << "]\n";
		chunk_size = main_len;
		buf_parts.emplace_back(std::make_pair(buf, buf_size));
		return buf_parts;
	}

	size_t actual_pos = 0;
	size_t parts = std::ceil(main_len/static_cast<double>(chunk_size));
	for (size_t i = 0;i < parts; ++i) {
		double end_dist = (i+1)*(main_len/static_cast<double>(parts));

		auto * begin_pos = buf + actual_pos;
		actual_pos = std::ceil(end_dist);
		auto * end_pos = buf+sizeof(buf) - ((actual_pos >= main_len) ? 0 : (main_len - actual_pos));
		size_t ptr_distance = end_pos - begin_pos;
		buf_parts.emplace_back(std::make_pair(begin_pos, ptr_distance));
	}
	return buf_parts;
}

#endif // BUFFER_SPLIT

