#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include "ramdisk.h"
#include "program.h"	
#include "file.h"	
#include "../../zlib/zlib.h"
#include <private/android_filesystem_config.h>
#include "line_endings.h"

	
#define CPIO_HEADER_SIZE sizeof(cpio_newc_header_t)
#define CPIO_TRAILER_MAGIC "TRAILER!!!"
#define CPIO_TRAILER_MAGIC_LENGTH strlen("TRAILER!!!")
typedef struct {
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
	   } cpio_newc_header_t;

typedef struct  {
	cpio_newc_header_t cpio_header;
	unsigned long  	file_size ;
	unsigned long  	name_size ;
	mode_t  	mode ;
	unsigned long  	bytes_to_file_start ;
	unsigned long  	file_start ;
	unsigned long  	bytes_to_next_header_start ;
	unsigned long  	next_header ;
	char * file_name ;
}cpio_entry_t;


static unsigned long  get_long_from_hex_field(char * header_field_value){
	char buffer[9];
	strncpy(buffer, header_field_value,8 );
	buffer[8]='\0';
	return strtol(buffer,NULL,16);
}
static cpio_entry_t populate_cpio_entry(unsigned char *data ) { //cpio_newc_header_t *cpio_header) {

	cpio_entry_t cpio_entry ;
	//	fprintf(stderr,"populate_cpio_entry\n");
	memcpy(&cpio_entry.cpio_header,data, CPIO_HEADER_SIZE);
	
	cpio_entry.file_size = get_long_from_hex_field(cpio_entry.cpio_header.c_filesize);
	cpio_entry.name_size = get_long_from_hex_field(cpio_entry.cpio_header.c_namesize);
	cpio_entry.mode = get_long_from_hex_field(cpio_entry.cpio_header.c_mode);
	cpio_entry.bytes_to_file_start = (4 - ((CPIO_HEADER_SIZE+cpio_entry.name_size) % 4)) % 4;
	cpio_entry.file_start = cpio_entry.bytes_to_file_start + CPIO_HEADER_SIZE+cpio_entry.name_size;
	cpio_entry.bytes_to_next_header_start = (4 - ((cpio_entry.file_start+cpio_entry.file_size) % 4)) % 4;
	cpio_entry.next_header = cpio_entry.file_start+cpio_entry.file_size+cpio_entry.bytes_to_next_header_start;
	
	cpio_entry.file_name = data+CPIO_HEADER_SIZE;
	//fprintf(stderr," %s\n",cpio_entry.file_name);
	return cpio_entry;
}

static void append_cpio_header_to_stream(struct stat s,char *filename,unsigned file_size,int name_size,unsigned char *output_header){
	 static unsigned next_inode = 300000;
	 fs_config(filename, S_ISDIR(s.st_mode),(unsigned*) &s.st_uid, (unsigned*)&s.st_gid, (unsigned*)&s.st_mode);  
	 sprintf((char*)output_header,"%06x%08x%08x%08x%08x%08x%08x"
           "%08x%08x%08x%08x%08x%08x%08x%s",
           0x070701,
           next_inode++,  //  s.st_ino,
           s.st_mode,
           0, // s.st_uid,
           0, // s.st_gid,
           1, // s.st_nlink,
           0, // s.st_mtime,
           file_size,
           0, // volmajor
           0, // volminor
           0, // devmajor
           0, // devminor,
           name_size,
           0,filename
           );     
}

unsigned long uncompress_gzip_ramdisk_memory(unsigned char *data ,unsigned size,unsigned char *uncompressed_data,unsigned uncompressed_max_size)
{
	
	z_stream zInfo = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
	zInfo.avail_in=  size;
    zInfo.total_in=  size;  
    zInfo.avail_out=  uncompressed_max_size;
    zInfo.total_out=  uncompressed_max_size;
    zInfo.next_in= data	;
    zInfo.next_out= uncompressed_data;
    unsigned long err, return_value= -1;
    err= inflateInit2( &zInfo,16+MAX_WBITS );               // zlib function
    
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
unsigned long compress_gzip_ramdisk_memory( unsigned char *data , unsigned size,unsigned char *compressed_data,unsigned compressed_max_size)
{
   
    z_stream zInfo = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
    zInfo.total_in=  zInfo.avail_in=  size;
    zInfo.total_out= zInfo.avail_out= compressed_max_size;
    zInfo.next_in= data;
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

long find_file_in_ramdisk_entries(unsigned char *data,unsigned size,unsigned long offset)
{
	
	cpio_entry_t cpio_entry = populate_cpio_entry(data);	
	if(!strncmp(option_values.filename,cpio_entry.file_name,cpio_entry.name_size)){
		write_to_file_mode(data+cpio_entry.file_start,cpio_entry.file_size,option_values.output,cpio_entry.mode);
		return -1;
	}
	if(!strncmp(cpio_entry.file_name,CPIO_TRAILER_MAGIC,CPIO_TRAILER_MAGIC_LENGTH)){	
		return -1;
	}
	return cpio_entry.next_header+offset;
}	
long extract_cpio_entry(unsigned char *data,unsigned size,unsigned long offset)
{

	cpio_entry_t cpio_entry = populate_cpio_entry(data);	
	fprintf(stderr,"process_uncompressed_ramdisk:%lu %s  ",offset,cpio_entry.file_name);
	if(!strncmp(cpio_entry.file_name,CPIO_TRAILER_MAGIC,CPIO_TRAILER_MAGIC_LENGTH)){	
		return -1;
	}
	if(S_ISDIR(cpio_entry.mode)){
		fprintf(stderr,"directory\n");
		mkdir(cpio_entry.file_name,cpio_entry.mode);
	}else if(S_ISREG(cpio_entry.mode)){
		fprintf(stderr,"file\n");
			write_to_file_mode(data+cpio_entry.file_start,cpio_entry.file_size,cpio_entry.file_name,cpio_entry.mode);
		
	}
	else if(S_ISLNK(cpio_entry.mode)){
		fprintf(stderr,"link\n");
		char symlink_src[cpio_entry.file_size];
		memcpy(symlink_src,(const char*)data+cpio_entry.file_start,cpio_entry.file_size);
		symlink_src[cpio_entry.file_size] ='\0';
		symlink(symlink_src,cpio_entry.file_name);	
	}	
	return cpio_entry.next_header+offset;
}
int process_uncompressed_ramdisk(unsigned char *cpio_raw_data ,unsigned cpio_raw_data_size, char  *ramdisk_dirname)
{
	long current_cpio_entry_offset  = 0;
	mkdir(ramdisk_dirname,0777);
	char cwd[PATH_MAX];
	getcwd(cwd,PATH_MAX);
	chdir(ramdisk_dirname);
	fprintf(stderr,"process_uncompressed_ramdisk\n");
	while(current_cpio_entry_offset != -1){
		
		current_cpio_entry_offset = extract_cpio_entry(
											cpio_raw_data + current_cpio_entry_offset,
											cpio_raw_data_size - current_cpio_entry_offset,
											current_cpio_entry_offset);
	}
	chdir(cwd);
	return 0;
}	
static unsigned long pack_ramdisk_entries(char *dir,char *path,unsigned char* output_buffer)
{
	DIR *dp;
	char cwd[PATH_MAX];
	getcwd(cwd,PATH_MAX);
	struct dirent *entry;
	//fprintf(stderr,"cwd: %s\n", cwd);
	static unsigned long offset  =0;
	struct stat statbuf;
	unsigned long name_size =0;
	unsigned long  bytes_to_file_start =0;
	unsigned long  file_start =0;
	unsigned long  bytes_to_next_header_start = 0;
	unsigned long  next_header = 0;
	if((dp = opendir(dir)) == NULL) 
		fprintf(stderr,"cannot open directory: %s\n", dir);
		
		chdir(dir);
		getcwd(cwd,PATH_MAX);
		//fprintf(stderr,"cwd: %s\n", cwd);
		while((entry = readdir(dp)) != NULL) {
			lstat(entry->d_name,&statbuf);
			char full_name[PATH_MAX];
			full_name[0] = 0;
			strncpy(full_name,path,PATH_MAX); 
			strncat(full_name,entry->d_name,PATH_MAX );
			name_size = strlen(full_name)+1;
			bytes_to_file_start = (4 - ((CPIO_HEADER_SIZE+name_size) % 4)) % 4;
			file_start = bytes_to_file_start + CPIO_HEADER_SIZE+name_size;
		
			//printf("offset:%ld:%s\n",offset,entry->d_name);
			if(S_ISDIR(statbuf.st_mode)) {
				/* Found a directory, but ignore . and .. */
				if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
						continue;
				bytes_to_next_header_start =(4 - ((file_start+0) % 4)) % 4;
				append_cpio_header_to_stream(statbuf,full_name,0,name_size,output_buffer+offset);
				strncat(full_name,"/",PATH_MAX );
				next_header = file_start+0+bytes_to_next_header_start;
				offset +=next_header;
				pack_ramdisk_entries(entry->d_name,full_name,output_buffer);
			}
			else if(S_ISREG(statbuf.st_mode)){
				printf("Reg:%s %d\n",entry->d_name,statbuf.st_mode);
				unsigned long file_size =0 ;
				unsigned char* data = load_file(entry->d_name,&file_size);
				if(!strncmp((char*)data,"LNK:",4)){	
					free(data);
					file_size =0 ;
					data = load_file_from_offset(entry->d_name,4,&file_size);
					statbuf.st_mode = statbuf.st_mode | S_IFLNK ;
					statbuf.st_size = file_size ;
					
				}			
				bytes_to_next_header_start =(4 - ((file_start+file_size) % 4)) % 4;
				next_header = file_start+file_size+bytes_to_next_header_start;
				append_cpio_header_to_stream(statbuf,full_name,statbuf.st_size,name_size,output_buffer+offset);
				memmove(output_buffer+offset+file_start,data,file_size);
				offset +=next_header;
				free(data);

			} 
			else if(S_ISLNK(statbuf.st_mode)){
				unsigned char* data=calloc(MEMORY_BUFFER_SIZE, sizeof(unsigned char));
				readlink(entry->d_name,(char*)data,PATH_MAX);
				unsigned file_size =strlen((char*)data);
				//printf("link:%s %s %d %d\n",entry->d_name,data,statbuf.st_mode,file_size);
				bytes_to_next_header_start =(4 - ((file_start+file_size) % 4)) % 4;
				next_header = file_start+file_size+bytes_to_next_header_start;
				append_cpio_header_to_stream(statbuf,full_name,statbuf.st_size,name_size,output_buffer+offset);
				memmove(output_buffer+offset+file_start,data,file_size);
				offset +=next_header;

			}
			
		}
		chdir("..");
		closedir(dp);
		return offset;
}
unsigned long pack_ramdisk_directory(unsigned char* ramdisk_cpio_data){
	
	
	unsigned long  offset = pack_ramdisk_entries(option_values.ramdisk,"",ramdisk_cpio_data);
	
	struct stat s;
    memset(&s, 0, sizeof(s));
    unsigned long name_size = strlen("TRAILER!!!")+1;
    unsigned long  bytes_to_file_start = (4 - ((CPIO_HEADER_SIZE+name_size) % 4)) % 4;
	unsigned long  file_start = bytes_to_file_start + CPIO_HEADER_SIZE+name_size;
	unsigned long  bytes_to_next_header_start = (4 - ((file_start+0) % 4)) % 4;
	unsigned long  next_header = file_start+0+bytes_to_next_header_start;
    append_cpio_header_to_stream(s,"TRAILER!!!",0,name_size,ramdisk_cpio_data+offset);
    offset += next_header;
    return offset;
}
