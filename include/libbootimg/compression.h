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
