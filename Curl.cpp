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

CURL *fetch_init()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	return curl_easy_init();
}

void fetch_set(const char *url, CURL *curl)
{
    curl_easy_setopt(curl, CURLOPT_URL, url);

#ifdef SKIP_PEER_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
}

int fetch_execute(const char *url, CURL *curl)
{
    CURLcode res = curl_easy_perform(curl);
    int httpCode(0);
    int rc(0);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    if (res != CURLE_OK) {
    	// std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    	rc = -1;
    }

    curl_easy_cleanup(curl);

    if (httpCode != 200) {
    	// std::cerr << "Couldn't GET from " << url << " - exiting" << std::endl;
    	rc = -1;
    }

    curl_global_cleanup();
    return rc;
}

int Curl::fetch(const char *url, const std::filesystem::path &path)
{
	CURL *curl = fetch_init();
	int rc(0);

	if (curl != nullptr) {
		fetch_set(url, curl);
	    FILE *f = fopen(path.string().c_str(), "wb");
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_cb);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
	    rc = fetch_execute(url, curl);
	    fclose(f);
	} else {
		rc = -1;
	}

	return rc;
}

int Curl::fetch(const char *url, std::unique_ptr<std::string> &data)
{
	CURL *curl = fetch_init();
	int rc(0);

	if (curl != nullptr) {
		fetch_set(url, curl);
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_string_cb);
	    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data.get());
	    rc = fetch_execute(url, curl);
	} else {
		rc = -1;
	}

	return rc;
}

int Curl::fetch(const char *url, std::unique_ptr<std::vector<char>> &data)
{
	CURL *curl = fetch_init();
	int rc(0);

	if (curl != nullptr) {
		fetch_set(url, curl);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_vector_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, data.get());
		rc = fetch_execute(url, curl);
	} else {
		rc = -1;
	}

	return rc;
}
