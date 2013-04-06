#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <utils.h>
#include <ramdisk.h>
#include <compression.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <private/android_filesystem_config.h>

#define CPIO_HEADER_MAGIC "070701"
#define CPIO_HEADER_MAGIC_SIZE 6
#define CPIO_TRAILER_MAGIC "TRAILER!!!"
#define CPIO_TRAILER_MAGIC_SIZE 10
typedef struct cpio_newc_header cpio_newc_header;

struct cpio_newc_header {
       char    c_magic[6];
       char    c_ino[8];
       char    c_mode[8];
       char    c_uid[8];
       char    c_gid[8];
       char    c_nlink[8];
       char    c_mtime[8];
       char    c_filesize[8];
       char    c_devmajor[8];
       char    c_devminor[8];
       char    c_rdevmajor[8];
       char    c_rdevminor[8];
       char    c_namesize[8];
       char    c_check[8];
} ;

#define MAX_RAMDISK_SIZE (8192*1024)*4

unsigned carve_out_entry_space(ramdisk_image* image){

    errno = 0;
    if(!image){
	errno = ENOEXEC;
	return -1;
    }
    if(!image->entry_count){
	errno = EINVAL ;
	return -1;
    }
    //fprintf(stderr,"DEBUG:carve_out_entry_space\n") ;   
    unsigned i; image->entries = calloc(image->entry_count, sizeof(ramdisk_entry*));
    for(i = 0; i < image->entry_count; i++) {
	image->entries[i] = calloc(1,sizeof(ramdisk_entry));	
	if (image->entries[i] == NULL) {
		break;
	    }
    }
       
    //fprintf(stderr,"DEBUG:carve_out_entry_space complete\n") ;   
    return errno; 

}
unsigned populate_ramdisk_entries(ramdisk_image* image){

     errno = 0;
     if(!image){
	errno = ENOEXEC;
	return -1;
    }
    if(!image->entry_count){
	errno = EINVAL ;
	return -1;
    }

    unsigned counter = 0;
    unsigned char * cpio_magic_offset_p = find_in_memory( image->start_addr, image->size,CPIO_HEADER_MAGIC, CPIO_HEADER_MAGIC_SIZE );
    //fprintf(stderr,"DEBUG:populate_ramdisk_entries\n") ;   
    while(cpio_magic_offset_p){
	//fprintf(stderr,"DEBUG:populate_ramdisk_entries  %p %d\n",cpio_magic_offset_p ,sizeof(ramdisk_entry)) ;   
	//fprintf(stderr,"%s %1c\n",cpio_magic_offset_p+CPIO_HEADER_MAGIC_SIZE,(cpio_magic_offset_p+CPIO_HEADER_MAGIC_SIZE)[0]);
	if(isdigit((cpio_magic_offset_p+CPIO_HEADER_MAGIC_SIZE)[0])){
	    
	
	
	    image->entries[counter]->start_addr = cpio_magic_offset_p;
	    
	    cpio_newc_header* header = (cpio_newc_header*)cpio_magic_offset_p;
	    
	    image->entries[counter]->mode = get_long_from_hex_field(header->c_mode);
	    image->entries[counter]->name_size = get_long_from_hex_field(header->c_namesize);
	    image->entries[counter]->name_addr = cpio_magic_offset_p+sizeof(cpio_newc_header);
	    image->entries[counter]->name_padding = ((4 - ((sizeof(cpio_newc_header) + image->entries[counter]->name_size ) % 4)) % 4);
	    image->entries[counter]->data_addr = image->entries[counter]->name_addr + 	image->entries[counter]->name_size + image->entries[counter]->name_padding ;
	    image->entries[counter]->data_size = get_long_from_hex_field(header->c_filesize);
	    image->entries[counter]->data_padding = ((4 - ((image->entries[counter]->data_size ) % 4)) % 4);
	    image->entries[counter]->next_addr = image->entries[counter]->data_addr + (image->entries[counter]->data_size + image->entries[counter]->data_padding);
	    
	    
	    counter += 1 ;
	}
	    // Get the next entry
	cpio_magic_offset_p+=CPIO_HEADER_MAGIC_SIZE;
	cpio_magic_offset_p = find_in_memory_start_at(image->start_addr,image->size,cpio_magic_offset_p,CPIO_HEADER_MAGIC, CPIO_HEADER_MAGIC_SIZE );
	
	// Double check we are not exceeding the allocated array size
	if( counter > image->entry_count) break;
    }
    //fprintf(stderr,"DEBUG:populate_ramdisk_entries complete\n") ;   
    return 0;
	    
}
static int qsort_comparer(const void* a, const void* b) {
  return strlcmp(*(const char**)a, *(const char**)b);
}

unsigned count_ramdisk_archive_entries(ramdisk_image* image){
    
    unsigned return_value = 0; 
    unsigned char * cpio_magic_offset_p = find_in_memory( image->start_addr, image->size,CPIO_HEADER_MAGIC, CPIO_HEADER_MAGIC_SIZE );
    while(cpio_magic_offset_p){
	if(isdigit((cpio_magic_offset_p+CPIO_HEADER_MAGIC_SIZE)[0])){
	    return_value += 1;	
	}
	cpio_magic_offset_p+=CPIO_HEADER_MAGIC_SIZE;
	cpio_magic_offset_p = find_in_memory_start_at(image->start_addr,image->size,cpio_magic_offset_p,CPIO_HEADER_MAGIC, CPIO_HEADER_MAGIC_SIZE );
    }
    return return_value;
    
}

int load_ramdisk_image_from_archive_file(const char *filename, ramdisk_image* image){
      
    return 0;
}
int load_ramdisk_image_from_cpio_file(const char *filename, ramdisk_image* image){
    return 0;
}
int load_ramdisk_image_from_cpio_memory(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image ){
    
    
    fprintf(stderr,"load_ramdisk_image_from_cpio_memory %u\n", ramdisk_size);
    // now we have uncompressed data check we have a cpio file look for the magic
    if(memcmp(ramdisk_addr,CPIO_HEADER_MAGIC,CPIO_HEADER_MAGIC_SIZE)){
	 fprintf(stderr,"CPIO_HEADER_MAGIC %u\n", ramdisk_size);
	return ENOEXEC;
    }
	
    // and also look for the trailer
    if(!find_in_memory(ramdisk_addr,ramdisk_size,CPIO_TRAILER_MAGIC,CPIO_TRAILER_MAGIC_SIZE)){
		 fprintf(stderr,"CPIO_TRAILER_MAGIC %u\n", ramdisk_size);
	return ENOEXEC;
    }
	
    image->size = ramdisk_size;
    image->start_addr = ramdisk_addr ;
    
    image->entry_count = count_ramdisk_archive_entries(image);
    
    if(!image->entry_count){
		 fprintf(stderr,"image->entry_count %u\n", image->entry_count);
	return EINVAL;
    }
    
    carve_out_entry_space(image);
    carve_out_entry_space(image);
    
    populate_ramdisk_entries(image);
    
    return 0;
}
int load_ramdisk_image_from_archive_memory(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image ){

fprintf(stderr,"load_ramdisk_image_from_archive_memory %u\n", ramdisk_size);
  
    errno = 0;
    // look for a gzip magic to make sure the ramdisk is the correct type
    unsigned char * gzip_magic_offset_p = find_in_memory(ramdisk_addr,ramdisk_size,GZIP_DEFLATE_MAGIC, GZIP_DEFLATE_MAGIC_SIZE );
    if(!gzip_magic_offset_p){
	return ENOEXEC;
	
	
    }
        
    image->compression_type = RAMDISK_COMPRESSION_GZIP ;
    
    unsigned char *uncompressed_ramdisk_data = calloc(MAX_RAMDISK_SIZE,sizeof(unsigned char)) ;
    
    
    
    unsigned uncompressed_ramdisk_size = uncompress_gzip_memory(gzip_magic_offset_p,ramdisk_size,uncompressed_ramdisk_data,MAX_RAMDISK_SIZE);
   
    if(!uncompressed_ramdisk_size){
	fprintf(stderr,"uncompressed_ramdisk_size error\n");
	free(uncompressed_ramdisk_data);
	return  EINVAL;
    }
    if(load_ramdisk_image_from_cpio_memory(uncompressed_ramdisk_data,uncompressed_ramdisk_size,image)){
	fprintf(stderr,"load_ramdisk_image_from_cpio_memory error\n");
	free(uncompressed_ramdisk_data);
	return  EINVAL;
    }
    fprintf(stderr,"load_ramdisk_image_from_archive_memory %u\n", image->entry_count);
    //free(uncompressed_ramdisk_data);
    return 0;

}
int save_ramdisk_entries_to_disk(ramdisk_image* image,unsigned char *directory_name){

    char cwd[PATH_MAX];
    errno = 0;
    if(directory_name){
	mkdir_and_parents(directory_name,0777);
	getcwd(cwd,PATH_MAX);
	chdir((char *)directory_name);
    }
    
    unsigned i = 0;
    fprintf(stderr,"ramdisk_entry_count %u\n", image->entry_count);
    for(i = 0 ; i < image->entry_count ; i++){
	int is_dir = S_ISDIR(image->entries[i]->mode);
	//fprintf(stderr,"%u:  %s %u %d\n", i,image->entries[i]->name_addr,image->entries[i]->mode,is_dir);
	write_item_to_disk_extended(image->entries[i]->data_addr,image->entries[i]->data_size,image->entries[i]->mode, image->entries[i]->name_addr,image->entries[i]->name_size);
	
    }
    
    if(directory_name){
	chdir(cwd);
    }
    return 0;
    
}
static unsigned total;
void count_filesystem_entries( char *name, int level){
    
    
    static unsigned total_size ;
    static int root_len; 
    if(level == 0 ) root_len = strlen(name)+1;
    DIR *dir;
    struct dirent *entry;
	
    if (!(dir = opendir(name)))
       return ;
    if (!(entry = readdir(dir)))
       return  ;
    
    do {
	struct stat sb;	
	lstat(name,&sb);
	
        if (S_ISDIR(sb.st_mode)) {
            char path[PATH_MAX];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
	    total_size += 0;
	    
	    //names[total]=strdup(path+root_len);
	    //fprintf(stderr,"names[%u]:%s\n",total,names[total]);
	    total++;
            count_filesystem_entries(path, level + 1);
        }
        else{
	    char path[PATH_MAX];
	    int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
	    //names[total]=strdup(path+root_len);
	    //fprintf("names[%d]:%s\n",total,names[total]);
	    total++;
	}   
    } while ((entry = readdir(dir)));
    closedir(dir);
   
   return  ;
}
static char ** names;

void get_filesystem_entry_names( char *name, int level){
    
    static unsigned total_size ;
    static int root_len; 
    if(level == 0 ) root_len = strlen(name)+1;
    DIR *dir;
    struct dirent *entry;
	
    if (!(dir = opendir(name)))
       return ;
    if (!(entry = readdir(dir)))
       return  ;
    
    do {
	struct stat sb;	
	lstat(name,&sb);
	
        if (S_ISDIR(sb.st_mode)) {
            char path[PATH_MAX];
            int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
	    total_size += 0;
	    
	    names[total]=strdup(path+root_len);
	    fprintf(stderr,"names[%u]:%s\n",total,names[total]);
	    total++;
            get_filesystem_entry_names(path, level + 1);
        }
        else{
	    char path[PATH_MAX];
	    int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
	    names[total]=strdup(path+root_len);
	    //fprintf("names[%d]:%s\n",total,names[total]);
	    total++;
	}   
    } while ((entry = readdir(dir)));
    closedir(dir);
   
   return  ;
}
static unsigned char* append_cpio_header_to_stream(struct stat s,char *filename, unsigned char* output_header){
	 static unsigned next_inode = 300000;
	 unsigned  namesize =  strlen(filename)+1;
	 unsigned filesize = S_ISDIR(s.st_mode) ? 0 : s.st_size;
	 unsigned long namealign = ((4 - ((sizeof(cpio_newc_header)+namesize) % 4)) % 4);
	
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
    fprintf(stderr,"namealign:%s %d %p %p\n",filename,namealign,output_header,output_header);
    output_header+=(sizeof(cpio_newc_header)+namesize);
    strncat(output_header,"\0\0\0\0",namealign);
    //fprintf(stderr,"Out:%p\n", output_header);
    output_header+=namealign;
    return output_header;
}
static unsigned char* append_file_contents_to_stream(struct stat s,char *filename, unsigned char* output_header){
    
    if(S_ISDIR(s.st_mode)){
    }else {
	unsigned long filealign = ((4 - ((s.st_size) % 4)) % 4);
	if(S_ISREG(s.st_mode)){
	    fprintf(stderr,"open reg %s\n",filename);
	    FILE* fp = fopen(filename,"r+b");
	    fread(output_header,s.st_size,1,fp);
	    output_header+=s.st_size;
	    fclose(fp);
	}else if(S_ISLNK(s.st_mode)){
	    
	    readlink(filename,output_header,PATH_MAX);
	    
	    output_header+=s.st_size;
	}
	output_header+=filealign;
    }
    
    return output_header; 
    
}

unsigned char *pack_ramdisk_directory(char* directory_name, unsigned *cpio_size){
    
    
    // allocate the memory required for the filenames list
    
    unsigned filesystem_entries =0 ;
    count_filesystem_entries(directory_name,0);
    fprintf(stderr,"filesystem_entries %u %u\n",filesystem_entries,total);
    filesystem_entries = total;
    int i; names = calloc(filesystem_entries, sizeof(names));
    for(i = 0; i < filesystem_entries; i++) {
	names[i] = (char *)calloc(PATH_MAX,sizeof(char));	
	if (names[i] == NULL) {
		perror("Memory cannot be allocated to arr[]");
	}
	 fprintf(stderr,"i: %d \n",i);
    }
    total = 0;
    get_filesystem_entry_names(directory_name,0);
    qsort(names, filesystem_entries, sizeof(char*), qsort_comparer);

    
   /* unsigned total_size = 0 ; 
    char cwd[PATH_MAX];
    getcwd(cwd,PATH_MAX);
    chdir(directory_name);
    for(i = 0; i < filesystem_entries; i++){
	struct stat sb;
	if(lstat(names[i],&sb) == -1)
	    continue;
	//fprintf(stderr,"name %s  size: %u\n",names[i] , sb.st_size);	
	total_size += sizeof(cpio_newc_header);
	total_size += strlen(names[i]); 
	total_size += ((4 - ((sizeof(cpio_newc_header) + strlen(names[i]) ) % 4)) % 4);
	if(!S_ISDIR(sb.st_mode)){  
	    total_size += sb.st_size ; 
	     total_size += ((4 - ((sb.st_size ) % 4)) % 4);
	}
	
    }
    fprintf(stderr,"filesystem_entries %s %u  size: %u %u\n",cwd,filesystem_entries,total_size,PATH_MAX);	
    chdir(cwd) ; */
    
    char cwd[PATH_MAX];
    getcwd(cwd,PATH_MAX);
    chdir(directory_name);
    unsigned char* cpio_data = calloc(MAX_RAMDISK_SIZE,sizeof(char));
    unsigned char* nextbyte = &cpio_data[0];
    for(i = 0; i < filesystem_entries; i++){
	
	struct stat sb;
	if(lstat(names[i],&sb) == -1)
	    continue;
	fprintf(stderr,"nextbyte %p\n",nextbyte);
	nextbyte = append_cpio_header_to_stream(sb,names[i],nextbyte);
	nextbyte = append_file_contents_to_stream(sb,names[i],nextbyte);
	
    }
    struct stat s ; memset(&s, 0, sizeof(s));
    nextbyte =append_cpio_header_to_stream(s,CPIO_TRAILER_MAGIC,nextbyte);
    
     chdir(cwd) ;
    (*cpio_size) = nextbyte - &cpio_data[0] ;
    // align the file size to the next 256 boundary
    while((*cpio_size) & 0xff) {
        (*cpio_size)++;
        
    }
    
    //fprintf(stderr,"file_size %u %p\n",file_size,nextbyte);
    
    return cpio_data;
    
    
    
}
int print_ramdisk_info(ramdisk_image* rimage){
    
    fprintf(stderr,"\nramdisk_image struct values:\n");
    fprintf(stderr,"  start_addr       :%p\n",rimage->start_addr);
    fprintf(stderr,"  size             :%u\n",rimage->size);
    fprintf(stderr,"  compression_type :%s\n",str_ramdisk_compression(rimage->compression_type));
    fprintf(stderr,"  type             :%s\n",str_ramdisk_type(rimage->type));
    fprintf(stderr,"  entry_count      :%u\n",rimage->entry_count);
    fprintf(stderr,"  entries          :%p\n",rimage->entries);
    
    return 0;
}
char *str_ramdisk_type(int ramdisk_type){
    
    switch(ramdisk_type){
	case RAMDISK_TYPE_UNKNOWN: return "unknown" ; 
	case RAMDISK_TYPE_NORMAL: return "android" ; 
	case RAMDISK_TYPE_RECOVERY: return "recovery" ; 
	case RAMDISK_TYPE_UBUNTU: return "ubuntu";
	default: return "unknown";
    }
    return "unknown";
}

char *str_ramdisk_compression(int compression_type){
  
  switch(compression_type){
	case RAMDISK_COMPRESSION_GZIP: return "gzip" ; 
	case RAMDISK_COMPRESSION_LZMA: return "lzma" ; 
	case RAMDISK_COMPRESSION_LZO: return "lzo" ; 
	case RAMDISK_COMPRESSION_BZIP: return "bz2";
	default: return "none";
    }
    return "unknown";
    
}
