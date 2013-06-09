/*
 * compression.c
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
 
//  standard headers
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <utils.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
// compression specific headers
#include <zlib.h>
#include <lzop.h>

#include <lzma.h>


long uncompress_xz_memory( unsigned char* compressed_data , size_t compressed_data_size, unsigned char* uncompressed_data,size_t uncompressed_max_size){
    
    lzma_action action;
    lzma_ret ret_xz;
    long return_value;

    /* initialize xz decoder */
    lzma_stream lzmaInfo = LZMA_STREAM_INIT; /* alloc and init lzma_stream struct */
    lzmaInfo.avail_in=  compressed_data_size;
    lzmaInfo.total_in=  compressed_data_size;  
    lzmaInfo.avail_out=  uncompressed_max_size;
    lzmaInfo.total_out=  uncompressed_max_size;
    lzmaInfo.next_in= compressed_data  ;
    lzmaInfo.next_out= uncompressed_data;
    
    ret_xz = lzma_stream_decoder (&lzmaInfo, UINT64_MAX, LZMA_TELL_UNSUPPORTED_CHECK | LZMA_CONCATENATED);
    if (ret_xz != LZMA_OK) {
        D( "lzma_stream_decoder error: %d\n", (int) ret_xz);
        return 0;
    }
    ret_xz = lzma_code (&lzmaInfo, action);
    D( "ret_xz: %d\n", (int) ret_xz);
    return_value= lzmaInfo.total_out;
    lzma_end (&lzmaInfo);
    return return_value; 
    
}
long compress_xz_memory( unsigned char* compressed_data , size_t compressed_data_size, unsigned char* uncompressed_data,size_t uncompressed_max_size){
    

    return 0;
    
}

long uncompress_lzo_memory( unsigned char* compressed_data , size_t compressed_data_size, unsigned char* uncompressed_data,size_t uncompressed_max_size){

    
    
    
    // Double check the lzop magic, we should have already checked this we deciding
    // what decompression routine we needed
    if( !check_magic(compressed_data,sizeof(lzop_magic)) ){
        unsigned c = 0; 
        for(c = 0 ; c < sizeof(lzop_magic) ; c++)
            D("ERROR LZOP MAIGC CHECK FAILED DATA:%02x MAGIC:%02x\n",compressed_data[c],lzop_magic[c]);
        errno = ENOEXEC ;
        return 0; 
    }
    
    
    // read the lzop file header. this is after the magic so we offset the data
    lzop_stream s;
    size_t header_read = bread_lzop_header(&s, compressed_data+sizeof(lzop_magic), LZOP_MAX_HEADER_SIZE);
    if ( !header_read){
        D("ERROR LZOP HEADER READ FAILED %u\n",header_read);
        errno = ENOEXEC ;
        return 0 ;
    }
    
    // Setup a lzop_block. The block buffer is a pointer to the start of the 
    // compressed data including the magic and the size is the full size of the compressed data
    lzop_block b;
    b.c_buf = compressed_data;
    b.c_len = compressed_data_size;
    
    // read the lzop block header.         
    size_t block_header_read = bread_lzop_block(&s, compressed_data+sizeof(lzop_magic)+header_read, LZOP_MAX_BLOCK_HEADER_LEN, &b);
    if(!block_header_read ) {
        errno = EINVAL ;
        return 0 ;
    }
    
    int data_offset = sizeof(lzop_magic)+header_read+block_header_read+b.c_len ;
    D("data_offset 1 (%u+%u+%u+%u) = %u \n",sizeof(lzop_magic),header_read,block_header_read,b.c_len, data_offset) ;
    
    long decompressed_size = 0 ; 
    while( b.d_len > 0 ) {
        size_t decompress_bytes = lzop_decompress(&s, &b, uncompressed_data+decompressed_size, uncompressed_max_size);
        if(!decompress_bytes){
            errno = EINVAL ;
            return 0 ;
        }
        decompressed_size += b.d_len;
        D("LZOP DECOMPRESS block->c_len=%u b.d_len=%u\n",b.c_len ,b.d_len);
        int block_read = bread_lzop_block(&s,compressed_data+data_offset,LZOP_MAX_BLOCK_HEADER_LEN,&b);
         if(!block_read){
            errno = EINVAL ;
            return 0 ;
        }
        data_offset +=(block_read+b.c_len);
       
        D("data_offset  2 %u block_read %u b.c_len %u b.d_len %u\n",data_offset,block_read,b.c_len,b.d_len) ;

    }
    D("LZO DECOMPRESSED SIZE:%lu\n",decompressed_size);
    return decompressed_size;
}
long compress_lzo_memory( unsigned char* uncompressed_data , size_t uncompressed_data_size,unsigned char* compressed_data,size_t compressed_max_size){
    
    /*lzo_bytep wrkmem = (lzo_bytep) calloc(LZO1X_1_MEM_COMPRESS,sizeof(char));
    long compressed_len = 0;
    lzo1x_1_compress(uncompressed_data,uncompressed_data_size,compressed_data,&compressed_len,wrkmem);
    free(wrkmem);
    return compressed_len;*/
    return 0;
}
long uncompress_gzip_memory( unsigned char* compressed_data , size_t compressed_data_size, unsigned char* uncompressed_data,size_t uncompressed_max_size)
{

    errno = 0 ;
    z_stream zInfo = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
    zInfo.avail_in=  compressed_data_size;
    zInfo.total_in=  compressed_data_size;  
    zInfo.avail_out=  uncompressed_max_size;
    zInfo.total_out=  uncompressed_max_size;
    zInfo.next_in= compressed_data  ;
    zInfo.next_out= uncompressed_data;
    size_t return_value= 0;
    long err= inflateInit2( &zInfo,16+MAX_WBITS );               // zlib function

    if ( err == Z_OK ) {
    err= inflate( &zInfo, Z_FINISH );     // zlib function
    if ( err == Z_STREAM_END ) {
        return_value= zInfo.total_out;
    }else{ 
        errno=err;
        return_value =0;
    }
    }else{
        errno=err;
        return_value =0;
    }
    inflateEnd( &zInfo );   
    return( return_value ); 
}
long compress_gzip_memory( unsigned char* uncompressed_data , size_t uncompressed_data_size,unsigned char* compressed_data,size_t compressed_max_size)
{
   
    z_stream zInfo = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
    zInfo.total_in=  zInfo.avail_in=  uncompressed_data_size;
    zInfo.total_out= zInfo.avail_out= compressed_max_size;
    zInfo.next_in= uncompressed_data;
    zInfo.next_out= compressed_data;

    long err, return_value= -1;
    err=  deflateInit2(&zInfo, Z_DEFAULT_COMPRESSION, Z_DEFLATED,MAX_WBITS + 16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);    
    if ( err == Z_OK ) {
        err= deflate( &zInfo, Z_FINISH );              // zlib function
        if ( Z_STREAM_END == Z_STREAM_END ) {
            return_value= zInfo.total_out;
        }
    }
    deflateEnd( &zInfo );    // zlib function
    return( return_value );
}
