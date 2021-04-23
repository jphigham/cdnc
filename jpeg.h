#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int jpeg_read_mem(const unsigned char *, const unsigned long, unsigned int *, unsigned int *, unsigned char *);
int jpeg_read_file(const char *, unsigned int *, unsigned int *, unsigned char *);

#ifdef __cplusplus
}
#endif
