#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include "ramdisk.h"
#include "program.h"	
#include "file.h"	
#include "../../zlib/zlib.h"
#include <private/android_filesystem_config.h>

	

#define CPIO_TRAILER_MAGIC "TRAILER!!!"
#define DEFAULT_PROPERTIES_HEADER "#\x0A# ADDITIONAL_DEFAULT_PROPERTIES\x0A#\x0A"
#define DEFAULT_PROPERTIES_HEADER_LENGTH 45
default_property_list_t** get_default_properties(char *data, int* property_count)
{  
    int i=0; char *key, *value, *eol, *tmp, *equals , *sol;
    (*property_count)=0; equals=data;
    while((equals = strchr(++equals, '='))) (*property_count)++;
	
	default_property_list_t** properties=calloc((*property_count),sizeof(properties));	
	
	for(i = 0; i < (*property_count); i++) properties[i] =  calloc(1,sizeof(default_property_list_t));
	
	sol = data ; i=0 ;
	while((eol = strchr(sol, '\n'))) {
			key = sol;
			*eol++ = 0;
			sol = eol;
			value = strchr(key, '=');
			if(value == 0) continue;
			*value++ = 0;
			while(isspace(*key)) key++;
			if(*key == '#') continue;
			tmp = value - 2;
			while((tmp > key) && isspace(*tmp)) *tmp-- = 0;
			while(isspace(*value)) value++;
			tmp = eol - 2;
			while((tmp > value) && isspace(*tmp)) *tmp-- = 0;
			properties[i]->key = key;
			properties[i]->value = value;
			i++;
	}
	(*property_count)=i;
	return properties;
}


static int qsort_comparer(const void* a, const void* b) {
  return strlcmp(*(const char**)a, *(const char**)b);
}
static unsigned long  get_long_from_hex_field(char * header_field_value){
	char buffer[9];
	strncpy(buffer, header_field_value,8 );
	buffer[8]='\0';
	return strtol(buffer,NULL,16);
}

static void* append_cpio_header_to_stream(struct stat s,char *filename, byte_p output_header){
	 static unsigned next_inode = 300000;
	size_t  namesize=  strlen(filename)+1;
	 unsigned filesize = S_ISDIR(s.st_mode) ? 0 : s.st_size;
	 unsigned long namealign = ((4 - ((CPIO_HEADER_SIZE+namesize) % 4)) % 4);
	
	 uint64_t capabilities;
	 fs_config(filename, S_ISDIR(s.st_mode),(unsigned*) &s.st_uid, (unsigned*)&s.st_gid, (unsigned*)&s.st_mode,&capabilities);  
	
	 sprintf((char*)output_header,"%06x%08x%08x%08x%08x%08x%08x"
           "%08x%08x%08x%08x%08x%08x%08x%s",
           0x070701,
           next_inode++,  //  s.st_ino,
           s.st_mode,
           0, // s.st_uid,
           0, // s.st_gid,
           1, // s.st_nlink,
           0, // s.st_mtime,
           filesize ,
           0, // volmajor
           0, // volminor
           0, // devmajor
           0, // devminor,
		  namesize	,
           0,filename
           );     
    //fprintf(stderr,"namealign:%d %p %p\n",namealign,output_header,output_header);
    output_header+=(CPIO_HEADER_SIZE+namesize);
    strncat(output_header,"\0\0\0\0",namealign);
    //fprintf(stderr,"Out:%p\n", output_header);
    output_header+=namealign;
    return output_header;
}


size_t uncompress_gzip_ramdisk_memory(const byte_p compressed_data ,const size_t compressed_data_size,byte_p uncompressed_data,const size_t uncompressed_max_size)
{
	
	z_stream zInfo = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
	zInfo.avail_in=  compressed_data_size;
    zInfo.total_in=  compressed_data_size;  
    zInfo.avail_out=  uncompressed_max_size;
    zInfo.total_out=  uncompressed_max_size;
    zInfo.next_in= compressed_data	;
    zInfo.next_out= uncompressed_data;
    size_t return_value= 0;
    unsigned long err= inflateInit2( &zInfo,16+MAX_WBITS );               // zlib function
    
    if ( err == Z_OK ) {
        err= inflate( &zInfo, Z_FINISH );     // zlib function
        if ( err == Z_STREAM_END ) {
            return_value= zInfo.total_out;
        }else{ 
			       
			fprintf(stderr,"Err:inflate\n");
		}
    }else{
		fprintf(stderr,"Err:inflateInit\n");
	}
	inflateEnd( &zInfo );   
    return( return_value ); 
}
size_t compress_gzip_ramdisk_memory(const byte_p uncompressed_data , size_t uncompressed_data_size,byte_p compressed_data,size_t compressed_max_size)
{
   
    z_stream zInfo = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
    zInfo.total_in=  zInfo.avail_in=  uncompressed_data_size;
    zInfo.total_out= zInfo.avail_out= compressed_max_size;
    zInfo.next_in= uncompressed_data;
    zInfo.next_out= compressed_data;

    unsigned long err, return_value= -1;
    err=  deflateInit2(&zInfo, Z_DEFAULT_COMPRESSION, Z_DEFLATED,MAX_WBITS + 16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);	
    if ( err == Z_OK ) {
        err= deflate( &zInfo, Z_FINISH );              // zlib function
        if ( err == Z_STREAM_END ) {
            return_value= zInfo.total_out;
        }
    }
    deflateEnd( &zInfo );    // zlib function
    return( return_value );
}




char **names;
int total;

void listdir(const char *name, int level)
{
    DIR *dir;
    struct dirent *entry;
	
    if (!(dir = opendir(name)))
       return  ;
    if (!(entry = readdir(dir)))
       return  ;
    
    do {
		struct stat sb;	
		lstat(entry->d_name,&sb);
        if (S_ISDIR(sb.st_mode)) {
            char path[1024];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
			names[total]=strdup(path+5);
			total++;
            listdir(path, level + 1);
        }
        else{
			char path[1024];
			int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
				names[total]=strdup(path+5);
            total++;
		}   
    } while ((entry = readdir(dir)));
    closedir(dir);
   return  ;
}

static int get_cpio_entries_internal(byte_p ramdisk_cpio_data,size_t ramdisk_size,cpio_entry_list_t*** entriesp){
	
	
	cpio_entry_list_t** entries=(*entriesp); int count=0;
	byte_p next_value,start =ramdisk_cpio_data;
	if((next_value = (start=find_in_memory(start,ramdisk_size,magic_cpio_ascii,6)))){
		while(next_value){
			entries[count]->start.position=next_value;
			next_value+=CPIO_HEADER_SIZE;
			entries[count]->name=next_value;
			int name_size =strlen(entries[count]->name);
			entries[count]->name_padding=(((4 - (CPIO_HEADER_SIZE+name_size) % 4)) % 4);
			next_value+=entries[count]->name_padding+name_size;
			entries[count]->data=next_value;
			next_value=find_in_memory(next_value,ramdisk_size-(next_value-start),magic_cpio_ascii,6);
			if(!next_value){
				// No Values after this get the remaining padding.. This should be the trailer if not we have a problem
				entries[count]->data_size=ramdisk_size-(entries[count]->data-start); //ramend; //entries[count]->data-test;
			}else{
				entries[count]->data_size=next_value-entries[count]->data;
				entries[count]->next=next_value;
			}
			count++;
		}
	}
	return count;
}
// count the number of cpio entries. find the amount of times the cpio_magic occurs in ramdisk_data.
int count_cpio_entries(byte_p uncompressed_ramdisk_data,size_t uncompressed_ramdisk_size){
	byte_p next_value ; int cpio_entry_count=0;
	if((next_value = (find_in_memory(uncompressed_ramdisk_data,uncompressed_ramdisk_size,magic_cpio_ascii,6)))){
		while(next_value){
			cpio_entry_count++;
			next_value=find_in_memory(next_value+1,uncompressed_ramdisk_size-(next_value-uncompressed_ramdisk_data),magic_cpio_ascii,6);
		}
	}
	return cpio_entry_count;
}
// get_cpio_entries. return a list of cpio_entries. The outside calling function is responsible for freeing the memory allocated by
// this function. 
cpio_entry_list_t** get_cpio_entries(byte_p uncompressed_ramdisk_data,size_t uncompressed_ramdisk_size,int* cpio_entry_count ){
		
		(*cpio_entry_count) = count_cpio_entries(uncompressed_ramdisk_data,uncompressed_ramdisk_size);
		int i ; cpio_entry_list_t** cpio_entries=calloc((*cpio_entry_count),sizeof(cpio_entry_list_t**));		
		for(i = 0; i < (*cpio_entry_count); i++) {
			cpio_entries[i] =  calloc(1,sizeof(cpio_entry_list_t));;
			if (cpio_entries[i] == NULL) {
				perror("Memory cannot be allocated to arr[]");
			}
		}
		//fprintf(stderr,"Cpio_entry_count: %d\n",(*cpio_entry_count));
		(*cpio_entry_count) = get_cpio_entries_internal(uncompressed_ramdisk_data ,uncompressed_ramdisk_size,&cpio_entries);
		//fprintf(stderr,"Cpio_entry_count: %d\n",(*cpio_entry_count));
		return cpio_entries;
}
cpio_entry_list_t** get_cpio_entries_from_archive(byte_p ramdisk_data,size_t ramdisk_size,size_t* uncompressed_ramdisk_size ,int* cpio_entry_count ){
	byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
	(*uncompressed_ramdisk_size) =	uncompress_gzip_ramdisk_memory(ramdisk_data,ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
	cpio_entry_list_t** cpio_entries = get_cpio_entries(uncompressed_ramdisk_data,(*uncompressed_ramdisk_size),cpio_entry_count);
	//fprintf(stderr,"from archive\n");
	return cpio_entries;
}	
cpio_entry_list_t**  get_cpio_entries_from_file(FILE*boot_image_file,size_t ramdisk_size,size_t* uncompressed_ramdisk_size, int* cpio_entry_count){
	byte_p ramdisk_data=malloc(ramdisk_size);
	fread(ramdisk_data,1,ramdisk_size,boot_image_file);
	cpio_entry_list_t** cpio_entries=get_cpio_entries_from_archive(ramdisk_data,ramdisk_size,uncompressed_ramdisk_size,cpio_entry_count);
	//fprintf(stderr,"from file %d %p\n",(*cpio_entry_count),cpio_entries);
	//fprintf(stderr,"from file\n");
	free(ramdisk_data);
	return cpio_entries;
}
default_property_list_t** get_default_properties_from_file(FILE*boot_image_file,size_t ramdisk_size,int* default_property_count){
	size_t uncompressed_ramdisk_size; int cpio_entries_total; int i ; 
	cpio_entry_list_t** cpio_entries = get_cpio_entries_from_file(boot_image_file,ramdisk_size ,&uncompressed_ramdisk_size,&cpio_entries_total);
	default_property_list_t** default_properties; 
	for(i=0;i<cpio_entries_total;i++){
		if(!strlcmp("default.prop", cpio_entries[i]->name)){
			 default_properties=get_default_properties(cpio_entries[i]->data,default_property_count);
			break; 
		}
	}
	free_cpio_entry_memory(cpio_entries,cpio_entries_total);
	return default_properties;
}

void extract_cpio_list_entry(cpio_entry_list_t* cpio_entry){
	
	long mode = get_long_from_hex_field(cpio_entry->start.cpio_header->c_mode);
	long filesize = get_long_from_hex_field(cpio_entry->start.cpio_header->c_filesize);
	if(!strncmp(cpio_entry->name,CPIO_TRAILER_MAGIC,11)){	
		return ;
	}
	if(S_ISDIR(mode)){
		mkdir_and_parents(cpio_entry->name,mode);
	}else if(S_ISREG(mode)){
		char *strslash = strrchr(cpio_entry->name,'/');
		
		if(strslash){
			char ch = strslash[0];
			strslash[0]='\0';
			mkdir_and_parents(cpio_entry->name,0755);
			strslash[0]=ch;
			
		}
		if( (CONVERT_LINE_ENDINGS) && (is_ascii_text(cpio_entry->data, filesize))){
				char output_buffer[filesize*2];
				filesize += unix_to_dos((byte_p)&output_buffer,cpio_entry->data);
				//log_write("converting line endings\n");
				write_to_file_mode(output_buffer,filesize,cpio_entry->name,mode);
		}else{
			write_to_file_mode(cpio_entry->data,filesize,cpio_entry->name,mode);
		}
			//write_to_file_mode(cpio_entry.file_start_p,cpio_entry.file_size,cpio_entry.file_name,cpio_entry.mode);
		
	}
	else if(S_ISLNK(mode)){
		char symlink_src[filesize+1];
		memcpy(symlink_src,(const char*)cpio_entry->data,filesize);
		symlink_src[filesize] ='\0';
		symlink(symlink_src,cpio_entry->name );	
	}	
		
}
int free_cpio_entry_memory(cpio_entry_list_t** cpio_entries,int cpio_entries_total) {
	for(cpio_entries_total;cpio_entries_total=0;cpio_entries_total--){
			free(cpio_entries[cpio_entries_total-1]);
	}
	free(cpio_entries);	
	
return 0;
}
int free_default_properties_memory(default_property_list_t** properties,int properties_total) {
	for(properties_total;properties_total=0;properties_total--){
			free(properties[properties_total-1]);
	}
	free(properties);
	
return 0;
}
// get_contiguous_cpio_stream - create a contiguous stream from the cpio_entries list
// cpio_entries - a pointer to an array of cpio_entry_list_t struct pointers
// cpio_entries_total - the total size of the cpio_entries array
// size - the total amount contiguous memory required by the cpio_entries
byte_p get_contiguous_cpio_stream(cpio_entry_list_t** cpio_entries,int cpio_entries_total,size_t size){
		byte_p ramdisk_cpio_data_start;int i;
		byte_p ramdisk_cpio_data = ramdisk_cpio_data_start= calloc(size,sizeof(char));
		for(i=0;i<cpio_entries_total;i++){
			memcpy(ramdisk_cpio_data,cpio_entries[i]->start.position,CPIO_HEADER_SIZE);
			ramdisk_cpio_data+=CPIO_HEADER_SIZE;
			int name_length = strlen(cpio_entries[i]->name)+cpio_entries[i]->name_padding;
			memcpy(ramdisk_cpio_data,cpio_entries[i]->name,name_length);
			ramdisk_cpio_data+=name_length;
			memcpy(ramdisk_cpio_data,cpio_entries[i]->data,cpio_entries[i]->data_size);
			ramdisk_cpio_data+=cpio_entries[i]->data_size;
		}
		return ramdisk_cpio_data_start;
}
byte_p get_contiguous_default_properties(default_property_list_t** default_properties, int default_properties_total, size_t* default_prop_size){

	int i ;
	(*default_prop_size)+=DEFAULT_PROPERTIES_HEADER_LENGTH;
	for(i=0;i<default_properties_total;i++){
		(*default_prop_size)+=strlen(default_properties[i]->key);
		(*default_prop_size)+=strlen("=");
		(*default_prop_size)+=strlen(default_properties[i]->value);
	}
	byte_p default_prop_stream = calloc((*default_prop_size),sizeof(char));
	strncpy(default_prop_stream,DEFAULT_PROPERTIES_HEADER,DEFAULT_PROPERTIES_HEADER_LENGTH);
	for(i=0;i<default_properties_total;i++){
		strncat(default_prop_stream,default_properties[i]->key,strlen(default_properties[i]->key));
		strncat(default_prop_stream,"=",1);
		strncat(default_prop_stream,default_properties[i]->value,strlen(default_properties[i]->value));
		strncat(default_prop_stream,"\n",1);
	}
	return default_prop_stream;
}

// update_default_properties_in_gzip - updates the default property file found in <boot_image_file>
// boot_image_file - A valid android boot image which must contain a valid ramdisk.
// ramdisk_size - The size of the gzipped ramdisk in boot_image_file
// default_properties - a list of properties to written into the updated default.prop
// default_properties_total - the total number of default properties in the <default_properties> list 
// gzipped_ramdisk_size (out) - the size of the data returned
// Return - a pointer to the start of a gzip stream containing the updated default.prop
byte_p update_default_properties_in_gzip(FILE* boot_image_file,size_t ramdisk_size ,default_property_list_t** default_properties,int default_properties_total,size_t*gzipped_ramdisk_size){

	size_t default_prop_size=0;int i ; size_t uncompressed_ramdisk_size=0; int cpio_entries_total=0;
	byte_p default_prop_stream = get_contiguous_default_properties(default_properties,default_properties_total,&default_prop_size);
	
	cpio_entry_list_t** cpio_entries = get_cpio_entries_from_file(boot_image_file,ramdisk_size ,&uncompressed_ramdisk_size,&cpio_entries_total);
	//fprintf(stderr,"ramdisk size: %u uncompressed_ramdisk_size: %u gzipped_ramdisk_size: %u default_prop_size:%u\n ",ramdisk_size,uncompressed_ramdisk_size,gzipped_ramdisk_size,default_prop_size);
	for(i=0;i<cpio_entries_total;i++){
		if(!strlcmp("default.prop", cpio_entries[i]->name)){
			sprintf(cpio_entries[i]->start.cpio_header->c_filesize,"%08x",default_prop_size);
			cpio_entries[i]->data=default_prop_stream;
			uncompressed_ramdisk_size-=cpio_entries[i]->data_size;
			cpio_entries[i]->data_size=default_prop_size+(((4 - (default_prop_size) % 4)) % 4);
			uncompressed_ramdisk_size+=cpio_entries[i]->data_size; 
			break; 
		}
	}
	//fprintf(stderr,"ramdisk size: %u uncompressed_ramdisk_size: %u gzipped_ramdisk_size: %u default_prop_size:%u\n ",ramdisk_size,uncompressed_ramdisk_size,gzipped_ramdisk_size,default_prop_size);
	byte_p ramdisk_gzip_data = compress_cpio_entries_to_gzip( cpio_entries,cpio_entries_total,uncompressed_ramdisk_size,gzipped_ramdisk_size);
	//fprintf(stderr,"ramdisk size: %u uncompressed_ramdisk_size: %u gzipped_ramdisk_size: %u default_prop_size:%u\n ",ramdisk_size,uncompressed_ramdisk_size,gzipped_ramdisk_size,default_prop_size);
	free(default_prop_stream);
	free_cpio_entry_memory(cpio_entries,cpio_entries_total) ;
	return ramdisk_gzip_data;
}
byte_p compress_cpio_entries_to_gzip(cpio_entry_list_t** cpio_entries,int cpio_entries_total,size_t cpio_size,size_t* ramdisk_size){
	byte_p ramdisk_cpio_data = get_contiguous_cpio_stream(cpio_entries,cpio_entries_total,cpio_size);
	byte_p ramdisk_gzip_data = calloc(cpio_size, sizeof(unsigned char));
	(*ramdisk_size)=compress_gzip_ramdisk_memory(ramdisk_cpio_data,cpio_size,ramdisk_gzip_data,cpio_size);
	return ramdisk_gzip_data ;
}
int process_uncompressed_ramdisk(const byte_p cpio_raw_data ,unsigned cpio_raw_data_size, char  *ramdisk_dirname)
{
	int cpio_entries_total = 0;
	cpio_entry_list_t** cpio_entries = get_cpio_entries(cpio_raw_data ,cpio_raw_data_size,&cpio_entries_total);
	long current_cpio_entry_offset  = 0;
	mkdir(ramdisk_dirname,0777);
	char cwd[PATH_MAX];
	getcwd(cwd,PATH_MAX);
	chdir(ramdisk_dirname);
	int i;
	for(i = 0; i < cpio_entries_total; i++){
		extract_cpio_list_entry(cpio_entries[i]);
	}
	for(i=0;i<cpio_entries_total;i++)
		free(cpio_entries[i]);
	free(cpio_entries);		
	chdir(cwd);
	return 0;
}	
size_t pack_ramdisk_directory(byte_p ramdisk_cpio_data){
		
	int i; names = calloc(256, sizeof(names));
	for(i = 0; i < 256; i++) {
		names[i] = (char *)calloc(1024,sizeof(char));	
		if (names[i] == NULL) {
			perror("Memory cannot be allocated to arr[]");
		}
	}
	getcwd(names[0],1024);total++;
	listdir(option_values.ramdisk_directory_name, 0);
	qsort(names, total, sizeof(char*), qsort_comparer);
	chdir(option_values.ramdisk_directory_name);
	byte_p nextbyte = &ramdisk_cpio_data[0];
	byte_p start_header=nextbyte;
	for(i = 1; i < total; i++) {
		
		//fprintf(stderr,"Out:%p %s\n", nextbyte,nextbyte);
		struct stat sb ; lstat(names[i],&sb);
		
		nextbyte = append_cpio_header_to_stream(sb,names[i],start_header);
		//fprintf(stderr,"names[%d]: %p %s %d\n",i,names[i] ,names[i], strlen(names[i])+4);
		unsigned filesize = S_ISDIR(sb.st_mode) ? 0 : sb.st_size;
		unsigned long   header_align = (4 - (((nextbyte-start_header)+filesize) % 4)) % 4;
		fprintf(stderr,"Heade:%d %p %p %d %d\n",nextbyte-start_header, nextbyte,start_header,header_align,sb.st_size);
		if(S_ISREG(sb.st_mode)){
			nextbyte = load_file_easy(names[i],sb.st_size,&nextbyte);
		}else if(S_ISLNK(sb.st_mode)){
			readlink(names[i],nextbyte,PATH_MAX);
			nextbyte+=filesize;
		}
		nextbyte+=header_align;
		start_header=nextbyte;
		fprintf(stderr,"names[%d]: %p %s %d\n",i,names[i] ,names[i], strlen(names[i])+4);
		
	}
	struct stat s ;	 memset(&s, 0, sizeof(s));
	nextbyte =append_cpio_header_to_stream(s,CPIO_TRAILER_MAGIC,start_header);
	nextbyte+=( (4 - (((nextbyte-start_header)) % 4)) % 4);
	//fprintf(stderr,"done:%p %s %d\n",ramdisk_cpio_data,ramdisk_cpio_data,nextbyte-ramdisk_cpio_data);
	//fprintf(stderr,"done:%p %s %d\n",nextbyte-100,nextbyte-100,nextbyte-ramdisk_cpio_data);
	//free(outputstream);
	chdir(names[0]);
	write_to_file(ramdisk_cpio_data,nextbyte-&ramdisk_cpio_data[0],"test2.cpio");
	free(names);
    return nextbyte-ramdisk_cpio_data ;
}
