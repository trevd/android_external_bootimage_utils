#include <string.h>
#include <utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
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
unsigned long  get_long_from_hex_field(char * header_field_value){
	char buffer[9];
	strncpy(buffer, header_field_value,8 );
	buffer[8]='\0';
	return strtol(buffer,NULL,16);
}
unsigned strlcmp(const unsigned char *s1, const unsigned char *s2){
	
	if(!s1 || !s2 )
		return -99;
	
	unsigned string_one_length=strlen((char*)s1);
	unsigned string_two_length=strlen((char*)s2);
	unsigned compare_length = string_one_length > string_two_length ?
								string_one_length : string_two_length;
	
	return strncmp((const char*)s1,(const char*)s2,compare_length); 
								
}
