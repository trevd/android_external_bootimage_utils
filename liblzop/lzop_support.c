#ifndef _LZOP_SUPPORT_C
#define _LZOP_SUPPORT_C

#include "lzop_support.h"
#include "lzop.h"

void set_be16(unsigned char *b, unsigned v) {
  b[1] = (unsigned char) (v >> 0);
  b[0] = (unsigned char) (v >> 8);
}

unsigned get_be16(const unsigned char *b) {
  unsigned v;
  v  = (unsigned) b[1] <<  0;
  v |= (unsigned) b[0] <<  8;
  return v;
}

void set_be32(unsigned char *b, lzo_uint32 v) {
    b[3] = (unsigned char) (v >>  0);
    b[2] = (unsigned char) (v >>  8);
    b[1] = (unsigned char) (v >> 16);
    b[0] = (unsigned char) (v >> 24);
}

lzo_uint32 get_be32(const unsigned char *b) {
  lzo_uint32 v;
  v  = (lzo_uint32) b[3] <<  0;
  v |= (lzo_uint32) b[2] <<  8;
  v |= (lzo_uint32) b[1] << 16;
  v |= (lzo_uint32) b[0] << 24;
  return v;
}

size_t lzo_malloc_value(int method) {
  switch(method) {
    case M_LZO1X_1:
      return LZO1X_1_MEM_COMPRESS;
      break;
    case M_LZO1X_1_15:
      return LZO1X_1_15_MEM_COMPRESS;
      break;
    case M_LZO1X_999:
      return LZO1X_999_MEM_COMPRESS;
      break;
    default:
      return 0;
  }
}

#endif
