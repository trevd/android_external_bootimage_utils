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
#define HEADER_PADDING (header->page_size-sizeof(boot_img_hdr))
#define HEADER_END (position+sizeof(boot_img_hdr))
#define HEADER_SIZE (sizeof(boot_img_hdr))
#define KERNEL_START (HEADER_END+HEADER_PADDING)
#define KERNEL_END (KERNEL_START+header->kernel_size)
#define KERNEL_PADDING (header->page_size - (header->kernel_size & HEADER_PAGE_MASK))
#define RAMDISK_START (KERNEL_END+KERNEL_PADDING)
#define RAMDISK_END (RAMDISK_START+header->ramdisk_size)
#define RAMDISK_PADDING (header->page_size - (header->ramdisk_size & HEADER_PAGE_MASK))
#define SECOND_START (RAMDISK_END+RAMDISK_PADDING)
#define SECOND_END (SECOND_START+header->second_size)
#define SECOND_PADDING (header->page_size - (header->second_size & HEADER_PAGE_MASK))
static unsigned char padding[MAXIMUM_KNOWN_PAGE_SIZE] = { 0, };
boot_img_hdr* load_boot_image_header(FILE *fp){
	
	if(!fp){
		PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE	
		fprintf(stderr,"\nerror opening file %s errno:%d %s\n",option_values.image_filename,errno,strerror(errno));
		exit(1);
	}
	fseek(fp,0,SEEK_END);
	unsigned long  size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	long int position = ftell(fp);
	if(size<sizeof(boot_img_hdr)){
		PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE	
		fprintf(stderr,"\nunexpected filesize %ld\n",(long int)size);
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
			PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE	
			fprintf(stderr,"\nerror opening file %s errno:%d %s\n",option_values.image_filename,errno,strerror(errno));
			fclose(fp);
			exit(0);
		}
		//fprintf(stderr,"bytes_read  %ld size %ld\n",(long int)bytes_read,size);
		if ((offset=find_in_memory(header_buffer,size,BOOT_MAGIC,BOOT_MAGIC_SIZE)) == NULL){
			help_error_boot_magic(option_values.image_filename);
			fclose(fp);
			exit(0);
		}
	}
	fseek(fp,offset-header_buffer,SEEK_SET);
	return (boot_img_hdr*)offset;
}
int get_content_hash(boot_img_hdr* header,byte_p kernel_data,byte_p ramdisk_data,byte_p second_data)
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
static int write_boot_image(FILE* boot_image_file,boot_img_hdr* header,byte_p kernel_data,byte_p ramdisk_data,byte_p second_data)
{
	unsigned pagemask = header->page_size - 1; 
	size_t header_size = (size_t)sizeof(boot_img_hdr);
	get_content_hash(header,kernel_data,ramdisk_data,second_data);
	size_t ramdisk_padding = header->page_size - (header->ramdisk_size & pagemask);
	size_t header_padding = header->page_size - (header_size & pagemask);
	size_t kernel_padding = header->page_size - (header->kernel_size & pagemask);
	size_t second_padding =  header->page_size - (header->second_size & pagemask);
	fprintf(stderr,"write_boot_image Header P:%d S:%u T:%u\n",header_padding,header_size,header_size+header_padding); 
	fprintf(stderr,"write_boot_image Kernel P:%d S:%u T:%u\n",kernel_padding,header->kernel_size,kernel_padding+header->kernel_size); 
	//log_write("write_boot_image Kernel P:%d S:%d\n",kernel_padding,header->kernel_size,kernel_padding+header->kernel_size); 
	fprintf(stderr,"write_boot_image Ramdisk P:%d S:%u T:%u\n",ramdisk_padding,header->ramdisk_size,ramdisk_padding+header->ramdisk_size); 
	
	if(fwrite(header,1,header_size,boot_image_file) !=  header_size) goto header1_fail;
	
    if(fwrite(padding,1,header_padding,boot_image_file) != header_padding) goto header_fail;

    if(fwrite(kernel_data,1,  header->kernel_size,boot_image_file) !=  header->kernel_size) goto kernel_fail;
	if(fwrite(padding,1,kernel_padding,boot_image_file) != kernel_padding ) goto kernel_fail;

    if(fwrite(ramdisk_data,1, header->ramdisk_size,boot_image_file) != header->ramdisk_size) goto ramdisk_fail;
    if(fwrite(padding,1,ramdisk_padding,boot_image_file) != ramdisk_padding ) goto ramdisk_fail;
    
    
    if(header->second_size>0) {
        if(fwrite(second_data,1,header->second_size,boot_image_file) != header->second_size) goto second_fail;
        if(fwrite(padding,1,second_padding,boot_image_file) != second_padding ) goto second_fail;
    }
    return 0;
    
header1_fail:
	fprintf(stderr,"HEADER1 FAIL\n");
	return 0; 
header_fail:
	fprintf(stderr,"HEADER FAIL\n");
	return 0; 
kernel_fail:
	fprintf(stderr,"KERNEL FAIL\n");
	return 0; 
ramdisk_fail:
	fprintf(stderr,"RAM FAIL\n");
	return 0; 
second_fail:
	fprintf(stderr,"SECOND FAIL\n");
	return 0; 
fail:
	fprintf(stderr,"FAIL\n");
	return 0; 
}
// Extract Kernel File
// boot_image_file : Boot Image File. File Position must be at the start of the kernel
// Returns : Sets boot_image_file file position to ramdisk entry
static int process_kernel_section(FILE* boot_image_file,boot_img_hdr* header){
	unsigned pagemask = header->page_size - 1;
	if(option_values.kernel_filename){
		fprintf(stderr,"Extracting Kernel %s size:%d\n",option_values.kernel_filename,header->kernel_size);
		byte_p buffer=malloc(header->kernel_size);
		fread(buffer,1,header->kernel_size,boot_image_file);
		write_to_file(buffer,header->kernel_size,option_values.kernel_filename);
		free(buffer);
		fprintf(stderr,"Extracting Kernel Complete\n");
	}else{
		fseek(boot_image_file,header->kernel_size,SEEK_CUR); 
	}
	fseek(boot_image_file,(header->page_size - (header->kernel_size & pagemask)),SEEK_CUR);
	
	return 0;
} 

static int process_ramdisk_archive(boot_img_hdr* header,byte_p ramdisk_data){
	if(option_values.ramdisk_archive_filename)	
		write_to_file(ramdisk_data,header->ramdisk_size,option_values.ramdisk_archive_filename);
	
	if(	option_values.ramdisk_cpio_filename || option_values.ramdisk_directory_name || option_values.file_list ){
		
		byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
		unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(ramdisk_data,header->ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
		
		if( option_values.ramdisk_cpio_filename )
			write_to_file(uncompressed_ramdisk_data,uncompressed_ramdisk_size,option_values.ramdisk_cpio_filename);
		
		if( option_values.ramdisk_directory_name )	{
			process_uncompressed_ramdisk(uncompressed_ramdisk_data,uncompressed_ramdisk_size,option_values.ramdisk_directory_name);
		}
		
		if ( option_values.file_list ){
			int cpio_entry_count = 0 ; int i ; int ok_to_write=FILE_YES;
			cpio_entry_list_t** cpio_entries = get_cpio_entries(uncompressed_ramdisk_data,uncompressed_ramdisk_size,&cpio_entry_count);
			while(option_values.file_list[0]){
				fprintf(stderr,"option_values.file_list[0]:%s cpio_entry_count:%d\n",option_values.file_list[0],cpio_entry_count);
				for(i=0; i < cpio_entry_count ; i++ ){
					fprintf(stderr,"option_values.file_list[0]:%s cpio_entry_count:%d cpio_entries[%d]->name:%s\n",option_values.file_list[0],cpio_entry_count,i,cpio_entries[i]->name);
					if (!strlcmp(cpio_entries[i]->name,option_values.file_list[0])){
						if(ok_to_write!=FILE_ALL){
							if(check_file_exists(cpio_entries[i]->name)) {
								ok_to_write =confirm_file_replace(cpio_entries[i]->name,cpio_entries[i]->name);
							}else{ 
								ok_to_write=FILE_YES; 
							}
						}
						if(ok_to_write!=FILE_NO) extract_cpio_list_entry(cpio_entries[i]);
						break ;
					}
				}
				option_values.file_list++;	
			}
			free_cpio_entry_memory(cpio_entries,cpio_entry_count);
		}
		free(uncompressed_ramdisk_data);
	}
	fprintf(stderr,"ramdisk_processed:\n");
	return 0;
}
static int process_ramdisk_section(FILE* boot_image_file,boot_img_hdr* header){
	
	if(header->ramdisk_size==0){
		help_error_image_ramdisk_zero();
	}
	
	char ramdisk_data[header->ramdisk_size];
	unsigned pagemask = header->page_size - 1;
	long ramdisk_padding = header->page_size - (header->ramdisk_size & pagemask);
	if(option_values.ramdisk_archive_filename || option_values.ramdisk_directory_name || option_values.ramdisk_cpio_filename || option_values.file_list){
		//fprintf(stderr," Extracting Ramdisk\n");
		memset(ramdisk_data,0,header->ramdisk_size);
		//fprintf(stderr," Extracting Ramdisk\n");
		fread(ramdisk_data,1,header->ramdisk_size,boot_image_file);
		fseek(boot_image_file,ramdisk_padding,SEEK_CUR); 
		process_ramdisk_archive( header, ramdisk_data);
			
	}else{
		fseek(boot_image_file,header->ramdisk_size+ramdisk_padding,SEEK_CUR); 
		return 0;
	}
	
	
	//if(option_values.ramdisk_archive_filename || option_values.ramdisk_directory_name || option_values.ramdisk_cpio_filename)
		//free(ramdisk_data);
	return 0 ;	
		
}
static int process_header_section(FILE* boot_image_file,boot_img_hdr* header){
	if(option_values.header_filename){
		PRINT_EXTRACT_MESSAGE("boot image header",option_values.header_filename);
		
		FILE * header_file = fopen(option_values.header_filename,"wb");
		if(header_file){
			fprintf(header_file,"kernel_size:%u"EOL"kernel_address:0x%08x"EOL"ramdisk_size:%u"EOL"ramdisk_address:0x%08x"EOL"second_size:%u"EOL"second_address:0x%08x"EOL
			"tags_addr:0x%08x"EOL"page_size:%u"EOL"name:%s"EOL"cmdline:%s"EOL,
			header->kernel_size,header->kernel_addr,header->ramdisk_size,header->ramdisk_addr,
			header->second_size,header->second_addr,header->tags_addr,header->page_size,header->name,header->cmdline);
			fclose(header_file);
		}
	}
	if(option_values.board_filename){
		if(strlen((char*)header->name)==0){
			fprintf(stderr,"Board Name file not written : name header entry is empty!\n");
		}else{
			FILE * board_file = fopen(option_values.board_filename,"wb");			
			if(board_file){
				fprintf(board_file,"%s\n",header->name);
				fclose(board_file);
			}	
		}
	}
	if(option_values.cmdline_filename){
		if(strlen((char*)header->cmdline)==0){
			fprintf(stderr,"Cmdline file not written : cmdline header entry is empty!\n");
		}else{
			FILE * cmdline_file = fopen(option_values.cmdline_filename,"wb");
			if(cmdline_file){
				fprintf(cmdline_file,"%s\n",header->cmdline);
				fclose(cmdline_file);
			}
		}	
	}
	if(option_values.page_size_filename){
		FILE * pagesize_file = fopen(option_values.page_size_filename,"wb");
		if(pagesize_file){
			fprintf(pagesize_file,"%d\n",header->page_size);
			fclose(pagesize_file);
		}
	}
	fseek(boot_image_file , HEADER_PAGE_SIZE, SEEK_CUR);
	return 0;
}
static int process_second_section(FILE* boot_image_file,boot_img_hdr* header){
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

int create_boot_image_file(){
	
	//fprintf(stderr,"create_boot_image_file\n");
	char starting_directory[PATH_MAX];
	memset(&starting_directory,'\0',PATH_MAX);
	//getcwd(starting_directory,PATH_MAX);
	//log_write("create_boot_image_file:kernel:%s:ramdisk_directory:%s image:%s starting_directory=%s\n",option_values.kernel_filename,option_values.ramdisk_directory_name,option_values.image_filename,starting_directory);
	byte_p ramdisk_gzip_data = NULL, kernel_data = NULL ,second_data = NULL,cmdline_data = NULL;
	size_t ramdisk_gzip_size = 0, kernel_size=0, second_size = 0;
	//fprintf(stderr,"create_boot_image_file 1\n");
	unsigned pagemask = option_values.page_size - 1; 
	if ( (option_values.ramdisk_directory_name) || (option_values.ramdisk_cpio_filename ) ){
		
		//log_write("create_boot_image_file:packing_ramdisk_directory\n");
		size_t cpio_size =0 ; 
		byte_p ramdisk_cpio_data =NULL;
		
		if (option_values.ramdisk_directory_name) {
			ramdisk_cpio_data = calloc(MEMORY_BUFFER_SIZE, sizeof(unsigned char)) ;
			cpio_size = pack_ramdisk_directory(ramdisk_cpio_data); 
			
		
		}else if ( option_values.ramdisk_cpio_filename  ){
			ramdisk_cpio_data = load_file( option_values.ramdisk_cpio_filename ,&cpio_size);
		}
		
		ramdisk_gzip_data = calloc(cpio_size, sizeof(unsigned char));
		ramdisk_gzip_size = compress_gzip_ramdisk_memory(ramdisk_cpio_data,cpio_size,ramdisk_gzip_data,cpio_size);
		fprintf(stderr,"create_boot_image_file:ramdisk_gzip_size=%u	\n",ramdisk_gzip_size);
		free(ramdisk_cpio_data);
		chdir(starting_directory);
		
	}else if ( option_values.ramdisk_archive_filename ){
			//fprintf(stderr,"load_ramdisk 1\n");
			ramdisk_gzip_data = load_file( option_values.ramdisk_archive_filename,&ramdisk_gzip_size );
	}
	if ( option_values.kernel_filename ) {
		kernel_data=load_file(option_values.kernel_filename,&kernel_size);
		//fprintf(stderr,"load_kernel kernel_size %u\n",kernel_size);
	}
	if ( option_values.second_filename ) second_data=load_file(option_values.second_filename,&second_size);

	boot_img_hdr head;
	memset(&head, 0, sizeof(boot_img_hdr));
	boot_img_hdr* header=&head ;
	strncpy((char*)header->magic,BOOT_MAGIC,BOOT_MAGIC_SIZE) ;
	//fprintf(stderr,"boot_img_hdr %d\n",sizeof(boot_img_hdr));
	header->kernel_size = kernel_size;
	header->kernel_addr =  option_values.kernel_offset+option_values.base_address;
	header->ramdisk_size = ramdisk_gzip_size;
	header->ramdisk_addr =  option_values.ramdisk_offset+option_values.base_address;
	header->second_size = second_size;
	header->second_addr =  option_values.second_offset+option_values.base_address;
	header->tags_addr = option_values.tags_offset+option_values.base_address;
	header->page_size = option_values.page_size;
	//fprintf(stderr,"create_boot_image_file:kernel_size %d\n",kernel_size);
	if(option_values.board_name)
		strncpy((char*)header->name,option_values.board_name,BOOT_NAME_SIZE) ;
		
	if (option_values.cmdline_text)
		strncpy((char*)header->cmdline,option_values.cmdline_text,BOOT_ARGS_SIZE);	
	FILE*fp = fopen(option_values.image_filename,"wb");
	write_boot_image(fp,header,kernel_data,ramdisk_gzip_data,second_data);
    fclose(fp);
    if(second_size) free(second_data);
    free(kernel_data);
	free(ramdisk_gzip_data);
	return 0;
}

int extract_boot_image_file(){

	log_write("Extracting %s\n",option_values.image_filename);
	FILE* boot_image_file = fopen(option_values.image_filename,"r+b");
	boot_img_hdr* header = load_boot_image_header(boot_image_file);
	
	process_header_section(boot_image_file,header);
	process_kernel_section(boot_image_file,header);
	process_ramdisk_section(boot_image_file,header);
	process_second_section(boot_image_file,header);
	log_write("Extracting %s\n",option_values.image_filename);
	
	fclose(boot_image_file);
	return 0;
}
int list_boot_image_kernel_info(FILE* boot_image_file,boot_img_hdr* header,long int position){
	
	fseek(boot_image_file,KERNEL_START,SEEK_SET);
	byte_p kernel_data=malloc(header->kernel_size);
	fread(kernel_data,1,header->kernel_size,boot_image_file);
	byte_p zImage_magic = find_in_memory(kernel_data,header->kernel_size,magic_linux_zimage,4);
	
	byte_p uncompressed_kernel_data = (byte_p) malloc(LARGE_MEMORY_BUFFER_SIZE) ;
	unsigned long uncompressed_kernel_size;
	byte_p compression_magic = NULL; 
	if((compression_magic=find_in_memory(kernel_data,header->kernel_size,magic_gzip_deflate,3))){
		uncompressed_kernel_size=uncompress_gzip_ramdisk_memory(compression_magic,header->kernel_size,uncompressed_kernel_data,LARGE_MEMORY_BUFFER_SIZE);
	}else if((compression_magic=find_in_memory(kernel_data,header->kernel_size,magic_lzop,4))){ int a=1; }
	else if((compression_magic=find_in_memory(kernel_data,header->kernel_size,magic_xz,6))){		int a=1;}
	
	//fprintf(stderr,"\n kernel gzip magic number [%02x%02x%02x] found @ [0x%08x] %u \n",gzip_magic[0],gzip_magic[1],gzip_magic[2],gzip_magic-kernel_data,gzip_magic-kernel_data); 
	//fprintf(stderr,"\n kernel size %lu\n",uncompressed_kernel_size);
	
	byte_p version_string = find_in_memory(uncompressed_kernel_data,uncompressed_kernel_size,magic_linux_version,13);
	//write_to_file(uncompressed_kernel_data,uncompressed_kernel_size,"image");
	if(option_values.list_kernel){
		fprintf(stderr,"\n Kernel Information\n\n");
		if(!zImage_magic)
			fprintf(stderr," Type: unknown\n");
		else 
			fprintf(stderr," Type: ARM Linux zImage (little-endian)\n");

		switch(compression_magic[0]){
			case 0x1F: { 		fprintf(stderr," Compression: GZIP\n"); break; }
			case 'L': { 		fprintf(stderr," Compression: LZO\n"); break; }
			case '7': { 		fprintf(stderr," Compression: XZ\n"); break; }
			default: fprintf(stderr," Compression: Unknown\n");
		}
		fprintf(stderr," %s",version_string);
	}
	else if(option_values.list_kernel_version) fprintf(stderr," %s",version_string);
	
exit_function:
	free(kernel_data);	
	free(uncompressed_kernel_data);
	
	return 0;
}
int list_boot_image_info(){
	
	FILE* boot_image_file = fopen(option_values.image_filename,"r+b");
	boot_img_hdr* header = load_boot_image_header(boot_image_file); 
	long int position = ftell(boot_image_file);
		PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE
	if(option_values.list_header){
		
		fprintf(stderr," \n Android Boot Image Header Found In File %s Position [0x%08lx] %ld Header Size [0x%08x] %u\n\n",option_values.image_filename,position,position,HEADER_SIZE,HEADER_SIZE);
		fprintf(stderr," Boot Image File Header Information\n");
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
	}
	if(option_values.list_section){
		fprintf(stderr,"\n File Section Information\n\n");
		fprintf(stderr," Header:  Start: [0x%08lx] %8ld   End: [0x%08lx] %8ld Padding: [0x%08x] %8u\n", position,position, HEADER_END,HEADER_END,HEADER_PADDING,HEADER_PADDING);
		fprintf(stderr," Kernel:  Start: [0x%08lx] %8ld   End: [0x%08lx] %8ld Padding: [0x%08x] %8u\n",KERNEL_START,KERNEL_START, KERNEL_END,KERNEL_END,KERNEL_PADDING,KERNEL_PADDING);
		fprintf(stderr," Ramdisk: Start: [0x%08lx] %8lu   End: [0x%08lx] %8lu Padding: [0x%08x] %8u\n",RAMDISK_START,RAMDISK_START, RAMDISK_END,RAMDISK_END,RAMDISK_PADDING,RAMDISK_PADDING);
		fprintf(stderr," Second:  Start: [0x%08lx] %8lu   End: [0x%08lx] %8lu Padding: [0x%08x] %8u\n",SECOND_START,SECOND_START, SECOND_END,SECOND_END,SECOND_PADDING,SECOND_PADDING);
	}
	list_boot_image_kernel_info(boot_image_file,header,position);
	if(option_values.list_ramdisk){
		fseek(boot_image_file,RAMDISK_START,SEEK_SET);
		
		byte_p compressed_data=malloc(header->ramdisk_size);
		fread(compressed_data,1,header->ramdisk_size,boot_image_file);

		byte_p uncompressed_ramdisk_data = (byte_p) malloc(MEMORY_BUFFER_SIZE) ;
		unsigned long uncompressed_ramdisk_size =	uncompress_gzip_ramdisk_memory(compressed_data,header->ramdisk_size,uncompressed_ramdisk_data,MEMORY_BUFFER_SIZE);
		
		byte_p result = find_string_in_memory(uncompressed_ramdisk_data,uncompressed_ramdisk_size,"default.prop");

		//while(result){
		//	byte_p cpio_start = result-CPIO_HEADER_SIZE;result+=1;
		//	if(!strstrlcmp((const char*)magic_cpio_ascii,6,(const char*)cpio_start,6)){
		//		cpio_entry_t cpio_entry ; // = populate_cpio_entry((const byte_p)(cpio_start));
		//		fprintf(stderr,"\n Ramdisk Information Information - Default Properties\n");
		//		fprintf(stderr," Default Properties\n\n");
		//		
		//		fprintf (stderr,"%*.*s\n",cpio_entry.file_size,cpio_entry.file_size, cpio_entry.file_start_p);
		//		break; 
		//	}else{	
		//		fprintf(stderr,"uncompressed_ramdisk_data looking again:  %p  size:%lu\n", uncompressed_ramdisk_data,uncompressed_ramdisk_size);
		//		result = find_string_in_memory(result,uncompressed_ramdisk_size-(result-uncompressed_ramdisk_data),option_values.file_list[0]);
		//	}
		//}
	
		
		free(uncompressed_ramdisk_data);
		free(compressed_data);
	}
	fclose(boot_image_file);
	fprintf(stderr,"\n");
	return 0;
	
	
	
}
// 
byte_p pick_kernel_data(FILE*boot_image_file, const boot_img_hdr* header,boot_img_hdr* new_header, int *rewrite){
	byte_p kernel_data=NULL;
	unsigned pagemask = header->page_size - 1;
	if(option_values.kernel_filename){
		// Using external kernel file. Move file pointer to ramdisk - before we adjust the header
		// Style Note: Probably should use a different variable for the new header values
		// to save confusion

		fprintf(stderr,"Updating boot image %s kernel with %s\n",option_values.image_filename,option_values.kernel_filename);	 
		size_t kernel_size=0;
		kernel_data = load_file(option_values.kernel_filename,	&kernel_size);
		if(memcmp(kernel_data+magic_linux_zimage_offset,magic_linux_zimage,4)){
			fprintf(stderr,"%s is not a valid kernel zImage\n",option_values.kernel_filename);
			fclose(boot_image_file);
			exit(0);
		};
		new_header->kernel_size=kernel_size;
		fseek(boot_image_file,header->kernel_size,SEEK_CUR); 
		(*rewrite)=1;
	}else{ //not updating the kernel. Read the original
		kernel_data=malloc(header->kernel_size);
		fread(kernel_data,1,header->kernel_size,boot_image_file);
		
	}
	fseek(boot_image_file,(header->page_size - (header->kernel_size & pagemask)),SEEK_CUR);
	return kernel_data;
}

int update_boot_image_file(){
			
	FILE* boot_image_file = fopen(option_values.image_filename,"r+b");
	boot_img_hdr* header = load_boot_image_header(boot_image_file); 
	long int position = ftell(boot_image_file);
	
	
	boot_img_hdr new_header ;
	memcpy(&new_header,header,sizeof(boot_img_hdr));
	unsigned pagemask = header->page_size - 1;
	fseek(boot_image_file , header->page_size, SEEK_CUR);
	
	byte_p ramdisk_gzip_data = NULL , kernel_data  =NULL ,second_data= NULL; int rewrite=0;
	kernel_data = pick_kernel_data(boot_image_file,header,&new_header,&rewrite);
	
	if(option_values.property_list){
		int properties_total=0; int i=0;int propertycounter=-1;
		long ramdisk_position = ftell(boot_image_file); 
		default_property_list_t**  properties =get_default_properties_from_file(boot_image_file ,header->ramdisk_size,&properties_total);
		while(option_values.property_list[++propertycounter]){
			char *key=option_values.property_list[propertycounter];
			char *value = strchr(key, '=');	int keylength =value-key; key[keylength]='\0'; value++;
			
			for(i=0; i< properties_total;i++) {
				//fprintf(stderr,"option_values.property_list %s %s keylength:%d\n",properties[i]->key,key,strlen(properties[i]->key));
				if(!strlcmp(properties[i]->key,key)){
					fprintf(stderr,"updating %s %s\n",properties[i]->key,properties[i]->value);
					properties[i]->value=value;
					fprintf(stderr,"updating %s %s\n",properties[i]->key,properties[i]->value);
					break;
				}
			}
		}
		fseek(boot_image_file ,ramdisk_position, SEEK_SET);
		ramdisk_gzip_data= update_default_properties_in_gzip(boot_image_file,header->ramdisk_size,properties,properties_total,&new_header.ramdisk_size);
		fseek(boot_image_file ,ramdisk_position+header->ramdisk_size, SEEK_SET);
		rewrite=1;
		//FILE* cpio_out= fopen("ramdisk.cpio.gz","w+b");
		//fwrite(ramdisk_gzip_data,new_header.ramdisk_size,1,cpio_out);
		//fclose(cpio_out);		
		
		
		
	}else if(option_values.file_list){ //updating ramdisk 
		fseek(boot_image_file,RAMDISK_START,SEEK_SET);
		//fprintf(stderr,"updating files in the ramdisk in boot image %s\n",option_values.image_filename,ftell(boot_image_file));
		log_write("ramdisk_size:%ld\n",header->ramdisk_size);		
		int filecounter=-1;  int i;int cpio_entries_total = 0; unsigned uncompressed_ramdisk_size;
		cpio_entry_list_t** cpio_entries = get_cpio_entries_from_file(boot_image_file ,header->ramdisk_size,&uncompressed_ramdisk_size ,&cpio_entries_total);
		//fprintf(stderr,"loaded from file %d %s\n",cpio_entries_total,cpio_entries[0]->name);
		//fprintf(stderr,"Ramdisk size: %u\n", uncompressed_ramdisk_size);
		while(option_values.file_list[++filecounter]){
			byte_p new_data;
			for(i=0;i<cpio_entries_total;i++){
				if(!strlcmp(cpio_entries[i]->name,option_values.file_list[filecounter])){
					size_t filesize = 0;
					new_data=load_file(option_values.file_list[filecounter],&filesize);
					//fprintf(stderr,"m\tch %u %s %s\n",filesize,option_values.file_list[filecounter],cpio_entries[i]->data);
					sprintf(cpio_entries[i]->start.cpio_header->c_filesize,"%08x",filesize);
					cpio_entries[i]->data=new_data;
					uncompressed_ramdisk_size-=cpio_entries[i]->data_size;
					cpio_entries[i]->data_size=filesize+(((4 - (filesize) % 4)) % 4);
					uncompressed_ramdisk_size+=cpio_entries[i]->data_size;
					//fprintf(stderr,"m\tch %u %u %s %s\n",filesize,cpio_entries[i]->data_size,option_values.file_list[filecounter],cpio_entries[i]->data);
					break; 
				}
			}
		}
		
		ramdisk_gzip_data = compress_cpio_entries_to_gzip( cpio_entries,cpio_entries_total,uncompressed_ramdisk_size,&new_header.ramdisk_size);
		
		FILE* cpio_out= fopen("ramdisk.cpio.gz","w+b");
		fwrite(ramdisk_gzip_data,new_header.ramdisk_size,1,cpio_out);
		fclose(cpio_out);		
		//fprintf(stderr,"%p %s %u %u %p %p \n",cpio_entries[i]->start.position,cpio_entries[i]->name,cpio_entries[i]->name_padding,cpio_entries[i]->data_size,cpio_entries[i]->data,cpio_entries[i]->next);
		////ramdisk_gzip_data = calloc(uncompressed_ramdisk_size, sizeof(unsigned char));
		//new_header.ramdisk_size = compress_gzip_ramdisk_memory(ramdisk_cpio_data_start,uncompressed_ramdisk_size,ramdisk_gzip_data,uncompressed_ramdisk_size);
		//free(ramdisk_cpio_data);
		free_cpio_entry_memory(cpio_entries,cpio_entries_total) ;
		rewrite=1;	
	}else {
		fprintf(stderr,"Using Existing  ramdisk in boot image %s %u\n",option_values.image_filename,header->ramdisk_size);
		ramdisk_gzip_data=malloc(header->ramdisk_size);
		fread(ramdisk_gzip_data,1,header->ramdisk_size,boot_image_file);
	}
	fseek(boot_image_file,(header->page_size - (header->ramdisk_size & pagemask)),SEEK_CUR);
	second_data=malloc(header->second_size);
	fread(second_data,1,header->second_size,boot_image_file);
	fseek(boot_image_file,(header->page_size - (header->second_size & pagemask)),SEEK_CUR);
	if(rewrite){
		if(option_values.board_name)strncpy((char*)header->name,option_values.board_name,BOOT_NAME_SIZE) ;
		if (option_values.cmdline_text)strncpy((char*)header->cmdline,option_values.cmdline_text,BOOT_ARGS_SIZE);
		fclose(boot_image_file);
		FILE*fp = fopen(option_values.image_filename,"wb");
		write_boot_image(fp,&new_header,kernel_data,ramdisk_gzip_data,second_data);
		fclose(fp);
		
		free(kernel_data);
		free(ramdisk_gzip_data);
		free(second_data);
	}else{ 
		//fprintf(stderr,"NO rewrite %s\n",option_values.board_name);
		if(option_values.board_name){
			fseek(boot_image_file,header->name-(unsigned char*)header,SEEK_SET);
			fwrite(option_values.board_name,1,BOOT_NAME_SIZE,boot_image_file);	
		}
		if (option_values.cmdline_text){
			fseek(boot_image_file,header->cmdline-(unsigned char*)header,SEEK_SET);
			fwrite(option_values.cmdline_text,1,BOOT_ARGS_SIZE,boot_image_file);	
		}
		fclose(boot_image_file);
	}
	
	
	return 0;
	exit(0);
}
