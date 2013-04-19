#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <utils.h>
#include <md5.h>



int  utils_debug;

void init_debug() {
    utils_debug = 1; 

}

/* NAME
	find_in_memory - locate a substring

 SYNOPSIS
      #include <utils.h>

      unsigned char * find_in_memory(unsigned char *haystack, size_t haystacklen,
					char *needle, size_t needlelen);

 DESCRIPTION
       The find_in_memory() function finds the start of the first occurrence of the substring needle of length needlelen in the memory area haystack of length haystacklen.

 RETURN VALUE
       The find_in_memory() function returns a pointer to the beginning of the substring, or NULL if the substring is not found.
       
       If an error occurs er 
       
*/
unsigned char *find_in_memory(unsigned char *haystack, unsigned haystack_len, char* needle, unsigned needle_len){
	
	
	if(!haystack)
	    errno = ENOMEM ;
	else if(!needle)
	    errno = ENOMEM ;
	else if(haystack_len < needle_len || !haystack_len || !needle_len)
	    errno = EINVAL ;
	
	if(errno) return NULL ;
	
	
	
	size_t begin=0;
	unsigned char* uneedle = (unsigned char *)needle ;
	//D("find_in_memory haystack=%p haystack_len=%u needle=%p needle_len=%u\n",haystack,haystack_len,needle,needle_len);
	//fprintf(stderr,"Memory HS:%p HL:%u\n",haystack,	haystack_len);
	//D("haystack[0]='%x' needle[0]='%x'\n",haystack[0],uneedle[0]);
	for(begin=0 ; begin < haystack_len; begin++){
		// make sure we are comparing apples with apples
		if(haystack[begin]==uneedle[0]){
			
			 if(!memcmp(uneedle,haystack+begin,needle_len)){
			     //D("haystack[%d]='%x'\n",begin,haystack[begin]);
			  return haystack+begin;
		      }
		}
	}
	//fprintf(stderr,"Memory Not Found\n");
	return NULL;
}
unsigned char *find_in_memory_start_at(unsigned char *haystack, unsigned haystack_len,unsigned char *haystack_offset, char* needle, unsigned needle_len){
	
	unsigned offset_difference = haystack_offset - haystack;
	//fprintf(stderr,"offset_difference %u\n",offset_difference);
	return find_in_memory(haystack_offset, haystack_len - offset_difference ,needle, needle_len);
}
// a simple wrapper to handle filename with out discovered lengths, keeps strlen all in one place

unsigned long write_item_to_disk(unsigned char *data,unsigned data_size,unsigned mode,char* name){
	
	errno = 0 ;
	if(!name){
	    errno = EINVAL ;
	    return errno;
	}
	return write_item_to_disk_extended(data,data_size,mode,name,strlen(name));
}


// write_file_to_disk - handles the extraction of the parent path creation if required 
unsigned long write_item_to_disk_extended(unsigned char *data,unsigned data_size,unsigned mode,char* name,unsigned name_size){
	
	errno = 0; 
	D("mode: %u %08x %d %s\n",mode,mode,S_ISDIR(mode), name);
	D("is dir %s\n",name);
	if(S_ISDIR(mode)){
	    
		mkdir_and_parents(name,mode);
	}else{

	    char * directory_seperator = strrchr(name,'/');
	    D("directory_seperator %p\n",directory_seperator);
	    if(directory_seperator){
		    // a cheeky bit of string manipulation to create a directory
		    (*directory_seperator) ='\0';
		    //fprintf(stderr,"directory: %s\n",name);
		    mkdir_and_parents(name,0777);
		    (*directory_seperator) ='/';
	    }
	    if(S_ISREG(mode)){
		    FILE* filefp = fopen(name,"wb");
		    if(!filefp)	return errno;
		    fwrite(data,data_size,1,filefp);
		    fclose(filefp);
		    chmod(name,mode);
	    }else if(S_ISLNK(mode)){
		    symlink_os(	(const char*)data, data_size ,name);			
	    }
			    
	}
	return errno;
}

unsigned char* read_regular_file_from_disk(const char *name, unsigned* data_size, unsigned size ){
    
    unsigned char *data =NULL;
    D("name=%s data_size=%u size=%u\n",name,*data_size,size);    
	
    data = 0;
    errno = 0;
    FILE* fp = fopen(name, "r+b");
    if(!fp) return NULL;

    data = calloc(size,sizeof(char));
    if(data == 0) {
	errno = ENOMEM; 
	goto oops;
    }
	
    if(fread(data, 1,size, fp) != size)  goto oops;
   // fprintf(stderr,"read_item_from_disk data %p\n",data);
    fclose(fp);

    if(!(*data_size)) *data_size = size;
    
    return data;

oops:
    fclose(fp);
    if(data != 0) free(data);
    return NULL;
}

unsigned char* read_item_from_disk(const char *name, unsigned* data_size){
    
    D("name=%s data_size=%u\n",name,(*data_size));    
        
    errno = 0 ;
    struct stat sb;
    if (stat(name, &sb) == -1) {
	D("stat(%s)=-1 errno=%d\n",name,errno); 
	return NULL;
    }
    D("mode %u\n",sb.st_mode & S_IFMT); 
    switch (sb.st_mode & S_IFMT) {
       case S_IFBLK:  return read_from_block_device(name,data_size);	break;
       case S_IFLNK:
       case S_IFREG:  return read_regular_file_from_disk(name,data_size, sb.st_size );	break;
       case S_IFSOCK: printf("socket\n");                  break;
       case S_IFCHR:  printf("character device\n");        break;
       case S_IFDIR:  printf("directory\n");               break;
       case S_IFIFO:  printf("FIFO/pipe\n");               break;
       default:       printf("unknown?\n");                break;
           }
    return NULL;
   
	
}
unsigned long  get_long_from_hex_field(char * header_field_value){
	char buffer[9];
	strncpy(buffer, header_field_value,8 );
	buffer[8]='\0';
	return strtol(buffer,NULL,16);
}
unsigned strlcmp(const char *s1, const char *s2){
	
	if(!s1 || !s2 )
		return -99;
	
	unsigned string_one_length=strlen(s1);
	unsigned string_two_length=strlen(s2);
	unsigned compare_length = string_one_length > string_two_length ?
								string_one_length : string_two_length;
	
	return strncmp(s1,s2,compare_length); 
								
}
unsigned strulcmp(const unsigned char *s1, const unsigned char *s2){
	
	if(!s1 || !s2 )
		return -99;
	
	unsigned string_one_length=strlen((char*)s1);
	unsigned string_two_length=strlen((char*)s2);
	unsigned compare_length = string_one_length > string_two_length ?
								string_one_length : string_two_length;
	
	return strncmp((const char*)s1,(const char*)s2,compare_length); 
								
}
char* get_md5_sum(unsigned char* data ,unsigned size) {
    
    struct MD5Context ctx;
    unsigned char* digest = calloc(MD5LENGTH,sizeof(char));

    MD5Init( &ctx );

    MD5Update( &ctx, data, size );

    MD5Final( digest, &ctx );

    D( "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", 
	digest[0],digest[1],digest[2],digest[3],
	digest[4],digest[5],digest[6],digest[7],
	digest[8],digest[9],digest[10],digest[11],
	digest[12],digest[13],digest[14],digest[15] );

    return (char*)digest;
}
int is_md5_match(unsigned char* data_a ,unsigned size_a,unsigned char* data_b ,unsigned size_b) {

    
    char * digest_a =get_md5_sum(data_a,size_a);
    char * digest_b =get_md5_sum(data_b,size_b);
    return !strncmp(digest_a,digest_b,MD5LENGTH);
    
}