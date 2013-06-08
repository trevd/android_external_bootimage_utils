#ifndef _LZOP_SUPPORT_H
#define _LZOP_SUPPORT_H

#include <lzo/lzo1x.h>

void set_be16(unsigned char *b, unsigned v);

unsigned get_be16(const unsigned char *b);

void set_be32(unsigned char *b, lzo_uint32 v);

lzo_uint32 get_be32(const unsigned char *b);

size_t lzo_malloc_value(int method);

size_t read_buf(int ifd, lzo_voidp buffer, size_t len);

#endif
