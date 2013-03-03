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

	

#define CPIO_TRAILER_MAGIC "TRAILER!!!"



static int qsort_comparer(const void* a, const void* b) {
  return strlcmp(*(const char**)a, *(const char**)b);
}

static unsigned long  get_long_from_hex_field(char * header_field_value){
	char buffer[9];
	strncpy(buffer, header_field_value,8 );
	buffer[8]='\0';
	return strtol(buffer,NULL,16);
}

cpio_entry_t populate_cpio_entry(const byte_p data ) { //cpio_newc_header_t *cpio_header) {

	cpio_entry_t *cpio_entry_p=(cpio_entry_t *)data;
	cpio_entry_t cpio_entry = (*cpio_entry_p);
	
	//	fprintf(stderr,"populate_cpio_entry\n");
	//cpio_entry.cpio_header=(cpio_newc_header_t)data;
	cpio_entry.entry_start_p=data;
	cpio_entry.file_size = get_long_from_hex_field(cpio_entry.cpio_header.c_filesize);
	cpio_entry.name_size = get_long_from_hex_field(cpio_entry.cpio_header.c_namesize);
	cpio_entry.mode = get_long_from_hex_field(cpio_entry.cpio_header.c_mode);
	unsigned long   name_align = ((4 - ((CPIO_HEADER_SIZE+cpio_entry.name_size) % 4)) % 4);
	unsigned long   header_align = (4 - ((CPIO_HEADER_SIZE+cpio_entry.name_size+name_align+cpio_entry.file_size) % 4)) % 4;
	cpio_entry.file_start_p=cpio_entry.entry_start_p+(CPIO_HEADER_SIZE+cpio_entry.name_size+name_align);
	cpio_entry.next_header_p= cpio_entry.entry_start_p+(CPIO_HEADER_SIZE+cpio_entry.name_size+name_align+cpio_entry.file_size+header_align);
	cpio_entry.file_name = (char *)data+CPIO_HEADER_SIZE;
	cpio_entry.is_directory=S_ISDIR(cpio_entry.mode);
	cpio_entry.is_file=S_ISREG(cpio_entry.mode);
	cpio_entry.is_link=S_ISLNK(cpio_entry.mode);
	char *strslash = strrchr(cpio_entry.file_name,'/');
	if(strslash){
		cpio_entry.parent_directory_length= strslash-cpio_entry.file_name;
	}else
		cpio_entry.parent_directory_length=-1;
	cpio_entry.is_trailer = !strlcmp(cpio_entry.file_name,CPIO_TRAILER_MAGIC);
	return cpio_entry;
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

byte_p modify_ramdisk_entry(const byte_p cpio_data,size_t *new_cpio_size,cpio_entry_t cpio_entry){
	
	const byte_p cpio_end  = cpio_data+(*new_cpio_size);
		fprintf(stderr,"cpio_end %p %u\n",cpio_end,(*new_cpio_size));
			struct stat sb;
			if(lstat(option_values.file_list[0], &sb)< 0){
				fprintf(stderr,"cannot stat %s %u\n",cpio_end,(*new_cpio_size));
				return NULL;
			}
				
			
			size_t new_file_size = 0 ;
			byte_p new_file_data = load_file(cpio_entry.file_name,&new_file_size);
			
			if( (CONVERT_LINE_ENDINGS) && (is_ascii_text(new_file_data,new_file_size))){
				byte_p output_buffer=malloc(new_file_size);
				size_t times = dos_to_unix(output_buffer , new_file_data) ; 
				new_file_size-= times ;
				free(new_file_data);
				new_file_data=output_buffer;
			}
			
			
			long aligned_file_size=new_file_size + ((4 - ((new_file_size) % 4)) % 4);
			unsigned long internal_new_cpio_size = (*new_cpio_size) +(aligned_file_size - cpio_entry.file_size) ;
			(*new_cpio_size)=internal_new_cpio_size;
			//fprintf(stderr,"new_cpio_size %p %ld\n",(*new_cpio_size));
			byte_p new_cpio_data = malloc(internal_new_cpio_size);
			// copy all data upto current entry
	
			memcpy(new_cpio_data,cpio_data,cpio_entry.entry_start_p-cpio_data);
			byte_p next_p = new_cpio_data+(cpio_entry.entry_start_p-cpio_data);
			
			sb.st_size=new_file_size;
			// write a new header block.
			append_cpio_header_to_stream(sb,cpio_entry.file_name,next_p); 
			next_p += cpio_entry.file_start_p-cpio_entry.entry_start_p;
			
			// 
			memcpy(next_p,new_file_data,aligned_file_size); 
			next_p += aligned_file_size;
			memcpy(next_p, cpio_entry.next_header_p, cpio_end-cpio_entry.next_header_p );
			
			free(new_file_data);
			return new_cpio_data;
}
byte_p extract_cpio_entry(cpio_entry_t cpio_entry,char * target_filename)
{

	if(!strlcmp(cpio_entry.file_name,CPIO_TRAILER_MAGIC)){	
		return NULL;
	}
	if(cpio_entry.is_directory){
		mkdir_and_parents(target_filename,cpio_entry.mode);
	}else if(cpio_entry.is_file){
		if(cpio_entry.parent_directory_length){
			char ch = cpio_entry.file_name[cpio_entry.parent_directory_length+1];
			cpio_entry.file_name[cpio_entry.parent_directory_length+1]='\0';
			mkdir_and_parents(cpio_entry.file_name,0755);
			cpio_entry.file_name[cpio_entry.parent_directory_length+1]=ch;
			
		}
		if( (CONVERT_LINE_ENDINGS) && (is_ascii_text(cpio_entry.file_start_p, cpio_entry.file_size ))){
				byte output_buffer[cpio_entry.file_size*2];
				cpio_entry.file_size += unix_to_dos((byte_p)&output_buffer,cpio_entry.file_start_p);
				//log_write("converting line endings\n");
				write_to_file_mode(output_buffer, cpio_entry.file_size,target_filename,cpio_entry.mode);
		}else{
			write_to_file_mode(cpio_entry.file_start_p,cpio_entry.file_size,target_filename,cpio_entry.mode);
		}
			//write_to_file_mode(cpio_entry.file_start_p,cpio_entry.file_size,cpio_entry.file_name,cpio_entry.mode);
		
	}
	else if(cpio_entry.is_link){
		char symlink_src[cpio_entry.file_size+1];
		memcpy(symlink_src,(const char*)cpio_entry.file_start_p,cpio_entry.file_size);
		symlink_src[cpio_entry.file_size] ='\0';
		symlink(symlink_src,target_filename);	
	}	
	return cpio_entry.next_header_p;
}
	

int process_uncompressed_ramdisk(const byte_p cpio_raw_data ,unsigned cpio_raw_data_size, char  *ramdisk_dirname)
{
	long current_cpio_entry_offset  = 0;
	mkdir(ramdisk_dirname,0777);
	char cwd[PATH_MAX];
	getcwd(cwd,PATH_MAX);
	chdir(ramdisk_dirname);
	//fprintf(stderr,"process_uncompressed_ramdisk\n");
	byte_p entry = cpio_raw_data;
	while(entry){
		cpio_entry_t cpio_entry = populate_cpio_entry(entry);	
		entry = extract_cpio_entry(cpio_entry,cpio_entry.file_name);
											
	}
	chdir(cwd);
	return 0;
}	
static unsigned long pack_ramdisk_entries(char *dir,char *path,byte_p output_buffer)
{
	log_write("pack_ramdisk_entries:%s\n",path);
	
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
	//if((dp = opendir(dir)) == NULL) 
		//log_write("cannot open directory: %s\n", dir);
		
	chdir(dir);
	getcwd(cwd,PATH_MAX);
	//fprintf(stderr,"cwd: %s\n", cwd);
	while((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name,&statbuf);
		char full_name[PATH_MAX];
		full_name[0] = '\0';
		strncpy(full_name,path,PATH_MAX); 
		strcat(full_name,entry->d_name);
		name_size = strlen(full_name)+1;
		bytes_to_file_start = (4 - ((CPIO_HEADER_SIZE+name_size) % 4)) % 4;
		file_start = bytes_to_file_start + CPIO_HEADER_SIZE+name_size;
	
		printf("offset:%ld:%s %08x\n",offset,entry->d_name,statbuf.st_mode);
		if(S_ISDIR(statbuf.st_mode)) {
			/* Found a directory, but ignore . and .. */
			if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
					continue;
			bytes_to_next_header_start =(4 - ((file_start+0) % 4)) % 4;
		//	append_cpio_header_to_stream(statbuf,full_name,name_size,output_buffer+offset);
			strncat(full_name,"/",PATH_MAX );
			next_header = file_start+0+bytes_to_next_header_start;
			offset +=next_header;
			pack_ramdisk_entries(entry->d_name,full_name,output_buffer);
		}
		else if(S_ISREG(statbuf.st_mode)){
			printf("Reg:%s %d\n",entry->d_name,statbuf.st_mode);
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
		//	append_cpio_header_to_stream(statbuf,full_name,name_size,output_buffer+offset);
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
			//append_cpio_header_to_stream(statbuf,full_name,name_size,output_buffer+offset);
			memmove(output_buffer+offset+file_start,data,file_size);
			offset +=next_header;

		}
		
	}
	chdir("..");
	closedir(dp);
	return offset;
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
unsigned long pack_ramdisk_directory(byte_p ramdisk_cpio_data){
	
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
	names[total]=CPIO_TRAILER_MAGIC;
	total++;
	chdir(option_values.ramdisk_directory_name);
	byte_p nextbyte = &ramdisk_cpio_data[0];
	for(i = 1; i < total; i++) {
		
		//fprintf(stderr,"Out:%p %s\n", nextbyte,nextbyte);
		struct stat sb ; lstat(names[i],&sb);
		byte_p start_header=nextbyte;
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
		fprintf(stderr,"names[%d]: %p %s %d\n",i,names[i] ,names[i], strlen(names[i])+4);
		
	}
	struct stat s ;	 memset(&s, 0, sizeof(s));
	nextbyte =append_cpio_header_to_stream(s,names[i+1] ,nextbyte);
	//fprintf(stderr,"done:%p %s %d\n",ramdisk_cpio_data,ramdisk_cpio_data,nextbyte-ramdisk_cpio_data);
	//fprintf(stderr,"done:%p %s %d\n",nextbyte-100,nextbyte-100,nextbyte-ramdisk_cpio_data);
	//free(outputstream);
	chdir(names[0]);
	write_to_file(ramdisk_cpio_data,nextbyte-ramdisk_cpio_data,"test.cpio");
	free(names);
    return nextbyte-ramdisk_cpio_data ;
}
