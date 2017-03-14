#ifndef CRYPTO
#define CRYPTO

#include <iostream>
#include <sodium.h>
#include <array>

namespace crypto {

int xsalsa_inplace(unsigned char * msg,
				   size_t msg_size,
				   std::array<unsigned char, crypto_stream_NONCEBYTES> &nonce,
				   std::array<unsigned char, crypto_stream_KEYBYTES> &key) {

	return crypto_stream_xor(msg,
							 msg,
							 msg_size,
							 nonce.data(),
							 key.data());
}

int xsalsa_inplace(unsigned char * msg,
				   size_t msg_size,
				   unsigned char * nonce,
				   unsigned char * key) {

	return crypto_stream_xor(msg,
							 msg,
							 msg_size,
							 nonce,
							 key);
}

}
#endif // CRYPTO

