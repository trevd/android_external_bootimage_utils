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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <utils.h>
#include <zlib.h>
#include <string.h>
long uncompress_lzo_memory( unsigned char* compressed_data , size_t compressed_data_size, unsigned char* uncompressed_data,unsigned uncompressed_max_size){

    return 0;
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
