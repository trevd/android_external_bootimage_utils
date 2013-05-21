#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <utils.h>
#include <kernel.h>
#include <compression.h>



#define KERNEL_VERSION_STRING "Linux version"
#define KERNEL_VERSION_STRING_SIZE 13

#define KERNEL_IKCFG_ST "IKCFG_ST"
#define KERNEL_IKCFG_ED "IKCFG_ED"
#define KERNEL_IKCFG_SIZE 8

#define MAX_KERNEL_SIZE (8192*1024)*4

int print_kernel_info(kernel_image* kimage){
    
    //fprintf(stderr,"  kernel_addr      :%p\n",bimage.kernel_addr); 
    //fprintf(stderr,"  kernel_size      :%u\n",bimage.kernel_size); 
    int klen = 0 ;
    if(kimage->version) klen = strlen(kimage->version);
   
    fprintf(stderr,"  Version   :%s",kimage->version);
    if(klen > 0 && kimage->version[klen-1]!='\n') {
	D("Adding newline kimage->version[%d]='%d'",klen,kimage->version[klen-1]);
	 fprintf(stderr,"\n");
	}
    return 0;
    
}


int load_kernel_image_from_memory(unsigned char* kernel_addr,unsigned kernel_size,kernel_image* image ){
    
    // Look for the kernel zImage Magic
    int return_value = 0 ;
    
    D("kernel_addr=%p kernel_size=%u\n",kernel_addr,kernel_size);
    unsigned char * kernel_magic_offset_p = find_in_memory(kernel_addr,kernel_size,KERNEL_ZIMAGE_MAGIC, KERNEL_ZIMAGE_MAGIC_SIZE );
    if(!kernel_magic_offset_p){
	D("kernel_magic_offset not found\n")
	return_value = ENOEXEC;
	goto exit;
    
    }

        
    D("kernel_magic_offset_p : %p\n",kernel_magic_offset_p);
    // look for a gzip entry in the packed kernel image data 
    unsigned char * gzip_magic_offset_p = find_in_memory_start_at(kernel_addr,kernel_size,kernel_magic_offset_p,GZIP_DEFLATE_MAGIC, GZIP_DEFLATE_MAGIC_SIZE );
    D("gzip_magic_offset_p : %p\n",gzip_magic_offset_p);
     if(!gzip_magic_offset_p){
	return_value = ENOEXEC;
	goto exit;
    }
    
    image->compression_type = KERNEL_COMPRESSION_GZIP ;
    
    unsigned char *uncompressed_kernel_data = calloc(MAX_KERNEL_SIZE,sizeof(unsigned char)) ;
    long uncompressed_kernel_size = 0;
    errno = 0 ;
    uncompressed_kernel_size = uncompress_gzip_memory(gzip_magic_offset_p,kernel_size,uncompressed_kernel_data,MAX_KERNEL_SIZE);
    if(errno){
	D("errno: %u %s\n",errno,strerror(errno));
	free(uncompressed_kernel_data);
	return  uncompressed_kernel_size;
	
    }
    D("uncompressed_kernel_size : %ld\n",uncompressed_kernel_size);
    // fill in the basic values    
    image->start_addr = uncompressed_kernel_data;
    image->size = uncompressed_kernel_size;
    image->version = (char *)find_in_memory(uncompressed_kernel_data,uncompressed_kernel_size,KERNEL_VERSION_STRING,KERNEL_VERSION_STRING_SIZE);
    
    // find the first number string
    image->version_number = image->version + KERNEL_VERSION_STRING_SIZE + 1 ;
    char* first_space = strchr(image->version_number,32);
    D("first_space : %p\n",first_space);
    
    D("image->version_number  : %s\n",image->version_number);
    image->version_number_length = first_space - image->version_number;
    D("image->version_number_length : %d\n",image->version_number_length);
    
    
    // find the config.gz if there is one
    image->config_addr = find_in_memory(uncompressed_kernel_data,uncompressed_kernel_size,KERNEL_IKCFG_ST,KERNEL_IKCFG_SIZE);
    if(image->config_addr){
	
	
	// Advance the config_addr position along to the start of the gzip "file"
	image->config_addr += KERNEL_IKCFG_SIZE;
	
	// find  the end
	unsigned char * config_end = find_in_memory_start_at(uncompressed_kernel_data , uncompressed_kernel_size ,image->config_addr, KERNEL_IKCFG_ED, KERNEL_IKCFG_SIZE);
	
	// if we can't find a KERNEL_IKCFG_ED then something is very wrong
	if(!config_end){
	    errno = EINVAL;
	    return_value =errno;
	    free(uncompressed_kernel_data);
	    goto exit;
	}
	
	
	image->config_size = config_end - image->config_addr ;
    }
    
    
    
    
exit:
    return return_value;
    
    
}
char *str_kernel_compression(int compression_type){
    
    
    
    switch(compression_type){
	case KERNEL_COMPRESSION_GZIP: return "gzip" ; 
	case KERNEL_COMPRESSION_LZMA: return "lzma" ; 
	case KERNEL_COMPRESSION_LZO: return "lzo" ; 
	defaut: return "none";
    }
    return "none";
}
