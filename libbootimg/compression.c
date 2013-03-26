#include <stdio.h>
#include <errno.h>
#include <utils.h>
#include <zlib.h>
long uncompress_gzip_memory( unsigned char* compressed_data , size_t compressed_data_size, unsigned char* uncompressed_data,size_t uncompressed_max_size)
{
	
    z_stream zInfo = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
    zInfo.avail_in=  compressed_data_size;
    zInfo.total_in=  compressed_data_size;  
    zInfo.avail_out=  uncompressed_max_size;
    zInfo.total_out=  uncompressed_max_size;
    zInfo.next_in= compressed_data	;
    zInfo.next_out= uncompressed_data;
    size_t return_value= 0;
    long err= inflateInit2( &zInfo,16+MAX_WBITS );               // zlib function
    
    if ( err == Z_OK ) {
        err= inflate( &zInfo, Z_FINISH );     // zlib function
        if ( err == Z_STREAM_END ) {
            return_value= zInfo.total_out;
        }else{ 
	    errno=err;
	    return_value =-1;
	}
    }else{
	    errno=err;
	    return_value =-1;
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