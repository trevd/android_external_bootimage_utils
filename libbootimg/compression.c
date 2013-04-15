#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <utils.h>
#include <zlib.h>
#include <minilzo.h>
#include <string.h>
long uncompress_lzo_memory( unsigned char* compressed_data , size_t compressed_data_size, unsigned char* uncompressed_data,unsigned uncompressed_max_size){
    
    
    int lzo_result = lzo_init() ;
    D("uncompress_lzo_memory %d\n",lzo_result);
    if(lzo_result != LZO_E_OK){
	errno = lzo_result;
	return -1;
    }
    
    lzo_uint uncompressed_size = uncompressed_max_size ;
    
    lzo_voidp workp ;
    lzo_memset(workp,0,uncompressed_size);
    D("lzo1x_decompress compressed_data=%p compressed_data_size=%u uncompressed_max_size=%d\n",compressed_data,compressed_data_size,uncompressed_max_size);
    lzo_result = lzo1x_decompress( compressed_data ,  compressed_data_size,  uncompressed_data, &uncompressed_size,workp);
    if(lzo_result != LZO_E_OK){
	D("lzo_result=%d %s %u\n",lzo_result,strerror(lzo_result),uncompressed_size);
	errno = lzo_result;
	return -1;
    }
    D("uncompressed_size=%l\n",uncompressed_size);
    return uncompressed_size;
}

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
