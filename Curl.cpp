#include "Curl.h"

#include <cstdint>
#include <filesystem>
#include <iostream>

#include <curl/curl.h>

#define SKIP_PEER_VERIFICATION 1

namespace
{
  std::size_t write_file_cb(
      void *ptr,
      std::size_t size,
      std::size_t num,
      void *stream)
  {
    const std::size_t written = fwrite(ptr, size, num, (FILE *) stream);
    return written;
  }

  std::size_t write_string_cb(
      const char* in,
      std::size_t size,
      std::size_t num,
      std::string* out)
  {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
  }

  std::size_t write_vector_cb(
      const char* in,
      std::size_t size,
      std::size_t num,
      std::vector<char>* out)
  {
    const std::size_t totalBytes(size * num);
    out->insert(out->end(), in, in + totalBytes);
    return totalBytes;
  }
}

Curl::Curl()
	: curl_(nullptr)
{
}

Curl::~Curl()
{
	if (curl_ != nullptr) {
	    curl_easy_cleanup(curl_);
	    curl_ = nullptr;
	}
}

void Curl::fetch_init()
{
	curl_ = curl_easy_init();
}

void Curl::fetch_set(const char *url)
{
    curl_easy_setopt(curl_, CURLOPT_URL, url);

#ifdef SKIP_PEER_VERIFICATION
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
    curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
}

int Curl::fetch_execute()
{
    int httpCode = 0;
    int rc = 0;

    CURLcode res = curl_easy_perform(curl_);

    if (res != CURLE_OK) {
    	std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl << std::flush;
    	rc = -1;
    }

    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl_);
    curl_ = nullptr;

    if (httpCode != 200) {
    	std::cerr << "Couldn't GET - exiting" << std::endl << std::flush;
    	rc = -1;
    }

    return rc;
}

int Curl::fetch(const char *url, const std::filesystem::path &path)
{
	int rc = 0;

	fetch_init();

	if (curl_ != nullptr) {
		fetch_set(url);
	    FILE *f = fopen(path.string().c_str(), "wb");
	    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_file_cb);
	    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, f);
	    rc = fetch_execute();
	    fclose(f);
	} else {
		rc = -1;
	}

	return rc;
}

int Curl::fetch(const char *url, std::unique_ptr<std::string> &data)
{
	int rc = 0;

	fetch_init();

	if (curl_ != nullptr) {
		fetch_set(url);
	    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_string_cb);
	    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, data.get());
	    rc = fetch_execute();
	} else {
		rc = -1;
	}

	return rc;
}

int Curl::fetch(const char *url, std::unique_ptr<std::vector<char>> &data)
{
	int rc = 0;

	fetch_init();

	if (curl_ != nullptr) {
		fetch_set(url);
		curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_vector_cb);
		curl_easy_setopt(curl_, CURLOPT_WRITEDATA, data.get());
		rc = fetch_execute();
	} else {
		rc = -1;
	}

	return rc;
}
