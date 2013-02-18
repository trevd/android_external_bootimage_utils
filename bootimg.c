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

static unsigned char padding[4096] = { 0, };
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
					"magic_offset:[%u][0x%08x]"EOL
					"page_size:[%u][0x%08x]"EOL
					"kernel_addr:[%u][0x%08x]"EOL
					"kernel_size:[%u][0x%08x]"EOL
					"kernel_pagecount:[%u][0x%08x]"EOL
					"kernel_offset:[%u][0x%08x]"EOL
					"ramdisk_addr:[%u][0x%08x]"EOL
					"ramdisk_size:[%u][0x%08x]"EOL
					"ramdisk_pagecount:[%u][0x%08x]"EOL
					"ramdisk_offset:[%u][0x%08x]"EOL
					"second_addr:[%x][0x%08x]"EOL
					"second_size:[%u][0x%08x]"EOL
					"second_pagecount:[%u][0x%08x]"EOL
					"second_offset:[%u][0x%08x]"EOL
					"tags_addr:[%u][0x%08x]"EOL,
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
	
	if(file_size < sizeof(boot_img_hdr)) {	
		log_write("parse_boot_image_info:error_image_too_small\n");
		free(data);
		exit(0);
	}
	byte_p magic_position_p = find_in_memory(data,file_size,BOOT_MAGIC,BOOT_MAGIC_SIZE);
	//
	//long offset_data = magic_offset_p-data;
	if(!magic_position_p)
	{
		//log_write("parse_boot_image_info:error_no_image_magic\n" );
		fprintf(stderr,"%s does not contain a valid android boot image magic\n"	,option_values.image_filename);	
		free(data);
		exit(0);
	}	
	//boot_img_hdr * boot_image_header_p=(boot_img_hdr *)offset_data;
	boot_image_t *boot_image_p=(boot_image_t *)magic_position_p;
	boot_image_t boot_image=(*boot_image_p);
	//boot_image_header_p = &boot_image.header;
	
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
static int write_boot_image(FILE* fp,boot_img_hdr header,unsigned char *kernel_data,unsigned char *ramdisk_data,unsigned char *second_data)
{
	unsigned pagemask =header.page_size - 1; 
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
int process_header_switch(boot_image_t boot_image){
	
	if ( ( ACTION_UNPACK || ACTION_EXTRACT )  && (option_values.header_filename)){
			//log_write("unpack_boot_image_file:extracting_header:%08x\n",option_values.header_filename);
			FILE* fp =fopen(option_values.header_filename,"wb");
			print_boot_image_info(boot_image,fp);
			fclose(fp);
	}
	return 0;
}
void process_cmdline_switch(boot_image_t boot_image){
	if (!option_values.cmdline_filename) return ;
	if( ( ACTION_UNPACK || ACTION_EXTRACT )  && (option_values.cmdline_filename)  ){
		//log_write("extract command line:%s\n",option_values.cmdline_filename);
		write_single_line_to_file((const char*)option_values.cmdline_filename,(const char*)boot_image.header.cmdline,strlen((const char*)boot_image.header.cmdline));
	}if(strlen(option_values.cmdline_filename)==0){
			option_values.cmdline_filename=DEFAULT_CMDLINE_NAME;
	}
}

int unpack_boot_image_file()
{
	size_t	file_size =0 ;
	byte_p raw_boot_image_data = load_file(option_values.image_filename,&file_size);
	boot_image_t boot_image=parse_boot_image_info(raw_boot_image_data,file_size);
	//log_write("unpack_boot_image_file:page_size=[%d]kernel_size=[%x]\n",boot_image.header.page_size,boot_image.header.kernel_size);
	int mkdir_result = mkdir(option_values.output_directory_name,0777);
	//log_write("unpack_boot_image_file:page_size=[%d]kernel_size=[%x]\n",boot_image.header.page_size,boot_image.header.kernel_size);
	chdir(option_values.output_directory_name);
	fprintf(stderr,"Unpacking boot image %s to %s\n",option_values.image_filename,option_values.output_directory_name);	 
	process_header_switch(boot_image);
	process_cmdline_switch(boot_image);
	
	
	if( option_values.ramdisk_archive_filename  ){
			write_to_file(boot_image.ramdisk_data_start,boot_image.header.ramdisk_size,option_values.ramdisk_archive_filename);
			//fprintf(stderr,"extracting gzip ramdisk:%s\n",option_values.ramdisk_archive_filename);

	}
	if(	option_values.ramdisk_cpio_filename || option_values.ramdisk_directory_name ){
		
		byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
		unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(boot_image.ramdisk_data_start,boot_image.header.ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
		
		if( option_values.ramdisk_cpio_filename ){
			write_to_file(uncompressed_ramdisk_data,uncompressed_ramdisk_size,option_values.ramdisk_cpio_filename);
			//fprintf(stderr,"extracting cpio ramdisk:%s\n",option_values.ramdisk_cpio_filename);
		}
		if( option_values.ramdisk_directory_name )
			//	fprintf(stderr,"extracting ramdisk:%s\n",option_values.ramdisk_directory_name);
						
		process_uncompressed_ramdisk(uncompressed_ramdisk_data,uncompressed_ramdisk_size,option_values.ramdisk_directory_name);
			
		free(uncompressed_ramdisk_data);
		
	}
	if	( option_values.kernel_filename ){
		write_to_file(boot_image.kernel_data_start,boot_image.header.kernel_size,option_values.kernel_filename);
		
	}
	if	( option_values.second_filename ){
		write_to_file(boot_image.second_data_start,boot_image.header.second_size,option_values.second_filename);
	//	fprintf(stderr,"extracting second:%s\n",option_values.second_filename);
	}
	//chdir(starting_directory);	
		
quit_now:
	//free(option_values.output);
	free(raw_boot_image_data);
	return 0;
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
		}else if ( option_values.ramdisk_cpio_filename  )
			ramdisk_cpio_data = load_file( option_values.ramdisk_cpio_filename ,&cpio_size);
		ramdisk_gzip_data = calloc(cpio_size, sizeof(unsigned char));
		ramdisk_gzip_size = compress_gzip_ramdisk_memory(ramdisk_cpio_data,cpio_size,ramdisk_gzip_data,cpio_size);
		log_write("create_boot_image_file:ramdisk_gzip_size=%lu\n",ramdisk_gzip_size);
		free(ramdisk_cpio_data);
		chdir(starting_directory);
		
	}
	if ( option_values.ramdisk_archive_filename )	ramdisk_gzip_data = load_file( option_values.ramdisk_name,&ramdisk_gzip_size );

	if ( option_values.kernel_filename ) kernel_data=load_file(option_values.kernel_filename,&kernel_size);

	if ( option_values.second_filename ) second_data=load_file(option_values.second_filename,&second_size);

	
	
	boot_img_hdr boot_image_header = { BOOT_MAGIC,kernel_size, 
			DEFAULT_KERNEL_ADDRESS,ramdisk_gzip_size,
			DEFAULT_RAMDISK_ADDRESS,second_size,DEFAULT_SECOND_ADDRESS,
			DEFAULT_TAGS_ADDRESS,option_values.page_size,
			.name = {'\0'},.unused={'\0'},.cmdline={'\0'},.id={'\0'}
			};
	if ( option_values.cmdline_filename ){ 
		  read_file_to_size(option_values.cmdline_filename,BOOT_ARGS_SIZE,boot_image_header.cmdline);
	}	 
		 
	if ( option_values.board_filename ) {
		read_file_to_size(option_values.board_filename,BOOT_ARGS_SIZE,boot_image_header.name);
	}
	struct dirent *entry;	
	FILE*fp = fopen(option_values.image_filename,"wb");
	write_boot_image(fp,boot_image_header,kernel_data,ramdisk_gzip_data,second_data);
    fclose(fp);
    if(second_size) free(second_data);
    free(kernel_data);
	free(ramdisk_gzip_data);
	return 0;
}
int list_boot_image_info()
{
	size_t file_size =0 ;
	byte_p raw_boot_image_data = load_file(option_values.image_filename,&file_size);
	boot_image_t boot_image=parse_boot_image_info(raw_boot_image_data,file_size);
	print_boot_image_info(boot_image,stderr);
	
quit_now:
	free(raw_boot_image_data);
	return 0;
}
int extract_boot_image_file(){
	
	size_t file_size =0 ;
	byte_p raw_boot_image_data = load_file(option_values.image_filename,&file_size);
	boot_image_t boot_image=parse_boot_image_info(raw_boot_image_data,file_size);
	byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
	unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(boot_image.ramdisk_data_start,boot_image.header.ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
	find_file_in_ramdisk_entries(uncompressed_ramdisk_data);
	free(uncompressed_ramdisk_data);
	free(raw_boot_image_data);
	return 0;
}

int update_boot_image_file_direct(){
	//int fd = open(option_values.image_filename,O_RDWR);
	FILE* fp = fopen(option_values.image_filename,"r+b");
	int fd = fileno(fp);
	if(!fd){
		fprintf(stderr,"error opening file %s errno:%d %s\n",option_values.image_filename,errno,strerror(errno));
		exit(1);
	}
	long size=lseek(fd,0,SEEK_END);
	if(size<sizeof(boot_img_hdr)){
		fprintf(stderr,"unexpected filesize %ld\n",(long int)size);
		close(fd);
		exit(0);
	}
	fprintf(stderr,"size %ld\n",size);
	off_t position =lseek(fd,0,SEEK_SET);
	fprintf(stderr,"position %ld\n",(long int)position);
	
	struct boot_img_hdr* header=NULL;
	// allocate a double page
	byte_p magic=malloc(8096*sizeof(byte));
	ssize_t bytes_read = read(fd,magic,8096);
	fprintf(stderr,"bytes_read %ld\n",(long int)bytes_read);
	byte_p offset=find_in_memory(magic,8096,BOOT_MAGIC,BOOT_MAGIC_SIZE);
	if(!offset) {
		// offset not found in the first double page read the full file to check 
		magic=realloc(magic,size);
		position =lseek(fd,0,SEEK_SET);
		if((bytes_read = read(fd,magic,size)) != size){
			fprintf(stderr,"bytes_read  %ld size %ld\n",(long int)bytes_read,size);
			exit(0);
		}
		fprintf(stderr,"bytes_read  %ld size %ld\n",(long int)bytes_read,size);
		if ((offset=find_in_memory(magic,size,BOOT_MAGIC,BOOT_MAGIC_SIZE)) == NULL){
			fprintf(stderr,"cannot find android boot image magic\n");
			close(fd);
			exit(0);
		}
	}
	ssize_t bytes_written=0;
	header=(struct boot_img_hdr*)offset;
	if(option_values.board_name){
		position= lseek(fd,header->name-magic,SEEK_SET);
		bytes_written= write(fd,option_values.board_name,BOOT_NAME_SIZE);	
	}
	if (option_values.cmdline_text){
		position= lseek(fd,header->cmdline-magic,SEEK_SET);
		bytes_written= write(fd,option_values.cmdline_text,BOOT_ARGS_SIZE);	
	}
	fsync(fd);
	fprintf(stderr,"position %ld bytes_written %ld\n",(long int)position,(long int)bytes_written);
	
	close(fd);
	free(magic);
	exit(0);
	
	
}

int update_boot_image_file(){
			
	
	size_t file_size =0, new_cpio_file_size=0;
	byte_p raw_boot_image_data = load_file(option_values.image_filename,&file_size);
	boot_image_t boot_image = parse_boot_image_info(raw_boot_image_data,file_size);
	
	byte_p ramdisk_gzip_data=NULL;byte_p kernel_data=NULL;
	
	if(option_values.kernel_filename){
		fprintf(stderr,"Updating boot image %s kernel with %s\n",option_values.image_filename,option_values.kernel_filename);	 
		 size_t kernel_size=0;
		 kernel_data = load_file(option_values.kernel_filename,	&kernel_size);
		 ramdisk_gzip_data=boot_image.ramdisk_data_start;
		 boot_image.header.kernel_size=kernel_size;
	}
	if(option_values.source_filename){
		fprintf(stderr,"Updating ramdisk in boot image %s\n",option_values.image_filename);
		byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
		size_t uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(boot_image.ramdisk_data_start,boot_image.header.ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
		//log_write("modify_size:%ld\n",uncompressed_ramdisk_size);
		byte_p new_cpio_data = modify_ramdisk_entry(uncompressed_ramdisk_data,uncompressed_ramdisk_size,&new_cpio_file_size);
		free(uncompressed_ramdisk_data);
		// nothing done
		if(new_cpio_data==uncompressed_ramdisk_data) goto quit_now;
		ramdisk_gzip_data = calloc(new_cpio_file_size, sizeof(unsigned char));
		size_t ramdisk_gzip_size = compress_gzip_ramdisk_memory(new_cpio_data,new_cpio_file_size,ramdisk_gzip_data,new_cpio_file_size);
		free(new_cpio_data);	
		boot_image.header.ramdisk_size = ramdisk_gzip_size;
		kernel_data=boot_image.kernel_data_start;
	}
	
	FILE*fp = fopen(option_values.image_filename,"wb");
	write_boot_image(fp,boot_image.header,kernel_data,ramdisk_gzip_data,boot_image.second_data_start);
	fclose(fp);
	
	free(raw_boot_image_data);
	
	
	if(option_values.kernel_filename)
		free(kernel_data);
	else
		free(ramdisk_gzip_data);
	return 0;
quit_now:
	free(raw_boot_image_data);
	exit(0);
	
}
