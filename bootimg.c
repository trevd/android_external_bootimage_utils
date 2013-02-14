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

#include "mincrypt/sha.h"

#include "bootimg.h"
#include "bootimg_utils.h"
#include "program.h"

#include "file.h"
#include "ramdisk.h"


/*void strrev(char *s) {
  int tmp, i, j;
  for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
    tmp = s[i];
    s[i] = s[j];
    s[j] = tmp;
  }
}  */ 

static unsigned long find_magic_offset(byte_p data , unsigned file_size){
	long offset=-1;
	long* poffset=&offset ;
	poffset= memmem1((const unsigned char*)data,file_size,BOOT_MAGIC,BOOT_MAGIC_SIZE);
	if(poffset==0) return -1;
	return  0-( (unsigned long)&data[0]-(unsigned long)&poffset[0]);
	
}
static int print_boot_image_info(boot_image_t boot_image,FILE* fp)
{	
	
	fprintf(fp,		"name:%s"EOL
					"cmdline:%s"EOL
					"magic_offset:[%u][0x%08x]"EOL
					"page_size:[%d][0x%08x]"EOL
					"kernel_addr:[%u][0x%08x]"EOL
					"kernel_size:[%u][0x%08x]"EOL
					"kernel_pagecount:[%u][0x%08x]"EOL
					"kernel_offset:[%u][0x%08x]"EOL
					"ramdisk_addr:[%u][0x%08x]"EOL
					"ramdisk_size:[%u][0x%08x]"EOL
					"ramdisk_pagecount:[%d][0x%08x]"EOL
					"ramdisk_offset:[%d][0x%08x]"EOL
					"second_addr:[%x][0x%08x]"EOL
					"second_size:[%d][0x%08x]"EOL
					"second_pagecount:[%d][0x%08x]"EOL
					"second_offset:[%d][0x%08x]"EOL
					"tags_addr:[%d][0x%08x]"EOL,
		    boot_image.header.name,boot_image.header.cmdline,
		    boot_image.magic_offset,boot_image.magic_offset,
		    boot_image.header.page_size,boot_image.header.page_size,
			boot_image.header.kernel_addr,boot_image.header.kernel_addr,
			boot_image.header.kernel_size,boot_image.header.kernel_size,
			boot_image.kernel_page_count,boot_image.kernel_page_count,
			boot_image.kernel_offset,boot_image.kernel_offset,
			boot_image.header.ramdisk_addr,boot_image.header.ramdisk_addr,
			boot_image.header.ramdisk_size,boot_image.header.ramdisk_size,		
			boot_image.ramdisk_page_count,boot_image.ramdisk_page_count,
			boot_image.ramdisk_offset,boot_image.ramdisk_offset,
			boot_image.header.second_addr,boot_image.header.second_addr,
			boot_image.header.second_size,boot_image.header.second_size,
			boot_image.second_page_count,boot_image.second_page_count,
			boot_image.second_offset,boot_image.second_offset,
			boot_image.header.tags_addr,boot_image.header.tags_addr);
	return 0;

}
// give the parse_boot_image_info
static boot_image_t parse_boot_image_info(byte_p data, unsigned file_size)
{
	
	if(file_size < sizeof(boot_img_hdr)) {	
		log_write("parse_boot_image_info:error_image_too_small\n");
		free(data);
		exit(0);
	}
	long magic_offset = find_magic_offset(data,file_size);
	if(magic_offset==-1)
	{
		log_write("parse_boot_image_info:error_no_image_magic\n" );
		free(data);
		exit(0);
	}
	log_write("parse_boot_image_info:image_magic_offset=[%ld][0x%lx]\n", magic_offset,magic_offset);
	log_write("parse_boot_image_info:image_header_address=[%ld][0x%lx]\n", magic_offset,magic_offset);
	boot_image_t *boot_image_p=(boot_image_t *)data+magic_offset;
	boot_image_t boot_image=(*boot_image_p);
	
    boot_image.magic_offset=magic_offset;
	boot_image.boot_image_filesize = file_size;
	boot_image.kernel_page_count = (boot_image.header.kernel_size + boot_image.header.page_size - 1) / boot_image.header.page_size;
	boot_image.ramdisk_page_count = (boot_image.header.ramdisk_size + boot_image.header.page_size - 1) / boot_image.header.page_size;
	boot_image.second_page_count =  (boot_image.header.second_size + boot_image.header.page_size - 1) / boot_image.header.page_size;

	boot_image.kernel_offset = boot_image.header.page_size;
	boot_image.ramdisk_offset = boot_image.kernel_offset + (boot_image.kernel_page_count * boot_image.header.page_size);	
	boot_image.second_offset = boot_image.ramdisk_offset + (boot_image.ramdisk_page_count *  boot_image.header.page_size);
	boot_image.header_data_start=data+magic_offset;
	boot_image.kernel_data_start=data+magic_offset+boot_image.kernel_offset;
	boot_image.ramdisk_data_start=data+magic_offset+boot_image.ramdisk_offset;
	boot_image.second_data_start=data+magic_offset+boot_image.second_offset;
	
	return boot_image;

}

int process_header_switch(boot_image_t boot_image){
	
	if ( ( ACTION_UNPACK || ACTION_EXTRACT )  && HAS_HEADER ){
			log_write("unpack_boot_image_file:extracting_header:%08x\n",option_values.header);
			FILE* fp =fopen(option_values.header,"wb");
			print_boot_image_info(boot_image,fp);
			fclose(fp);
	}
	return 0;
}
void process_cmdline_switch(boot_image_t boot_image){
	if (!option_values.cmdline) return ;
	if( ( ACTION_UNPACK || ACTION_EXTRACT )  && HAS_CMDLINE  ){
		log_write("extract command line:%s\n",option_values.cmdline);
		write_single_line_to_file((const char*)option_values.cmdline,(const char*)boot_image.header.cmdline,strlen((const char*)boot_image.header.cmdline));
	}if(strlen(option_values.cmdline)==0){
			option_values.cmdline=DEFAULT_CMDLINE_NAME;
	}
}

int unpack_boot_image_file()
{
	unsigned long 	file_size =0 ;
	byte_p raw_boot_image_data = load_file(option_values.image_filename,&file_size);
	boot_image_t boot_image=parse_boot_image_info(raw_boot_image_data,file_size);
	log_write("unpack_boot_image_file:page_size=[%d]kernel_size=[%x]\n",boot_image.header.page_size,boot_image.header.kernel_size);
	int mkdir_result = mkdir(option_values.output,0777);
	log_write("unpack_boot_image_file:page_size=[%d]kernel_size=[%x]\n",boot_image.header.page_size,boot_image.header.kernel_size);
	chdir(option_values.output);
	
	process_header_switch(boot_image);
	process_cmdline_switch(boot_image);
	
	
	if( HAS_RAMDISK_ARCHIVE  ){
			write_to_file(boot_image.ramdisk_data_start,boot_image.header.ramdisk_size,option_values.ramdisk_archive_name);
			fprintf(stderr,"extracting gzip ramdisk:%s\n",option_values.ramdisk_archive_name);

	}
	if(	HAS_RAMDISK_CPIO || HAS_RAMDISK_DIRECTORY ){
		
		byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
		unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(boot_image.ramdisk_data_start,boot_image.header.ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
		
		if( HAS_RAMDISK_CPIO ){
			write_to_file(uncompressed_ramdisk_data,uncompressed_ramdisk_size,option_values.ramdisk_cpio_name);
			fprintf(stderr,"extracting cpio ramdisk:%s\n",option_values.ramdisk_cpio_name);
		}
		if( HAS_RAMDISK_DIRECTORY )
				fprintf(stderr,"extracting ramdisk:%s\n",option_values.ramdisk_directory_name);
						
		process_uncompressed_ramdisk(uncompressed_ramdisk_data,uncompressed_ramdisk_size,option_values.ramdisk_name);
			
		free(uncompressed_ramdisk_data);
		
	}
	if	( HAS_KERNEL ){
		write_to_file(boot_image.kernel_data_start,boot_image.header.kernel_size,option_values.kernel_name);
		
	}
	if	( HAS_SECOND ){
		write_to_file(boot_image.second_data_start,boot_image.header.second_size,option_values.second);
		fprintf(stderr,"extracting second:%s\n",option_values.second);
	}
	//chdir(starting_directory);	
		
quit_now:
	//free(option_values.output);
	free(raw_boot_image_data);
	return 0;
}

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
int pack_boot_image_file(){
	
	
	char starting_directory[PATH_MAX];
	getcwd(starting_directory,PATH_MAX);
	log_write("create_boot_image_file:kernel:%s:ramdisk_directory:%s image:%s starting_directory=%s\n",option_values.kernel_name,option_values.ramdisk_directory_name,option_values.image_filename,starting_directory);
	unsigned char *ramdisk_gzip_data = NULL, *kernel_data = NULL ,*second_data = NULL,*cmdline_data = NULL;
	unsigned long ramdisk_gzip_size = 0, kernel_size=0, second_size = 0;
	unsigned pagemask = option_values.page_size - 1; 
	if ( HAS_RAMDISK_DIRECTORY ){
		log_write("create_boot_image_file:packing_ramdisk_directory\n");
		unsigned char* ramdisk_cpio_data = calloc(MEMORY_BUFFER_SIZE, sizeof(unsigned char)) ;
		unsigned long cpio_size = pack_ramdisk_directory(ramdisk_cpio_data);
		log_write("create_boot_image_file:cpio_size=%lu\n",cpio_size);
		ramdisk_gzip_data = calloc(cpio_size, sizeof(unsigned char));
		ramdisk_gzip_size = compress_gzip_ramdisk_memory(ramdisk_cpio_data,cpio_size,ramdisk_gzip_data,cpio_size);
		log_write("create_boot_image_file:ramdisk_gzip_size=%lu\n",ramdisk_gzip_size);
		free(ramdisk_cpio_data);
		chdir(starting_directory);
		
	}
	if ( HAS_RAMDISK_CPIO ){
		unsigned long cpio_size  ;
		unsigned char *ramdisk_cpio_data = load_file( option_values.ramdisk_name,&cpio_size);
		ramdisk_gzip_data = calloc(cpio_size, sizeof(unsigned char));
		ramdisk_gzip_size = compress_gzip_ramdisk_memory(ramdisk_cpio_data,cpio_size,ramdisk_gzip_data,cpio_size);
		free(ramdisk_cpio_data);
	}
	if ( HAS_RAMDISK_ARCHIVE ){
		ramdisk_gzip_data = load_file( option_values.ramdisk_name,&ramdisk_gzip_size );
		
	}
	if ( HAS_KERNEL ) {
		kernel_data=load_file(option_values.kernel_name,&kernel_size);
		
	}
	if ( HAS_SECOND ) {
		second_data=load_file(option_values.second,&second_size);
	}
	
	
	boot_img_hdr boot_image_header = { BOOT_MAGIC,kernel_size, 
			DEFAULT_KERNEL_ADDRESS,ramdisk_gzip_size,
			DEFAULT_RAMDISK_ADDRESS,second_size,DEFAULT_SECOND_ADDRESS,
			DEFAULT_TAGS_ADDRESS,option_values.page_size,
			.name = {'\0'},.unused={'\0'},.cmdline={'\0'},.id={'\0'}
			};
	if ( HAS_CMDLINE ){ 
		  read_file_to_size(option_values.cmdline,BOOT_ARGS_SIZE,boot_image_header.cmdline);
	}	 
		 
	if ( HAS_BOARD ) {
		read_file_to_size(option_values.board,BOOT_ARGS_SIZE,boot_image_header.name);
	}
	struct dirent *entry;	
	
	
	get_content_hash(&boot_image_header,kernel_data,ramdisk_gzip_data,second_data);
	 
	int header_padding =  option_values.page_size - (sizeof(boot_image_header) & pagemask);
	int ramdisk_padding = option_values.page_size - (ramdisk_gzip_size & pagemask);
	int kernel_padding = option_values.page_size - (kernel_size & pagemask);
	log_write("header_padding:count=%d\n",header_padding);
	log_write("kernel_padding:count=%d\n",kernel_padding);
	log_write("ramdisk_padding:count=%d\n",ramdisk_padding);
	int second_padding =  option_values.page_size - (second_size & pagemask);
	
	FILE*fp = fopen(option_values.image_filename,"wb");
	int fd = fileno(fp); 
	if(write(fd, &boot_image_header,sizeof(boot_image_header)) != sizeof(boot_image_header)) goto fail;
    if(write(fd, padding, header_padding) != header_padding) goto fail;

    if(write(fd, kernel_data, boot_image_header.kernel_size) != (int)boot_image_header.kernel_size) goto fail;
   if(write(fd, padding, kernel_padding) != kernel_padding ) goto fail;

    if(write(fd, ramdisk_gzip_data, boot_image_header.ramdisk_size) !=(int) boot_image_header.ramdisk_size) goto fail;
    if(write(fd, padding, ramdisk_padding ) != ramdisk_padding ) goto fail;
   

    if(HAS_SECOND) {
        if(write(fd, second_data, boot_image_header.second_size) !=(int) boot_image_header.second_size) goto fail;
        if(write(fd, padding, second_padding ) != second_padding ) goto fail;
        free(second_data);
    }
    
    fclose(fp);
    free(kernel_data);
	free(ramdisk_gzip_data);
	close(fd);
	return 0;
fail:
    unlink("boot.img");
    close(fd);
    fprintf(stderr,"error: failed writing '%s'\n",
            strerror(errno));
    return 1;
}
int list_boot_image_info()
{
	unsigned long file_size =0 ;
	byte_p raw_boot_image_data = load_file(option_values.image_filename,&file_size);
	boot_image_t boot_image=parse_boot_image_info(raw_boot_image_data,file_size);
	print_boot_image_info(boot_image,stderr);
	
quit_now:
	free(raw_boot_image_data);
	return 0;
}
int extract_boot_image_file(){
	
	unsigned long file_size =0 ;
	byte_p raw_boot_image_data = load_file(option_values.image_filename,&file_size);
	boot_image_t boot_image=parse_boot_image_info(raw_boot_image_data,file_size);
	byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
	unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(boot_image.ramdisk_data_start,boot_image.header.ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
	find_file_in_ramdisk_entries(uncompressed_ramdisk_data);
	free(uncompressed_ramdisk_data);
	free(raw_boot_image_data);
	return 0;
}
int update_boot_image_file(){
		
	unsigned long file_size =0,new_cpio_file_size=0;
	byte_p raw_boot_image_data = load_file(option_values.image_filename,&file_size);
	boot_image_t boot_image=parse_boot_image_info(raw_boot_image_data,file_size);
	byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
	
	unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(boot_image.ramdisk_data_start,boot_image.header.ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
	log_write("modify_size:%ld\n",uncompressed_ramdisk_size);
	byte_p new_cpio_data = modify_ramdisk_entry(uncompressed_ramdisk_data,uncompressed_ramdisk_size,&new_cpio_file_size);
	// nothing done
	if(new_cpio_data==uncompressed_ramdisk_data) goto quit_now;
	byte_p ramdisk_gzip_data = calloc(new_cpio_file_size, sizeof(unsigned char));
	unsigned long ramdisk_gzip_size = compress_gzip_ramdisk_memory(new_cpio_data,new_cpio_file_size,ramdisk_gzip_data,new_cpio_file_size);
	unsigned pagemask =boot_image.header.page_size - 1; 
	
	int ramdisk_padding = boot_image.header.page_size - (ramdisk_gzip_size & pagemask);
	int header_padding = boot_image.header.page_size - (sizeof(boot_image.header) & pagemask);
	int kernel_padding = boot_image.header.page_size - (boot_image.header.kernel_size & pagemask);
	log_write("ramdisk_size:%ld\n",boot_image.header.ramdisk_size);
	boot_image.header.ramdisk_size = ramdisk_gzip_size;
	get_content_hash(&boot_image.header,boot_image.kernel_data_start,ramdisk_gzip_data,boot_image.second_data_start);
	log_write("ramdisk_size:%ld ramdisk_padding:%ld\n",boot_image.header.ramdisk_size,ramdisk_padding);
	int second_padding =  option_values.page_size - (boot_image.header.second_size & pagemask);
	
	FILE*fp = fopen(option_values.image_filename,"wb");
	int fd = fileno(fp); 
	if(write(fd, &boot_image.header,sizeof( boot_image.header)) != sizeof( boot_image.header)) goto fail;
    if(write(fd, padding, header_padding) != header_padding) goto fail;

    if(write(fd,  boot_image.kernel_data_start,  boot_image.header.kernel_size) != (int) boot_image.header.kernel_size) goto fail;
   if(write(fd, padding, kernel_padding) != kernel_padding ) goto fail;

    if(write(fd, ramdisk_gzip_data, boot_image.header.ramdisk_size) !=(int) boot_image.header.ramdisk_size) goto fail;
    if(write(fd, padding, ramdisk_padding ) != ramdisk_padding ) goto fail;
   

    if(HAS_SECOND) {
        if(write(fd, boot_image.second_data_start, boot_image.header.second_size) !=(int) boot_image.header.second_size) goto fail;
        if(write(fd, padding, second_padding ) != second_padding ) goto fail;
    }
	
fail:
	fclose(fp);
	free(ramdisk_gzip_data);
	free(new_cpio_data);
	
quit_now:
	free(uncompressed_ramdisk_data);	
	free(raw_boot_image_data);
	return 0;
	
}
