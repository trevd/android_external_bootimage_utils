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

#include <errno.h>
#include <time.h>

#include "mincrypt/sha.h"
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
static boot_image_t populate_boot_image_info_struct(unsigned char *data,unsigned file_size);



static unsigned long find_magic_offset(unsigned char *data , unsigned size){
	long offset=-1;
	long* poffset=&offset ;
		poffset= memmem1((const unsigned char*)data,size,BOOT_MAGIC,BOOT_MAGIC_SIZE);

	if(poffset==0) return -1;
	return  0-( (unsigned long)&data[0]-(unsigned long)&poffset[0]);
	
}
int print_boot_image_info(boot_image_t boot_image)
{	
	unsigned char *boot_image_info = (unsigned char *)malloc(4096);
	sprintf((char *)boot_image_info,
					"name:%s\n"
					"cmdline:%s\n"
					"page_size:%d\t[0x%08x]\n"
					"kernel_addr:%u [0x%08x]\n"
					"kernel_size:%u [0x%08x]\n"
					"kernel_pagecount:%u [0x%08x]\n"
					"kernel_offset:%u [0x%08x]\n"
					"ramdisk_addr:%u  [0x%08x]\n"
					"ramdisk_size:%u [0x%08x]\n"
					"ramdisk_pagecount:%d [0x%08x]\n"
					"ramdisk_offset:%d [0x%08x]\n"
					"second_addr:%x [0x%08x]\n"
					"second_size:%d [0x%08x]\n"
					"second_pagecount:%d [0x%08x]\n"
					"second_offset:%d\t[0x%08x]\n",
		    boot_image.header.name,boot_image.header.cmdline,
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
			boot_image.second_offset,boot_image.second_offset);
	if ( HAS_HEADER )
		write_to_file((unsigned char *)boot_image_info,(unsigned) strlen((const char *)boot_image_info),option_values.header);
	
	fprintf(stderr,"%s", boot_image_info);
	return 0;
	
}


static boot_image_t populate_boot_image_info_struct(unsigned char *data,unsigned file_size)
{
	boot_image_t boot_image;
	memcpy(&boot_image.header,data,sizeof(boot_img_hdr));
	if(strncmp((const char *) boot_image.header.magic,BOOT_MAGIC,BOOT_MAGIC_SIZE)){
		fprintf(stderr,"Not a valid Boot Image\n");
		free(data);
		exit(0);
	}
	boot_image.boot_image_filesize = file_size;
	boot_image.kernel_page_count = (boot_image.header.kernel_size + boot_image.header.page_size - 1) / boot_image.header.page_size;
	boot_image.ramdisk_page_count = (boot_image.header.ramdisk_size + boot_image.header.page_size - 1) / boot_image.header.page_size;
	boot_image.second_page_count =  (boot_image.header.second_size + boot_image.header.page_size - 1) / boot_image.header.page_size;
	
	boot_image.kernel_offset = boot_image.header.page_size;
	boot_image.ramdisk_offset = boot_image.kernel_offset + (boot_image.kernel_page_count * boot_image.header.page_size);	
	boot_image.second_offset = boot_image.ramdisk_offset + (boot_image.ramdisk_page_count *  boot_image.header.page_size);
	return boot_image;
	
}
int unpack_boot_image_file()
{
	fprintf(stderr,"image:\"%s\"\n" , option_values.image);
	unsigned long 	file_size =0 ;
	unsigned char *raw_boot_image_data = load_file( option_values.image,&file_size);
	if(file_size < sizeof(boot_img_hdr)) {
		fprintf(stderr,"WTF! ARE YOU THINKING! Bytes Read is Less Than Even The Size of the Header. Boot Image Corrupt\n");
		goto quit_now;
	}
	fprintf(stderr,"size:%ld [0x%lx] Bytes\n" , file_size,file_size);
	long magic = find_magic_offset(raw_boot_image_data,file_size);
	if(magic==-1)
	{
		fprintf(stderr,"ERROR:could not find boot image magic!\n" );
		goto quit_now;
	}
	
	fprintf(stderr,"magic offset:");
	fprintf(stderr,"%ld [0x%lx]\n" , magic,magic);
	fprintf(stderr,"output directory:%s\n",option_values.output);	
	unsigned char *offset_raw_boot_image_data = raw_boot_image_data+magic;
	boot_image_t boot_image = populate_boot_image_info_struct(offset_raw_boot_image_data,file_size-magic); 
	int mkdir_result=0;
	if((mkdir_result = mkdir(option_values.output,0777)))
	{
		
		//fprintf(stderr,"mkdir output:%s Error:%d\n",option_values.output,mkdir_result);
	}
	char starting_directory[PATH_MAX];
	getcwd(starting_directory,PATH_MAX);
	chdir(option_values.output);
	if ( HAS_HEADER ){
		fprintf(stderr,"extracting header info:%s\n",option_values.header);
		print_boot_image_info(boot_image);
		
	}	
	if( HAS_CMDLINE ){
		fprintf(stderr,"extracting command line:%s\n",option_values.cmdline);
		write_single_line_to_file((const char*)option_values.cmdline,(const char*)boot_image.header.cmdline,strlen((const char*)boot_image.header.cmdline));
	}
	if( HAS_RAMDISK_ARCHIVE  ){
		
			char ramdisk_archive_name[PATH_MAX] ;
			strcpy(ramdisk_archive_name,option_values.ramdisk );
			strcat(ramdisk_archive_name,".cpio.gz");
			write_to_file(offset_raw_boot_image_data+boot_image.ramdisk_offset,boot_image.header.ramdisk_size,ramdisk_archive_name);
			fprintf(stderr,"extracting gzip ramdisk:%s\n",ramdisk_archive_name);

	}
	if(	HAS_RAMDISK_CPIO || HAS_RAMDISK_DIRECTORY ){
		
		unsigned char *uncompressed_ramdisk_data = (unsigned char *) malloc(MEMORY_BUFFER_SIZE) ;
		unsigned long ramdisk_uncompress_size =	uncompress_gzip_ramdisk_memory(offset_raw_boot_image_data+boot_image.ramdisk_offset,boot_image.header.ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
		
		if( HAS_RAMDISK_CPIO ){
			char ramdisk_cpio_name[PATH_MAX] ;
			strcpy(ramdisk_cpio_name,option_values.ramdisk );
			strcat(ramdisk_cpio_name,".cpio");
			write_to_file(uncompressed_ramdisk_data,ramdisk_uncompress_size,ramdisk_cpio_name);
			fprintf(stderr,"extracting cpio ramdisk:%s\n",ramdisk_cpio_name);
		}
		if( HAS_RAMDISK_DIRECTORY )
				fprintf(stderr,"extracting ramdisk:%s\n",option_values.ramdisk);
				
		process_uncompressed_ramdisk(uncompressed_ramdisk_data,ramdisk_uncompress_size,option_values.ramdisk);
			
		free(uncompressed_ramdisk_data);
		
	}
	
	
	if	( HAS_KERNEL ){
		write_to_file(offset_raw_boot_image_data+boot_image.kernel_offset,boot_image.header.kernel_size,option_values.kernel);
		fprintf(stderr,"extracting kernel at %u in %s to %s/%s\n",boot_image.kernel_offset,option_values.image ,option_values.output, option_values.kernel);
	}
	if	( HAS_SECOND ){
		write_to_file(offset_raw_boot_image_data+boot_image.second_offset,boot_image.header.second_size,option_values.second);
		fprintf(stderr,"extracting second:%s\n",option_values.second);
	}
	chdir(starting_directory);	
		
quit_now:
	free(option_values.output);
	//free(offset_raw_boot_image_data);
	free(raw_boot_image_data);
	return 0;
}



static unsigned char padding[4096] = { 0, };

int write_padding(int fd, unsigned pagesize, unsigned itemsize)
{
    unsigned pagemask = pagesize - 1;
    int count;
    if((itemsize & pagemask) == 0) {
        return 0;
    }

    count = pagesize - (itemsize & pagemask);

    if(write(fd, padding, count) != count) {
        return -1;
    } else {
        return 0;
    }
}
int populate_boot_image_header(boot_img_hdr* header,unsigned char *name,unsigned char *cmdline, unsigned  page_size,unsigned long  kernel_size,unsigned long  ramdisk_size,unsigned long second_size)
{
	header->page_size=page_size;
	header->kernel_size=kernel_size;
	header->ramdisk_size=ramdisk_size;
	header->second_size=second_size;
	strcpy((char*)header->cmdline,(char*)cmdline);
	strcpy((char*)header->name,(char*)name);
	memcpy(header->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE);
	return 0;
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
int print_boot_image_header_info(boot_img_hdr boot_image_header)
{
	fprintf(stderr,"boot_image_header :\n"
						"page_size:%d\n"
						"kernel_size:%d\n",
		   boot_image_header.page_size,
		   boot_image_header.kernel_size);
	return 0;
	
}

int create_boot_image_file(){
	
    //printf("out:%ld",offset);
	//write_to_file(ramdisk_cpio_data,offset,"packed-ramdisk.cpio");
	char starting_directory[PATH_MAX];
	getcwd(starting_directory,PATH_MAX);
	
	unsigned char *ramdisk_gzip_data = NULL, *kernel_data = NULL ,*second_data = NULL,*cmdline_data = NULL;
	unsigned long ramdisk_gzip_size = 0, kernel_size=0, second_size = 0;
	if ( HAS_RAMDISK_DIRECTORY ){
		printf("input:ramd\n");
		unsigned char* ramdisk_cpio_data = calloc(MEMORY_BUFFER_SIZE, sizeof(unsigned char)) ;
		unsigned long cpio_size = pack_ramdisk_directory(ramdisk_cpio_data);
		ramdisk_gzip_data = calloc(cpio_size, sizeof(unsigned char));
		ramdisk_gzip_size = compress_gzip_ramdisk_memory(ramdisk_cpio_data,cpio_size,ramdisk_gzip_data,cpio_size);
		free(ramdisk_cpio_data);
		chdir(starting_directory);
		
	}
	if ( HAS_RAMDISK_CPIO ){
		unsigned long cpio_size  ;
		unsigned char *ramdisk_cpio_data = load_file( option_values.ramdisk,&cpio_size);
		ramdisk_gzip_data = calloc(cpio_size, sizeof(unsigned char));
		ramdisk_gzip_size = compress_gzip_ramdisk_memory(ramdisk_cpio_data,cpio_size,ramdisk_gzip_data,cpio_size);
		free(ramdisk_cpio_data);
	}
	if ( HAS_RAMDISK_ARCHIVE ){
		ramdisk_gzip_data = load_file( option_values.ramdisk,&ramdisk_gzip_size );
	}
	if ( HAS_KERNEL ) {
		kernel_data=load_file(option_values.kernel,&kernel_size);
	}
	if ( HAS_SECOND ) {
		second_data=load_file(option_values.second,&second_size);
	}
	cmdline_data = calloc(BOOT_ARGS_SIZE,sizeof(unsigned char));
	if ( HAS_CMDLINE ){
		  
		  read_file_to_size(option_values.cmdline,BOOT_ARGS_SIZE,cmdline_data);
	}else
		 strncpy((char *)cmdline_data,"\0",1);
		
	if ( HAS_BOARD ) {
	}
	struct dirent *entry;	
	boot_img_hdr boot_image_header ;
	unsigned base           = 0x10000000;
    unsigned kernel_offset  = 0x00008000;
    unsigned ramdisk_offset = 0x01000000;
    unsigned second_offset  = 0x00f00000;
    unsigned tags_offset    = 0x00000100;

    boot_image_header.kernel_addr =  base + kernel_offset;
    boot_image_header.ramdisk_addr = base + ramdisk_offset;
    boot_image_header.second_addr =  base + second_offset;
    boot_image_header.tags_addr =    base + tags_offset;
	populate_boot_image_header(&boot_image_header,(unsigned char*)"",cmdline_data,option_values.page_size,kernel_size,ramdisk_gzip_size,second_size);
	get_content_hash(&boot_image_header,kernel_data,ramdisk_gzip_data,second_data);
	print_boot_image_header_info(boot_image_header);
	
	FILE*fp = fopen(option_values.output,"wb");
	int fd = fileno(fp); 
	if(write(fd, &boot_image_header, sizeof(boot_image_header)) != sizeof(boot_image_header)) goto fail;
    if(write_padding(fd, option_values.page_size, sizeof(boot_image_header))) goto fail;

    if(write(fd, kernel_data, boot_image_header.kernel_size) != (int)boot_image_header.kernel_size) goto fail;
    if(write_padding(fd, option_values.page_size, boot_image_header.kernel_size)) goto fail;

    if(write(fd, ramdisk_gzip_data, boot_image_header.ramdisk_size) !=(int) boot_image_header.ramdisk_size) goto fail;
    if(write_padding(fd, option_values.page_size, boot_image_header.ramdisk_size)) goto fail;

    if(HAS_SECOND) {
        if(write(fd, second_data, boot_image_header.second_size) !=(int) boot_image_header.second_size) goto fail;
        if(write_padding(fd, option_values.page_size, boot_image_header.ramdisk_size)) goto fail;
        free(second_data);
    }
    
    fclose(fp);
    free(kernel_data);
	free(ramdisk_gzip_data);
	
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
	fprintf(stderr,"Loading Boot Image: \"%s\" into memory...." , option_values.image);
	unsigned long 	file_size =0 ;
	unsigned char *raw_boot_image_data = load_file( option_values.image,&file_size);
	if(file_size < sizeof(boot_img_hdr)) {
		fprintf(stderr,"WTF! ARE YOU THINKING! Bytes Read is Less Than Even The Size of the Header. Boot Image Corrupt\n");
		goto quit_now;
	}
	fprintf(stderr,"Loaded %ld [0x%lx] Bytes\n" , file_size,file_size);
	fprintf(stderr,"Searching For Bootloader Magic String ANDROID!.....");
	long magic = find_magic_offset(raw_boot_image_data,file_size);
	if(magic==-1)
	{
		fprintf(stderr,"Not Found!\n" );
		goto quit_now;
	}
	fprintf(stderr,"Found at offset %ld [0x%lx]\n" , magic,magic);
	unsigned char *offset_raw_boot_image_data = raw_boot_image_data+magic;
	boot_image_t boot_image = populate_boot_image_info_struct(offset_raw_boot_image_data,file_size-magic); 
	print_boot_image_info(boot_image);
	
quit_now:
	free(raw_boot_image_data);
	return 0;
}
int extract(){
	
	unsigned long file_size =0 ;
	fprintf(stderr,"Loading Boot Image: \"%s\" into memory...." , option_values.image);
	unsigned char *raw_boot_image_data = load_file( option_values.image,&file_size);
	if(file_size < sizeof(boot_img_hdr)) {
		fprintf(stderr,"WTF! ARE YOU THINKING! Bytes Read is Less Than Even The Size of the Header. Boot Image Corrupt\n");
		exit(0);
	}
	fprintf(stderr,"Loaded %ld [0x%lx] Bytes\n" , file_size,file_size);
	fprintf(stderr,"Searching For Bootloader Magic String ANDROID!.....");
	long magic = find_magic_offset(raw_boot_image_data,file_size);
	if(magic==-1)
	{
		fprintf(stderr,"Not Found!\n" );
		free(raw_boot_image_data);
			exit(0);
	}
	fprintf(stderr,"Found at offset %ld [0x%lx]\n" , magic,magic);
	unsigned char *offset_raw_boot_image_data = raw_boot_image_data+magic;
	boot_image_t boot_image = populate_boot_image_info_struct(offset_raw_boot_image_data,file_size-magic); 
	unsigned char *uncompressed_ramdisk_data = (unsigned char *) malloc(MEMORY_BUFFER_SIZE) ;
	unsigned long ramdisk_uncompress_size =	uncompress_gzip_ramdisk_memory(offset_raw_boot_image_data+boot_image.ramdisk_offset,boot_image.header.ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
	unsigned long offset=0;
	while(offset != -1){
		offset = find_file_in_ramdisk_entries(uncompressed_ramdisk_data+offset,ramdisk_uncompress_size-offset,offset);
	}
	free(uncompressed_ramdisk_data);
	free(raw_boot_image_data);
	return 0;
	
}
