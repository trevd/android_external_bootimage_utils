#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include "ramdisk.h"
#include "program.h"	
#include "file.h"	
#include "../../zlib/zlib.h"
#include <private/android_filesystem_config.h>

	
#define CPIO_HEADER_SIZE sizeof(cpio_newc_header_t)
#define CPIO_TRAILER_MAGIC "TRAILER!!!"
#define CPIO_TRAILER_MAGIC_LENGTH 11
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
	byte_p entry_start_p;
	byte_p file_start_p; 
	byte_p next_header_p;
	unsigned long entry_size ;  
	unsigned long  	next_header ;
	char * file_name ;
	int is_trailer ;
	int is_directory ; 
	int is_file ;
	int is_link ;
}cpio_entry_t;


static unsigned long  get_long_from_hex_field(char * header_field_value){
	char buffer[9];
	strncpy(buffer, header_field_value,8 );
	buffer[8]='\0';
	return strtol(buffer,NULL,16);
}
static cpio_entry_t populate_cpio_entry(const byte_p data ) { //cpio_newc_header_t *cpio_header) {

	cpio_entry_t *cpio_entry_p=(cpio_entry_t *)data;
	cpio_entry_t cpio_entry = (*cpio_entry_p);
	
	//	fprintf(stderr,"populate_cpio_entry\n");
	//cpio_entry.cpio_header=(cpio_newc_header_t)data;
	cpio_entry.entry_start_p=data;
	cpio_entry.file_size = get_long_from_hex_field(cpio_entry.cpio_header.c_filesize);
	cpio_entry.name_size = get_long_from_hex_field(cpio_entry.cpio_header.c_namesize);
	cpio_entry.mode = get_long_from_hex_field(cpio_entry.cpio_header.c_mode);
	cpio_entry.bytes_to_file_start = ((4 - ((CPIO_HEADER_SIZE+cpio_entry.name_size) % 4)) % 4);
	cpio_entry.file_start_p = data+(cpio_entry.bytes_to_file_start + CPIO_HEADER_SIZE+cpio_entry.name_size);
	cpio_entry.file_start = cpio_entry.bytes_to_file_start + CPIO_HEADER_SIZE+cpio_entry.name_size;
	cpio_entry.bytes_to_next_header_start = (4 - ((cpio_entry.file_start+cpio_entry.file_size) % 4)) % 4;
	cpio_entry.next_header = cpio_entry.file_start+cpio_entry.file_size+cpio_entry.bytes_to_next_header_start;
	cpio_entry.next_header_p = cpio_entry.file_start_p+(cpio_entry.file_size+cpio_entry.bytes_to_next_header_start); 
	cpio_entry.file_name = (char *)data+CPIO_HEADER_SIZE;
	cpio_entry.entry_size = cpio_entry.next_header_p  - cpio_entry.entry_start_p;
	cpio_entry.is_directory=S_ISDIR(cpio_entry.mode);
	cpio_entry.is_file=S_ISREG(cpio_entry.mode);
	cpio_entry.is_link=S_ISLNK(cpio_entry.mode);
	cpio_entry.is_trailer = !strlcmp(cpio_entry.file_name,CPIO_TRAILER_MAGIC);
	//fprintf(stderr," %s\n",cpio_entry.file_name);
	return cpio_entry;
}

static void append_cpio_header_to_stream(struct stat s,char *filename,int name_size,unsigned char *output_header){
	 static unsigned next_inode = 300000;
	 unsigned filesize = S_ISDIR(s.st_mode) ? 0 : s.st_size;
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
           filesize ,
           0, // volmajor
           0, // volminor
           0, // devmajor
           0, // devminor,
           name_size,
           0,filename
           );     
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
byte_p modify_ramdisk_entry(const byte_p cpio_data,const size_t cpio_size,size_t *new_cpio_size){
	cpio_entry_t cpio_entry = populate_cpio_entry(cpio_data);
	const byte_p cpio_end  = cpio_data+cpio_size;
	//write_to_file(cpio_data,cpio_size,"cpio_full");
	
	while(!cpio_entry.is_trailer){
		if(!strlcmp(option_values.target_filename,cpio_entry.file_name)){
			struct stat sb;
			//log_write("stat=%s:\n",option_values.source_filename); 
			lstat(option_values.source_filename, &sb); 
			size_t new_file_size = 0 ;
			byte_p new_file_data = load_file(option_values.source_filename,&new_file_size);
			
			if( (CONVERT_LINE_ENDINGS) && (is_ascii_text(new_file_data,new_file_size))){
				byte_p output_buffer=malloc(new_file_size);
				
				size_t times = dos_to_unix(output_buffer , new_file_data) ; 
				//log_write("convert:%s\n",buf);
				//log_write("nfs:%ld %ld\n",times, new_file_size);
				new_file_size-= times ;
				//log_write("nfs:%ld %ld\n",times, new_file_size);
				//log_write("nfd:%p\n", new_file_data);
				free(new_file_data);
				new_file_data=output_buffer;
				//log_write("nfd:%p\n", new_file_data);
			}
			
			// align the file_size and work out the new cpio_size
				//log_write("convert:%s",new_file_size);
			long aligned_file_size=new_file_size + ((4 - ((new_file_size) % 4)) % 4);
			unsigned long internal_new_cpio_size = cpio_size +(aligned_file_size - cpio_entry.file_size) ;
			(*new_cpio_size)=internal_new_cpio_size;
			byte_p new_cpio_data = malloc(internal_new_cpio_size);
			// copy all data upto current entry
			long bytes_before_entry = cpio_entry.entry_start_p-cpio_data;
			memcpy(new_cpio_data,cpio_data,bytes_before_entry);
			byte_p next_p = new_cpio_data+bytes_before_entry;
			//log_write("next_p:%p\n",next_p);
			sb.st_size=new_file_size;
			append_cpio_header_to_stream(sb,cpio_entry.file_name,strlen(cpio_entry.file_name)+1,next_p); 
			next_p += cpio_entry.file_start;
			//log_write("next_p:%p\n",next_p);
			memcpy(next_p,new_file_data,aligned_file_size); 
			next_p += aligned_file_size;
			memcpy(next_p,cpio_entry.next_header_p,cpio_end-cpio_entry.next_header_p);
			//write_to_file(new_cpio_data,internal_new_cpio_size,"new_cpio");
			//log_write("aligned_file_size=%ld file_start=%ld new_cpio_data=%p next_p=%p\n",aligned_file_size,cpio_entry.file_start ,new_cpio_data,next_p);
			char test[cpio_entry.file_start];
			free(new_file_data);
			return new_cpio_data;
			//write_to_file(cpio_data,cpio_entry.entry_start_p-cpio_data,"cpio_head");
			//write_to_file(cpio_entry.next_header_p,cpio_end-cpio_entry.next_header_p,"cpio_bot");
			
			//log_write("cpio_entry:es=%ld cpio_size=%ld file_name=%s file_diff:%ld \n",cpio_entry.entry_size,cpio_size,cpio_entry.file_name,file_diff);
			return 0; 			
		}else
				cpio_entry = populate_cpio_entry(cpio_entry.next_header_p);			
	}
	return cpio_data;
}
long find_file_in_ramdisk_entries(const byte_p data)
{
	cpio_entry_t cpio_entry = populate_cpio_entry(data);
	while(!cpio_entry.is_trailer){
			log_write("cpio_entry:source %s file_name=%s next_header:%p is_trailer:%d length:%d %d\n",option_values.source_filename,cpio_entry.file_name,cpio_entry.next_header_p,cpio_entry.is_trailer,strlen(cpio_entry.file_name),cpio_entry.name_size);	
			if(!strlcmp(option_values.source_filename,cpio_entry.file_name)){
				int ok_to_write=1	;
				if(check_file_exists(option_values.target_filename)){
					ok_to_write =confirm_file_replace(option_values.source_filename,option_values.target_filename);
				}
					
				
				if(!ok_to_write) return -1;	
				if( (CONVERT_LINE_ENDINGS) && (is_ascii_text(cpio_entry.file_start_p, cpio_entry.file_size ))){
					byte output_buffer[cpio_entry.file_size*2];
					cpio_entry.file_size += unix_to_dos((byte_p)&output_buffer,cpio_entry.file_start_p);
					//log_write("converting line endings\n");
					write_to_file_mode(output_buffer, cpio_entry.file_size,option_values.target_filename,cpio_entry.mode);
				}else{
						
					write_to_file_mode(cpio_entry.file_start_p,cpio_entry.file_size,option_values.target_filename,cpio_entry.mode);
				}
				return 0;
			}else
				cpio_entry = populate_cpio_entry(cpio_entry.next_header_p);				
	}
	//log_write("cpio_entry:file_name=%s next_header:%p is_trailer:%d length:%d\n",cpio_entry.file_name,cpio_entry.next_header_p,cpio_entry.is_trailer,strlen(cpio_entry.file_name));	
	return -1;

}	
long extract_cpio_entry(const byte_p data,unsigned size,unsigned long offset)
{

	cpio_entry_t cpio_entry = populate_cpio_entry(data);	
	//fprintf(stderr,"process_uncompressed_ramdisk:%lu %s ",offset,cpio_entry.file_name);
	if(!strlcmp(cpio_entry.file_name,CPIO_TRAILER_MAGIC)){	
		return -1;
	}
	if(cpio_entry.is_directory){
		//fprintf(stderr,"directory\n");
		mkdir(cpio_entry.file_name,cpio_entry.mode);
	}else if(cpio_entry.is_file){
		//fprintf(stderr,"file\n");
			write_to_file_mode(cpio_entry.file_start_p,cpio_entry.file_size,cpio_entry.file_name,cpio_entry.mode);
		
	}
	else if(cpio_entry.is_link){
		//fprintf(stderr,"link\n");
		char symlink_src[cpio_entry.file_size+1];
		memcpy(symlink_src,(const char*)cpio_entry.file_start_p,cpio_entry.file_size);
		symlink_src[cpio_entry.file_size] ='\0';
		symlink(symlink_src,cpio_entry.file_name);	
	}	
	return cpio_entry.next_header+offset;
}
int process_uncompressed_ramdisk(const byte_p cpio_raw_data ,unsigned cpio_raw_data_size, char  *ramdisk_dirname)
{
	long current_cpio_entry_offset  = 0;
	mkdir(ramdisk_dirname,0777);
	char cwd[PATH_MAX];
	getcwd(cwd,PATH_MAX);
	chdir(ramdisk_dirname);
	//fprintf(stderr,"process_uncompressed_ramdisk\n");
	while(current_cpio_entry_offset != -1){
		
		current_cpio_entry_offset = extract_cpio_entry(
											cpio_raw_data + current_cpio_entry_offset,
											cpio_raw_data_size - current_cpio_entry_offset,
											current_cpio_entry_offset);
	}
	chdir(cwd);
	return 0;
}	
static unsigned long pack_ramdisk_entries(char *dir,char *path,byte_p output_buffer)
{
	//log_write("pack_ramdisk_entries:%s\n",path);
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
		//log_write("cannot open directory: %s\n", dir);
		
	chdir(dir);
	getcwd(cwd,PATH_MAX);
	//fprintf(stderr,"cwd: %s\n", cwd);
	while((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name,&statbuf);
		char full_name[PATH_MAX];
		full_name[0] = 0;
		strncpy(full_name,path,PATH_MAX); 
		strcat(full_name,entry->d_name);
		name_size = strlen(full_name)+1;
		bytes_to_file_start = (4 - ((CPIO_HEADER_SIZE+name_size) % 4)) % 4;
		file_start = bytes_to_file_start + CPIO_HEADER_SIZE+name_size;
	
		//printf("offset:%ld:%s\n",offset,entry->d_name);
		if(S_ISDIR(statbuf.st_mode)) {
			/* Found a directory, but ignore . and .. */
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
					continue;
			bytes_to_next_header_start =(4 - ((file_start+0) % 4)) % 4;
			append_cpio_header_to_stream(statbuf,full_name,name_size,output_buffer+offset);
			strncat(full_name,"/",PATH_MAX );
			next_header = file_start+0+bytes_to_next_header_start;
			offset +=next_header;
			pack_ramdisk_entries(entry->d_name,full_name,output_buffer);
		}
		else if(S_ISREG(statbuf.st_mode)){
			//printf("Reg:%s %d\n",entry->d_name,statbuf.st_mode);
			size_t file_size =0 ;
			byte_p data = load_file(entry->d_name,&file_size);
			if(!strncmp((char*)data,"LNK:",4)){	
				free(data);
				file_size =0 ;
				data = load_file_from_offset(entry->d_name,4,&file_size);
				statbuf.st_mode = statbuf.st_mode | S_IFLNK ;
				statbuf.st_size = file_size ;
				
			}			
			bytes_to_next_header_start =(4 - ((file_start+file_size) % 4)) % 4;
			next_header = file_start+file_size+bytes_to_next_header_start;
			append_cpio_header_to_stream(statbuf,full_name,name_size,output_buffer+offset);
			memmove(output_buffer+offset+file_start,data,file_size);
			offset +=next_header;
			free(data);

		} 
		else if(S_ISLNK(statbuf.st_mode)){
			//printf("link:%s %d\n",entry->d_name,statbuf.st_mode);
			unsigned char* data=calloc(MEMORY_BUFFER_SIZE, sizeof(unsigned char));
			readlink(entry->d_name,(char*)data,PATH_MAX);
			unsigned file_size =strlen((char*)data);
			//printf("link:%s %s %d %d\n",entry->d_name,data,statbuf.st_mode,file_size);
			bytes_to_next_header_start =(4 - ((file_start+file_size) % 4)) % 4;
			next_header = file_start+file_size+bytes_to_next_header_start;
			append_cpio_header_to_stream(statbuf,full_name,name_size,output_buffer+offset);
			memmove(output_buffer+offset+file_start,data,file_size);
			offset +=next_header;

		}
		
	}
	chdir("..");
	closedir(dp);
	return offset;
}
unsigned long pack_ramdisk_directory(byte_p ramdisk_cpio_data){
	
	
	unsigned long  offset = pack_ramdisk_entries(option_values.ramdisk_directory_name,"",ramdisk_cpio_data);
	
	struct stat s;
    memset(&s, 0, sizeof(s));
    unsigned long name_size = CPIO_TRAILER_MAGIC_LENGTH;
    unsigned long  bytes_to_file_start = (4 - ((CPIO_HEADER_SIZE+name_size) % 4)) % 4;
	unsigned long  file_start = bytes_to_file_start + CPIO_HEADER_SIZE+name_size;
	unsigned long  bytes_to_next_header_start = (4 - ((file_start+0) % 4)) % 4;
	unsigned long  next_header = file_start+0+bytes_to_next_header_start;
    append_cpio_header_to_stream(s,CPIO_TRAILER_MAGIC,name_size,ramdisk_cpio_data+offset);
    offset += next_header;
    return offset;
}
