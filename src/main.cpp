
#include <random>
#include <algorithm>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "crypto.hpp"
#include "unit_tests.hpp"

char generate_random_char() {
	static const char Charset[] = "0123456789"
								  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								  "abcdefghijklmnopqrstuvwxyz";
	const size_t max_index = (sizeof(Charset) - 1);

	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> dis(0, max_index);

	return Charset[dis(gen)];
}

template<typename T,std::size_t SIZE>
std::array<T, SIZE> generate_random_array () {
	std::array<T, SIZE> ar;
	std::generate_n(ar.begin(), SIZE, generate_random_char);
	return ar;
}

void test_crypto_inplace() {

	std::array<unsigned char, crypto_box_NONCEBYTES> nonce;
	std::array<unsigned char,  crypto_stream_KEYBYTES> key;
	nonce = generate_random_array<unsigned char, crypto_box_NONCEBYTES>();
	key = generate_random_array<unsigned char, crypto_stream_KEYBYTES>();

	unsigned char start_msg[] = {'a','b','c','d','e','f','g','h'};
	unsigned char msg[] = {'a','b','c','d','e','f','g','h'};

	// encrypt
	if(crypto::xsalsa_inplace(msg, sizeof(msg),nonce, key) != 0) {
		std::cerr << "Fail to xsalsa encrypt/decrypt\n";
	}

	// decrypt
	if(crypto::xsalsa_inplace(msg, sizeof(msg),nonce, key) != 0) {
		std::cerr << "Fail to xsalsa encrypt/decrypt\n";
	}

	if (std::memcmp(msg, start_msg, sizeof(msg))) {
		std::cout << "fail to decrypt msg\n";
		return;
	}
	std::cout << "test_crypto_inplace OK\n";

}

void test_crypto_inplace2() {

	std::array<unsigned char, crypto_box_NONCEBYTES> nonce;
	std::array<unsigned char,  crypto_stream_KEYBYTES> key;
	nonce = generate_random_array<unsigned char, crypto_box_NONCEBYTES>();
	key = generate_random_array<unsigned char, crypto_stream_KEYBYTES>();

	unsigned char start_msg[] = {'a','b','c','d','e','f','g','h'};
	unsigned char msg[] = {'a','b','c','d','e','f','g','h'};

	auto buf_vec = chunk_char_buf(msg,sizeof(msg),4);
	for(auto &i : buf_vec) {
		// encrypt
		if(crypto::xsalsa_inplace(i.first, i.second,nonce, key) != 0) {
			std::cerr << "Fail to xsalsa encrypt/decrypt\n";
		}
	}

	for(auto &i : buf_vec) {
		// decrypt
		if(crypto::xsalsa_inplace(i.first, i.second,nonce, key) != 0) {
			std::cerr << "Fail to xsalsa encrypt/decrypt\n";
		}
	}


	if (std::memcmp(msg, start_msg, sizeof(msg))) {
		std::cout << "fail to decrypt msg\n";
		return;
	}
	std::cout << "test_crypto_inplace OK\n";

}

class thread_crypto final {
public:
	struct job;
	thread_crypto (size_t num_of_threads) : ready_for_run_flag(false)
	{

		m_threads.resize(num_of_threads);
		for(auto &thr : m_threads) {
			thr = std::thread([this]{
				while(true) {
					ready_for_run_flag = true;
					ready_for_run_cv.notify_all();
					std::unique_lock<std::mutex> lk(wakeup_mtx);
					wakeup_cv.wait(lk);
					size_t job_index = job_to_let.fetch_add(1);
					if(job_index >= m_jobs.size()) {
						jobs_end.notify_all();
						break; // todo
					}
					m_jobs.at(job_index)();
				}
			});
		}
	}

	void add_job (job && j) {
		m_jobs.emplace_back(std::move(j));
	}

	void run_jobs() {
		std::unique_lock<std::mutex> lk(wakeup_mtx);
		ready_for_run_cv.wait(lk, [this]{return ready_for_run_flag.load();});
		wakeup_cv.notify_all();
	}
	void join() {
		std::unique_lock<std::mutex> lk(wakeup_mtx);
		jobs_end.wait(lk);
	}

	struct job {
		job() = default;
		job(job && rhs) noexcept {
			if(this == &rhs) {
				return;
			}
			m_buf = rhs.m_buf; rhs.m_buf = nullptr;
			m_nonce = rhs.m_nonce; rhs.m_nonce = nullptr;
			m_key = rhs.m_key; rhs.m_key = nullptr;
			m_buf_size = rhs.m_buf_size; rhs.m_buf_size = 0;
		}

		void operator ()() {
			if (crypto::xsalsa_inplace(m_buf, m_buf_size, m_nonce, m_key) != 0) {
				std::cerr << "fail to encrypt/decrypt\n";
			}
		}
		unsigned char * m_buf;
		size_t m_buf_size;
		unsigned char * m_nonce;
		unsigned char * m_key;
	};

	~thread_crypto () {
		wakeup_cv.notify_all();
		jobs_end.notify_all();
		for (auto &thr : m_threads) {
			thr.join();
		}
	}

private:
	std::atomic<size_t> job_to_let;
	std::atomic<bool> ready_for_run_flag;
	std::condition_variable ready_for_run_cv;
	std::mutex wakeup_mtx;
	std::condition_variable wakeup_cv;
	std::condition_variable jobs_end;
	std::vector<job> m_jobs;
	std::vector<std::thread> m_threads;
};

constexpr size_t chunk_size = 4;

int main() {
	test1();
	test2();
	test3();

	if (sodium_init() == -1) {
		throw std::runtime_error("Fail to init sodium");
	}

	test_crypto_inplace();
	test_crypto_inplace2();

	std::array<unsigned char, crypto_box_NONCEBYTES> nonce;
	std::array<unsigned char,  crypto_stream_KEYBYTES> key;
	nonce = generate_random_array<unsigned char, crypto_box_NONCEBYTES>();
	key = generate_random_array<unsigned char, crypto_stream_KEYBYTES>();

	unsigned char start_msg[] = {'a','b','c','d','e','f','g','h'};
	auto buf_vec = chunk_char_buf(start_msg, sizeof(start_msg),chunk_size);

	thread_crypto thread_encrypt(2);

	for(auto &i: buf_vec) {
		thread_crypto::job loc_job;
		loc_job.m_buf = i.first;
		loc_job.m_buf_size = i.second;
		loc_job.m_nonce = nonce.data();
		loc_job.m_key = key.data();
		thread_encrypt.add_job(std::move(loc_job));
	}
	//std::this_thread::sleep_for(std::chrono::seconds(2));
	thread_encrypt.run_jobs();
	thread_encrypt.join();

	//std::vector<std::pair<unsigned char *, size_t>> m_bufs;


	return 0;
}

