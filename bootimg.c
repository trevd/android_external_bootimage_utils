/* tools/mkbootimg/mkbootimg.c
**
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include "file.h"
#include "program.h"
#include "mincrypt/sha.h"

#include "bootimg.h"
#include "ramdisk.h"


/*void strrev(char *s) {
  int tmp, i, j;
  for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
    tmp = s[i];
    s[i] = s[j];
    s[j] = tmp;
  }
}  */ 
#define MAXIMUM_KNOWN_PAGE_SIZE 4096
#define HEADER_PAGE_MASK (header->page_size - 1)
#define KERNEL_SIZE header->kernel_size
#define HEADER_PAGE_SIZE header->page_size
#define KERNEL_PADDING (header->page_size - (header->kernel_size & HEADER_PAGE_MASK))
static unsigned char padding[MAXIMUM_KNOWN_PAGE_SIZE] = { 0, };
boot_img_hdr* load_boot_image_header(FILE *fp){
	
	//int fd = open(option_values.image_filename,O_RDWR);
	if(!fp){
		fprintf(stderr,"error opening file %s errno:%d %s\n",option_values.image_filename,errno,strerror(errno));
		exit(1);
	}
	fseek(fp,0,SEEK_END);
	unsigned long  size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	long int position = ftell(fp);
	if(size<sizeof(boot_img_hdr)){
		fprintf(stderr,"unexpected filesize %ld\n",(long int)size);
		fclose(fp);
		exit(0);
	}
		
	// allocate a known max page
	byte_p header_buffer=malloc(MAXIMUM_KNOWN_PAGE_SIZE);
	size_t bytes_read = fread(header_buffer,1,MAXIMUM_KNOWN_PAGE_SIZE,fp);
	//fprintf(stderr,"bytes_read %ld\n",(long int)bytes_read);
	byte_p offset=find_in_memory(header_buffer,MAXIMUM_KNOWN_PAGE_SIZE,BOOT_MAGIC,BOOT_MAGIC_SIZE);
	if(!offset) {
		// offset not found in the first double page read the full file to check 
		header_buffer=realloc(header_buffer,size);
		fseek(fp,0,SEEK_SET);
		if((bytes_read = fread(header_buffer,1,size,fp)) != size){
			fprintf(stderr,"bytes_read  %ld size %ld\n",(long int)bytes_read,size);
			fclose(fp);
			exit(0);
		}
		fprintf(stderr,"bytes_read  %ld size %ld\n",(long int)bytes_read,size);
		if ((offset=find_in_memory(header_buffer,size,BOOT_MAGIC,BOOT_MAGIC_SIZE)) == NULL){
			fprintf(stderr,"cannot find android boot image magic\n");
			fclose(fp);
			exit(0);
		}
	}
	fseek(fp,offset-header_buffer,SEEK_SET);
	return (boot_img_hdr*)offset;
}
int get_content_hash(boot_img_hdr* header,unsigned char *kernel_data,unsigned char *ramdisk_data,unsigned char *second_data)
{
	SHA_CTX ctx;
    SHA_init(&ctx);
    SHA_update(&ctx, kernel_data, header->kernel_size);
    SHA_update(&ctx, &header->kernel_size, sizeof(header->kernel_size));
    SHA_update(&ctx, ramdisk_data, header->ramdisk_size);
    SHA_update(&ctx, &header->ramdisk_size, sizeof(header->ramdisk_size));
    SHA_update(&ctx, second_data, header->second_size);
    SHA_update(&ctx, &header->second_size, sizeof(header->second_size));
    const uint8_t* sha = SHA_final(&ctx);
	memcpy(&header->id, sha, SHA_DIGEST_SIZE > sizeof(header->id) ? sizeof(header->id) : SHA_DIGEST_SIZE);
    return 0;

}
static int print_boot_image_info(boot_image_t boot_image,FILE* fp)
{	
	
	fprintf(fp,		"name:%s"EOL
					"cmdline:%s"EOL
					"magic_offset:     [0x%08x][%u]"EOL
					"page_size:        [0x%08x][%u]"EOL
					"kernel_addr:      [0x%08x][%u]"EOL
					"kernel_size:      [0x%08x][%u]"EOL
					"kernel_pagecount: [0x%08x][%u]"EOL
					"kernel_offset:    [0x%08x][%u]"EOL
					"ramdisk_addr:     [0x%08x][%u]"EOL
					"ramdisk_size:     [0x%08x][%u]"EOL
					"ramdisk_pagecount:[0x%08x][%u]"EOL
					"ramdisk_offset:   [0x%08x][%u]"EOL
					"second_addr:      [0x%08x][%u]"EOL
					"second_size:      [0x%08x][%u]"EOL
					"second_pagecount: [0x%08x][%u]"EOL
					"second_offset:    [0x%08x][%u]"EOL
					"tags_addr:        [0x%08x][%u]"EOL,
		    boot_image.header.name,boot_image.header.cmdline,
		    (unsigned)boot_image.magic_offset,(unsigned)boot_image.magic_offset,
		    boot_image.header.page_size,boot_image.header.page_size,
			boot_image.header.kernel_addr,boot_image.header.kernel_addr,
			boot_image.header.kernel_size,boot_image.header.kernel_size,
			boot_image.kernel_page_count,boot_image.kernel_page_count,
			(unsigned)boot_image.kernel_offset,(unsigned)boot_image.kernel_offset,
			boot_image.header.ramdisk_addr,boot_image.header.ramdisk_addr,
			boot_image.header.ramdisk_size,boot_image.header.ramdisk_size,		
			boot_image.ramdisk_page_count,boot_image.ramdisk_page_count,
			(unsigned)boot_image.ramdisk_offset,(unsigned)boot_image.ramdisk_offset,
			boot_image.header.second_addr,boot_image.header.second_addr,
			boot_image.header.second_size,boot_image.header.second_size,
			boot_image.second_page_count,boot_image.second_page_count,
			(unsigned)boot_image.second_offset,(unsigned)boot_image.second_offset,
			boot_image.header.tags_addr,boot_image.header.tags_addr);
	return 0;

}
// give the parse_boot_image_info
static boot_image_t parse_boot_image_info(byte_p data, size_t file_size)
{
	
	
	if( sizeof(boot_img_hdr)<file_size ) {	
		log_write("parse_boot_image_info:error_image_too_small\n");
		//free(data);
		//exit(0);
	}
	byte_p magic_position_p = find_in_memory(data,file_size,BOOT_MAGIC,BOOT_MAGIC_SIZE);
	log_write("parse_boot_image_info:error_image_too_small\n");
	//long offset_data = magic_offset_p-data;
	if(!magic_position_p)
	{
		//log_write("parse_boot_image_info:error_no_image_magic\n" );
		fprintf(stderr,"%s does not contain a valid android boot image magic\n"	,option_values.image_filename);	
		free(data);
		exit(0);
	}	
	boot_image_t *boot_image_p=(boot_image_t *)magic_position_p;
	boot_image_t boot_image=(*boot_image_p);
	//log_write("parse_boot_image_info:boot_image=[%08x] : boot_image.header [%08x]\n", boot_image_p,boot_image_header_p);
    boot_image.magic_offset=magic_position_p-data;
	boot_image.boot_image_filesize = file_size;

	boot_image.kernel_page_count = (boot_image.header.kernel_size + boot_image.header.page_size - 1) / boot_image.header.page_size;
	boot_image.ramdisk_page_count = (boot_image.header.ramdisk_size + boot_image.header.page_size - 1) / boot_image.header.page_size;
	boot_image.second_page_count =  (boot_image.header.second_size + boot_image.header.page_size - 1) / boot_image.header.page_size;

	boot_image.kernel_offset = boot_image.header.page_size;
	boot_image.ramdisk_offset = boot_image.kernel_offset + (boot_image.kernel_page_count * boot_image.header.page_size);	
	boot_image.second_offset = boot_image.ramdisk_offset + (boot_image.ramdisk_page_count *  boot_image.header.page_size);
	boot_image.header_data_start=magic_position_p;
	boot_image.kernel_data_start=magic_position_p+boot_image.kernel_offset;
	boot_image.ramdisk_data_start=magic_position_p+boot_image.ramdisk_offset;
	boot_image.second_data_start=magic_position_p+boot_image.second_offset;
	
	return boot_image;

}
static int write_boot_image(FILE* fp,boot_img_hdr* header_p,unsigned char *kernel_data,unsigned char *ramdisk_data,unsigned char *second_data)
{

	boot_img_hdr header= (*header_p);
	unsigned pagemask = header.page_size - 1; 
	int header_size = sizeof( header);
	get_content_hash(&header,kernel_data,ramdisk_data,second_data);
	int ramdisk_padding = header.page_size - (header.ramdisk_size & pagemask);
	int header_padding = header.page_size - (header_size & pagemask);
	int kernel_padding = header.page_size - (header.kernel_size & pagemask);
	int second_padding =  header.page_size - (header.second_size & pagemask);
	int fd = fileno(fp); 
	if(write(fd, &header, header_size) != header_size) goto fail;
    if(write(fd, padding, header_padding) != header_padding) goto fail;

    if(write(fd, kernel_data,  header.kernel_size) != (int) header.kernel_size) goto fail;
	if(write(fd, padding, kernel_padding) != kernel_padding ) goto fail;

    if(write(fd, ramdisk_data, header.ramdisk_size) !=(int) header.ramdisk_size) goto fail;
    if(write(fd, padding, ramdisk_padding ) != ramdisk_padding ) goto fail;
    if(header.second_size>0) {
        if(write(fd, second_data,header. second_size) !=(int) header.second_size) goto fail;
        if(write(fd, padding, second_padding ) != second_padding ) goto fail;
    }
fail:
	return 0; 
}
// Extract Kernel File
// boot_image_file : Boot Image File. File Position must be at the start of the kernel
// Returns : Sets boot_image_file file position to ramdisk entry
static int unpack_kernel_file(FILE* boot_image_file,boot_img_hdr* header){
	unsigned pagemask = header->page_size - 1;
	if(option_values.kernel_filename){
		fprintf(stderr," Extracting Kernel %s\n",option_values.kernel_filename);
		byte_p buffer=malloc(header->kernel_size);
		fread(buffer,1,header->kernel_size,boot_image_file);
		write_to_file(buffer,header->kernel_size,option_values.kernel_filename);
		free(buffer);
	}else{
		fseek(boot_image_file,header->kernel_size,SEEK_CUR); 
	}
	fseek(boot_image_file,(header->page_size - (header->kernel_size & pagemask)),SEEK_CUR);
	return 0;
} 
static int unpack_ramdisk(FILE* boot_image_file,boot_img_hdr* header){
	byte_p buffer=NULL;
	unsigned pagemask = header->page_size - 1;
	long ramdisk_padding = header->page_size - (header->ramdisk_size & pagemask);
	if(option_values.ramdisk_archive_filename || option_values.ramdisk_directory_name || option_values.ramdisk_cpio_filename){
		fprintf(stderr," Extracting Ramdisk\n");
	
		buffer=malloc(header->ramdisk_size);
		fread(buffer,1,header->ramdisk_size,boot_image_file);
		fseek(boot_image_file,ramdisk_padding,SEEK_CUR); 
	}else{
		fseek(boot_image_file,header->ramdisk_size+ramdisk_padding,SEEK_CUR); 
		return 0;
	}
	if(option_values.ramdisk_archive_filename)	
		write_to_file(buffer,header->ramdisk_size,option_values.ramdisk_archive_filename);
	
	if(	option_values.ramdisk_cpio_filename || option_values.ramdisk_directory_name ){
		
		byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
		unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(buffer,header->ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
		
		if( option_values.ramdisk_cpio_filename )
			write_to_file(uncompressed_ramdisk_data,uncompressed_ramdisk_size,option_values.ramdisk_cpio_filename);
		
		if( option_values.ramdisk_directory_name )				
			process_uncompressed_ramdisk(uncompressed_ramdisk_data,uncompressed_ramdisk_size,option_values.ramdisk_directory_name);
		
	}
	if(option_values.ramdisk_archive_filename || option_values.ramdisk_directory_name || option_values.ramdisk_cpio_filename)
		free(buffer);
	return 0 ;	
		
}
static int unpack_second_file(FILE* boot_image_file,boot_img_hdr* header){
	unsigned pagemask = header->page_size - 1;
	if((header->second_size > 0) && (option_values.second_filename)){
		fprintf(stderr," Extracting Second %s\n",option_values.second_filename);
		byte_p buffer=malloc(header->second_size);
		fread(buffer,1,header->second_size,boot_image_file);
		write_to_file(buffer,header->second_size,option_values.second_filename);
		free(buffer);
	}else{
		fseek(boot_image_file,header->second_size,SEEK_CUR); 
	}
	fseek(boot_image_file,(header->page_size - (header->second_size & pagemask)),SEEK_CUR);
	return 0;
} 
int unpack_boot_image_file()
{
	FILE* boot_image_file = fopen(option_values.image_filename,"r+b");
	boot_img_hdr* header = load_boot_image_header(boot_image_file);
	long  offset = ftell(boot_image_file);
	long data_size = 0,position =0;
	fprintf(stderr," Android Boot Image Found @ %ld\n",offset);
	fseek(boot_image_file , header->page_size, SEEK_CUR);
	unpack_kernel_file(boot_image_file,header);
	fprintf(stderr," Position  %ld\n",ftell(boot_image_file));
	unpack_ramdisk(boot_image_file,header);
	fprintf(stderr," Position  %ld\n",ftell(boot_image_file));
	unpack_second_file(boot_image_file,header);	
	fclose(boot_image_file);
	exit(0);	
}

int pack_boot_image_file(){
	
	
	char starting_directory[PATH_MAX];
	getcwd(starting_directory,PATH_MAX);
	log_write("create_boot_image_file:kernel:%s:ramdisk_directory:%s image:%s starting_directory=%s\n",option_values.kernel_filename,option_values.ramdisk_directory_name,option_values.image_filename,starting_directory);
	byte_p ramdisk_gzip_data = NULL, kernel_data = NULL ,second_data = NULL,cmdline_data = NULL;
	size_t ramdisk_gzip_size = 0, kernel_size=0, second_size = 0;
	unsigned pagemask = option_values.page_size - 1; 
	if ( option_values.ramdisk_directory_name || option_values.ramdisk_cpio_filename  ){
		
		log_write("create_boot_image_file:packing_ramdisk_directory\n");
		size_t cpio_size =0 ; 
		unsigned char* ramdisk_cpio_data =NULL;
		
		if (option_values.ramdisk_directory_name) {
			ramdisk_cpio_data = calloc(MEMORY_BUFFER_SIZE, sizeof(unsigned char)) ;
			cpio_size = pack_ramdisk_directory(ramdisk_cpio_data); 
		
		}else if ( option_values.ramdisk_cpio_filename  ){
			ramdisk_cpio_data = load_file( option_values.ramdisk_cpio_filename ,&cpio_size);
		ramdisk_gzip_data = calloc(cpio_size, sizeof(unsigned char));
		ramdisk_gzip_size = compress_gzip_ramdisk_memory(ramdisk_cpio_data,cpio_size,ramdisk_gzip_data,cpio_size);
		log_write("create_boot_image_file:ramdisk_gzip_size=%lu\n",ramdisk_gzip_size);
		free(ramdisk_cpio_data);
		chdir(starting_directory);
		}
	}
	if ( option_values.ramdisk_archive_filename )	ramdisk_gzip_data = load_file( option_values.ramdisk_name,&ramdisk_gzip_size );

	if ( option_values.kernel_filename ) kernel_data=load_file(option_values.kernel_filename,&kernel_size);

	if ( option_values.second_filename ) second_data=load_file(option_values.second_filename,&second_size);

	
	
	boot_img_hdr boot_image_header = { BOOT_MAGIC,kernel_size, 
			DEFAULT_KERNEL_ADDRESS,ramdisk_gzip_size,
			DEFAULT_RAMDISK_ADDRESS,second_size,DEFAULT_SECOND_ADDRESS,
			DEFAULT_TAGS_ADDRESS,option_values.page_size, .unused={'\0'}	, .id={'\0'}		
			};
	 char* pboot =(char*) &boot_image_header.name[0];
	 char* cmdline =(char*) &boot_image_header.cmdline[0];
	 pboot=option_values.board_name; cmdline=option_values.cmdline_text;
	 
	struct dirent *entry;	
	FILE*fp = fopen(option_values.image_filename,"wb");
	write_boot_image(fp,&boot_image_header,kernel_data,ramdisk_gzip_data,second_data);
    fclose(fp);
    if(second_size) free(second_data);
    free(kernel_data);
	free(ramdisk_gzip_data);
	return 0;
}

int extract_boot_image_file(){
	
	FILE* boot_image_file = fopen(option_values.image_filename,"r+b");
	boot_img_hdr* header = load_boot_image_header(boot_image_file);
	fseek(boot_image_file , HEADER_PAGE_SIZE + KERNEL_SIZE + KERNEL_PADDING,SEEK_CUR);
	
	// assign some memory to read the file	
	byte_p ramdisk_data =  (byte_p) malloc(header->ramdisk_size) ;
	fread(ramdisk_data,1,header->ramdisk_size,boot_image_file);
	fclose(boot_image_file);
	
	byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
	unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(ramdisk_data,header->ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
	
	
	//fprintf(stderr,"second_filename=%s  strlen=%d ,count=%d\n",option_values.source_filename,strlen(option_values.source_filename),option_values.source_length);
	free(ramdisk_data);
	
	find_file_in_ramdisk_entries(uncompressed_ramdisk_data);
	free(uncompressed_ramdisk_data);
	
	
	return 0;
}

int list_boot_image_info(){
	
	FILE* fp = fopen(option_values.image_filename,"r+b");
	boot_img_hdr* header = load_boot_image_header(fp); 
	long int position = ftell(fp);
	fprintf(stderr," Android Boot Image Found @ %ld\n",position);
	fprintf(stderr," File Header   :\n");
	fprintf(stderr," Kernel Size   : 0x%08x %u\n",header->kernel_size,header->kernel_size);
	fprintf(stderr," Kernel Addr   : 0x%08x %u\n",header->kernel_addr,header->kernel_addr);
	fprintf(stderr," Ramdisk Size  : 0x%08x %u\n",header->ramdisk_size,header->ramdisk_size);
	fprintf(stderr," Ramdisk Addr  : 0x%08x %u\n",header->ramdisk_addr,header->ramdisk_addr);
	fprintf(stderr," Second Size   : 0x%08x %u\n",header->second_size,header->second_size);
	fprintf(stderr," Second Addr   : 0x%08x %u\n",header->second_addr,header->second_addr);
	fprintf(stderr," Tags Addr     : 0x%08x %u\n",header->tags_addr,header->tags_addr);
	fprintf(stderr," Page Size     : 0x%08x %u\n",header->page_size,header->page_size);
	fprintf(stderr," name          : %s\n",header->name);
	fprintf(stderr," cmdline       : %s\n",header->cmdline);
	fclose(fp);
	return 0;
}
int update_boot_image_file_direct(){

	FILE* fp = fopen(option_values.image_filename,"r+b");
	boot_img_hdr *header=load_boot_image_header(fp);
	long int position = ftell(fp);
	fprintf(stderr," Android Boot Image Found @ %ld\n",position);
	ssize_t bytes_written=0;
	if(option_values.board_name){
		fseek(fp,header->name-(unsigned char*)header,SEEK_SET);
		bytes_written= fwrite(option_values.board_name,1,BOOT_NAME_SIZE,fp);	
	}
	if (option_values.cmdline_text){
		fseek(fp,header->cmdline-(unsigned char*)header,SEEK_SET);
		bytes_written= fwrite(option_values.cmdline_text,1,BOOT_ARGS_SIZE,fp);	
	}
	fsync(fileno(fp));
	fprintf(stderr," bytes_written %ld\n",(long int)bytes_written);
	fclose(fp); 
	return 0;	
}

int update_boot_image_file(){
			
	
	FILE* boot_image_file = fopen(option_values.image_filename,"r+b");
	boot_img_hdr* header = load_boot_image_header(boot_image_file);
	unsigned pagemask = header->page_size - 1;
	fseek(boot_image_file , header->page_size, SEEK_CUR);
	byte_p ramdisk_gzip_data=NULL , kernel_data=NULL ,second_data=NULL;
	
	if(option_values.kernel_filename){
		// updating the kernel
		fprintf(stderr,"Updating boot image %s kernel with %s\n",option_values.image_filename,option_values.kernel_filename);	 
		size_t kernel_size=0;
		kernel_data = load_file(option_values.kernel_filename,	&kernel_size);
		header->kernel_size=kernel_size;
		fseek(boot_image_file,header->kernel_size,SEEK_CUR); 
	}else{ //not updating the kernel . read the original
		kernel_data=malloc(header->kernel_size);
		fread(kernel_data,1,header->kernel_size,boot_image_file);
		
	}
	fseek(boot_image_file,(header->page_size - (header->kernel_size & pagemask)),SEEK_CUR);
	
	if(option_values.source_filename){ //updating ramdisk 
		fprintf(stderr,"Updating ramdisk in boot image %s\n",option_values.image_filename);
		byte_p ramdisk_data =malloc(header->ramdisk_size);
		fread(ramdisk_data,1,header->ramdisk_size,boot_image_file);
		byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
		size_t uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(ramdisk_data,header->ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
		//log_write("modify_size:%ld\n",uncompressed_ramdisk_size);
		size_t new_cpio_file_size=0;
		byte_p new_cpio_data = modify_ramdisk_entry(uncompressed_ramdisk_data,uncompressed_ramdisk_size,&new_cpio_file_size);
		free(uncompressed_ramdisk_data);
		free(ramdisk_data);
		// nothing done
		if(new_cpio_data!=uncompressed_ramdisk_data){
			ramdisk_gzip_data = calloc(new_cpio_file_size, sizeof(unsigned char));
			size_t ramdisk_gzip_size = compress_gzip_ramdisk_memory(new_cpio_data,new_cpio_file_size,ramdisk_gzip_data,new_cpio_file_size);
			free(new_cpio_data);	
			header->ramdisk_size = ramdisk_gzip_size;
		}
	}else {
		ramdisk_gzip_data=malloc(header->ramdisk_size);
		fread(ramdisk_gzip_data,1,header->ramdisk_size,boot_image_file);
	}
	fseek(boot_image_file,(header->page_size - (header->ramdisk_size & pagemask)),SEEK_CUR);
	second_data=malloc(header->second_size);
	fread(second_data,1,header->second_size,boot_image_file);
	fseek(boot_image_file,(header->page_size - (header->second_size & pagemask)),SEEK_CUR);
	fclose(boot_image_file);
	
	FILE*fp = fopen(option_values.image_filename,"wb");
	write_boot_image(fp,header,kernel_data,ramdisk_gzip_data,second_data);
	fclose(fp);
	
	free(kernel_data);
	free(ramdisk_gzip_data);
	free(second_data);
	
	return 0;
	exit(0);
}
