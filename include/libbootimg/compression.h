/*
 * compression.h
 * 
 * Copyright 2013 Trevor Drake <trevd1234@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef _25aa2a1a_9068_11e2_8847_5404a601fa9d
#define _25aa2a1a_9068_11e2_8847_5404a601fa9d

/*
   This file is part of the LZO real-time data compression library.

   Copyright (C) 1996..2008 Markus Franz Xaver Johannes Oberhumer
   All Rights Reserved.

   Markus F.X.J. Oberhumer <markus@oberhumer.com>
   http://www.oberhumer.com/opensource/lzo/

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdint.h>

#define LZO1X
#undef LZO1Y

#undef assert
/*
static void die_at(int line)
{
        bb_error_msg_and_die("internal error at %d", line);
}
#define assert(v) if (!(v)) die_at(__LINE__)
*/
#define assert(v) ((void)0)
uint32_t *global_crc32_table;
uint32_t crc32_block_endian0(uint32_t val, const void *buf, unsigned len, uint32_t *crc_table);
uint32_t crc32_block_endian1(uint32_t val, const void *buf, unsigned len, uint32_t *crc_table);
uint32_t* crc32_filltable(uint32_t *crc_table, int endian);
int lzo1x_1_compress(const uint8_t* src, unsigned src_len,
        uint8_t* dst, unsigned* dst_len,
        void* wrkmem);
int lzo1x_1_15_compress(const uint8_t* src, unsigned src_len,
        uint8_t* dst, unsigned* dst_len,
        void* wrkmem);
int lzo1x_999_compress_level(const uint8_t* in, unsigned in_len,
        uint8_t* out, unsigned* out_len,
        void* wrkmem,
        int compression_level);

/* decompression */
//int lzo1x_decompress(const uint8_t* src, unsigned src_len,
//      uint8_t* dst, unsigned* dst_len,
//      void* wrkmem /* NOT USED */);
/* safe decompression with overrun testing */
int lzo1x_decompress_safe(const uint8_t* src, unsigned src_len,
        uint8_t* dst, unsigned* dst_len,
        void* wrkmem /* NOT USED */);

#define LZO_E_OK                    0
#define LZO_E_ERROR                 (-1)
#define LZO_E_OUT_OF_MEMORY         (-2)    /* [not used right now] */
#define LZO_E_NOT_COMPRESSIBLE      (-3)    /* [not used right now] */
#define LZO_E_INPUT_OVERRUN         (-4)
#define LZO_E_OUTPUT_OVERRUN        (-5)
#define LZO_E_LOOKBEHIND_OVERRUN    (-6)
#define LZO_E_EOF_NOT_FOUND         (-7)
#define LZO_E_INPUT_NOT_CONSUMED    (-8)
#define LZO_E_NOT_YET_IMPLEMENTED   (-9)    /* [not used right now] */

/* lzo-2.03/include/lzo/lzoconf.h */
#define LZO_VERSION   0x2030




#define GZIP_DEFLATE_MAGIC "\x1F\x8B\x08"
#define GZIP_DEFLATE_MAGIC_SIZE 3

#define LZOP_MAGIC "\x89\x4C\x5A\x4F" // .LZO
#define LZOP_MAGIC_SIZE 4

#define XZ_MAGIC "\xFD\x37\x7A\x58\x5A\x0" // .7zXZ NULL
#define XZ_MAGIC_SIZE 6

#define LZMA_MAGIC "\x5D\x00" // ] .NULL 
#define LZMA_MAGIC_SIZE 2

#define BZIP2_MAGIC "\x42\x5A" // BZ 
#define BZIP2_MAGIC_SIZE 2

#define LZ4_MAGIC "\x02\x21" // .!
#define LZ4_MAGIC_SIZE 2



long uncompress_gzip_memory(unsigned char* compressed_data , size_t compressed_data_size, 
                    unsigned char* uncompressed_data,size_t uncompressed_max_size);
    
    long compress_gzip_memory( unsigned char* uncompressed_data ,size_t uncompressed_data_size,
                unsigned char* compressed_data,size_t compressed_max_size);
    long uncompress_lzo_memory(unsigned char* compressed_data , size_t compressed_data_size, 
                    unsigned char* uncompressed_data,size_t uncompressed_max_size);
    
    long compress_lzo_memory( unsigned char* uncompressed_data ,size_t uncompressed_data_size,
                unsigned char* compressed_data,size_t compressed_max_size);
#endif
