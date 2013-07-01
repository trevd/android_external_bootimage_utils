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


#define COMPRESSION_BZIP2_FILE_EXTENSION ".bz2"
#define COMPRESSION_GZIP_FILE_EXTENSION ".gz"
#define COMPRESSION_LZO_FILE_EXTENSION ".lzo"
#define COMPRESSION_LZMA_FILE_EXTENSION ".lzma"
#define COMPRESSION_LZ4_FILE_EXTENSION ".lz4"
#define COMPRESSION_CPIO_FILE_EXTENSION ".cpio"
#define COMPRESSION_xz_FILE_EXTENSION ".xz"






#define COMPRESSION_GZIP_DEFLATE        1
#define COMPRESSION_LZOP                2
#define COMPRESSION_XZ                  3
#define COMPRESSION_LZMA                4
#define COMPRESSION_BZIP2               5
#define COMPRESSION_LZ4                 6
#define COMPRESSION_CPIO                7
#define COMPRESSION_INDEX_MAX           COMPRESSION_CPIO
/* get_compression_name_from_index(int index)
 * 
 * Helper function to get the name of a compression type value 
 * index should be a valid compression index number
 * 
 * Return:
 * On success a pointer to a string containing the name of the compression type
 * On failure NULL and errno is set ;
 */
char * get_compression_name_from_index(unsigned index); 
unsigned get_compression_index_from_name(char *name) ;

unsigned char * find_compressed_data_in_memory( unsigned char *haystack, unsigned haystack_len, int* compression );

unsigned char * find_compressed_data_in_memory_start_at( unsigned char *haystack, unsigned haystack_len,
                unsigned char *haystack_offset, int* compression );

long uncompress_gzip_memory(unsigned char* compressed_data , size_t compressed_data_size, 
                unsigned char* uncompressed_data,size_t uncompressed_max_size);
    
long compress_gzip_memory( unsigned char* uncompressed_data ,size_t uncompressed_data_size,
                unsigned char* compressed_data,size_t compressed_max_size);

long uncompress_lzo_memory(unsigned char* compressed_data , size_t compressed_data_size, 
                unsigned char* uncompressed_data,size_t uncompressed_max_size);

long compress_lzo_memory( unsigned char* uncompressed_data ,size_t uncompressed_data_size,
                unsigned char* compressed_data,size_t compressed_max_size);
                
long uncompress_xz_memory(unsigned char* compressed_data , size_t compressed_data_size, 
                unsigned char* uncompressed_data,size_t uncompressed_max_size);

long compress_xz_memory( unsigned char* uncompressed_data ,size_t uncompressed_data_size,
                unsigned char* compressed_data,size_t compressed_max_size);

long uncompress_bzip2_memory(unsigned char* compressed_data , size_t compressed_data_size, 
                unsigned char* uncompressed_data,size_t uncompressed_max_size);

long compress_bzip2_memory( unsigned char* uncompressed_data ,size_t uncompressed_data_size,
                unsigned char* compressed_data,size_t compressed_max_size);



long uncompress_lzma_memory(unsigned char* compressed_data , size_t compressed_data_size, 
                unsigned char* uncompressed_data,size_t uncompressed_max_size);

long compress_lzma_memory( unsigned char* uncompressed_data ,size_t uncompressed_data_size,
                unsigned char* compressed_data,size_t compressed_max_size);


#endif
