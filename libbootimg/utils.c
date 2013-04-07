#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <utils.h>

unsigned char *find_in_memory(unsigned char *haystack, unsigned haystack_len, char* needle, unsigned needle_len){
	
	size_t begin=0;
	//fprintf(stderr,"Memory HS:%p HL:%u\n",haystack,	haystack_len);
	for(begin=0 ; begin < haystack_len; begin++){
		if(haystack[begin]==needle[0]){
			 if(!memcmp(needle,haystack+begin,needle_len)) return haystack+begin;
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

unsigned long write_item_to_disk(char *data,unsigned data_size,unsigned mode,char* name){
	
	errno = 0 ;
	if(!name){
		errno = EINVAL ;
		return errno;
	}
	return write_item_to_disk_extended(data,data_size,mode,name,strlen(name));
}


// write_file_to_disk - handles the extraction of the parent path creation if required 
unsigned long write_item_to_disk_extended(char *data,unsigned data_size,unsigned mode,char* name,unsigned name_size){
	
	errno = 0; 
	
	//fprintf(stderr,"mode: %u %08x %d %s\n",mode,mode,S_ISDIR(mode), name);
	//fprintf(stderr,"is dir %s\n",name);
	if(S_ISDIR(mode)){
	    
		mkdir_and_parents(name,mode);
	}else{

	    char * directory_seperator = strrchr(name,'/');
	    //fprintf(stderr,"directory_seperator %p\n",directory_seperator);
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
		    symlink_os(data, data_size ,name);			
	    }
			    
	}
	return errno;
	
		
	
}

unsigned char* read_item_from_disk(const char *name, unsigned* data_size){
    
    errno = 0 ;
    unsigned char *data =NULL;
    unsigned size;
    int fd;
	
    data = 0;
    
    //fprintf(stderr,"read_item_from_disk %s\n",name);
    struct stat sb;
	if (stat(name, &sb) == -1) {
		
		return errno;
	}
    size = sb.st_size; 
    //fprintf(stderr,"read_item_from_disk size %u\n",size);
    FILE* fp = fopen(name, "r+b");
   // fprintf(stderr,"read_item_from_disk fp %p\n",fp);
   // fprintf(stderr,"read_item_from_disk size %u\n",size);
    if(!fp) return NULL;

    data = calloc(size,sizeof(char));
    if(data == 0) {
	errno = ENOMEM; 
	goto oops;
    }
	
    if(fread(data, 1,size, fp) != size)  goto oops;
   // fprintf(stderr,"read_item_from_disk data %p\n",data);
    fclose(fp);

    if(data_size) *data_size = size;
    
    return data;

oops:
    fclose(fp);
    if(data != 0) free(data);
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
