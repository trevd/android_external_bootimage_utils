#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <bootimg.h>
#include <sha.h>
#include <bootimage.h>
#include <utils.h>
// Private defines
#define BOOT_IMAGE_SIZE_MAX (8192*1024)*4

#define MAXIMUM_KNOWN_PAGE_SIZE 4096

static unsigned char padding[MAXIMUM_KNOWN_PAGE_SIZE] = { 0, };

static size_t get_padding(size_t size,unsigned page_size){
	unsigned pagemask = page_size - 1;
	size_t padding =page_size - (size & pagemask);
	if(padding==page_size) padding =0 ; 
	return padding ; 
}
int set_boot_image_padding(boot_image* image){
	
	image->ramdisk_padding = get_padding(image->ramdisk_size,image->page_size);
	
	image->header_padding = get_padding(image->header_size,image->page_size);
	
	image->kernel_padding = get_padding(image->kernel_size,image->page_size);
	
	if(image->second_size > 0)
		image->second_padding = get_padding(image->second_size,image->page_size);	
}
int set_boot_image_offsets(boot_image*image)
{
	
	image->kernel_offset = image->header_offset + image->page_size;
	image->ramdisk_offset = image->kernel_offset + image->kernel_size + image->kernel_padding;
	if(image->second_size > 0){
		image->second_offset = image->ramdisk_offset + image->ramdisk_size + image->ramdisk_padding;
	}else{
		image->second_offset = -1;
	}
}

static int load_boot_image_into_memory(const char *filename, unsigned char** data,size_t *filesize){
	
	// Stat the file for the size
	struct stat sb;
	if (stat(filename, &sb) == -1) {
		return errno;
    }
	
	// Check the filesize. We will handle files upto
	// 32MB in size. A boot image should not be this size
	// anyway but we will try to handle it
	// any larger and you can just fuck off
	if(sb.st_size > BOOT_IMAGE_SIZE_MAX){
		errno = EFBIG;
		return errno;
	}
	(*filesize) = sb.st_size; 
	
	// Attempt to open the file in filename
	FILE* file = fopen(filename,"r+b");
	if(!file)
		return errno;
	
	
	// Allocate memory for file 
	(*data) = calloc((*filesize),sizeof(unsigned char));
		
	if(!data)
		goto close_file;
		
		
	// read the boot image into memory
	size_t read_size =  fread((*data),1,(*filesize),file);
	if(  read_size != (*filesize) ){
		goto close_file;
	}
	
close_file:	
	// Close the file stream if we have one.
	if(file){
		if(fclose(file)==EOF){
			return errno;
		}
	}
	return errno;
}
int set_boot_image_content_hash(boot_image* image)
{
	SHA_CTX ctx;
    SHA_init(&ctx);
    SHA_update(&ctx, image->kernel_addr, image->kernel_size);
    SHA_update(&ctx, &image->kernel_size, sizeof(image->kernel_size));
    SHA_update(&ctx, image->ramdisk_addr, image->ramdisk_size);
    SHA_update(&ctx, &image->ramdisk_size, sizeof(image->ramdisk_size));
    SHA_update(&ctx, image->second_addr, image->second_size);
    SHA_update(&ctx, &image->second_size, sizeof(image->second_size));
    const uint8_t* sha = SHA_final(&ctx);
	memcpy(&image->id, sha, SHA_DIGEST_SIZE > sizeof(image->id) ? sizeof(image->id) : SHA_DIGEST_SIZE);
    return 0;
}


int write_boot_image_header_to_disk(const char *filename, boot_image* image){
	
	FILE * header_file = fopen(filename,"wb");
		if(header_file){
			fprintf(header_file,"kernel_size:%u"EOL"kernel_address:0x%08x"EOL"ramdisk_size:%u"EOL"ramdisk_address:0x%08x"EOL"second_size:%u"EOL"second_address:0x%08x"EOL
			"tags_address:0x%08x"EOL"page_size:%u"EOL"name:%s"EOL"cmdline:%s"EOL,
			image->kernel_size,image->kernel_phy_addr,image->ramdisk_size,image->ramdisk_phy_addr,
			image->second_size,image->second_phy_addr,image->tags_phy_addr,image->page_size,image->name,image->cmdline);
			fclose(header_file);
		}
	return 0;
}
int load_boot_image_header_from_disk(const char *filename, boot_image* image){
	
	FILE * header_file = fopen(filename,"r");
	if(header_file){
		char line[256];
		while ( fgets ( line, sizeof line, header_file ) ) {
			
			if(!memcmp("kernel_address:",line,15)){
				//fprintf(stderr,"%d %s\n",strlen(line+17),line+17);
				char *value = line+17;
				image->kernel_phy_addr = strtol(value,NULL,16 ); /* write the line */
			}
			if(!memcmp("ramdisk_address:",line,16)){
				image->ramdisk_phy_addr = strtol(line+16,NULL,16 ); /* write the line */
			}
			if(!memcmp("tags_address:",line,13)){
				image->tags_phy_addr = strtol(line+15,NULL,16 ); /* write the line */
			}
			if(!memcmp("second_address:",line,15)){
				image->second_phy_addr = strtol(line+17,NULL,16 ); /* write the line */
			}
			if(!memcmp("page_size:",line,10)){
				//fprintf(stderr,"page_size: %s\n",line+10);
				image->page_size = strtol(line+10,NULL,10 ); /* write the line */
			}
			if(!memcmp("name:",line,5)){
				memcpy(image->name,line+5,strlen(line+5));
				image->name[strlen(line+5)-1]='\0';
			}
			if(!memcmp("cmdline:",line,8)){
				memcpy(image->cmdline,line+8,strlen(line+8));
				image->cmdline[strlen(line+8)-1]='\0';
				//fprintf(stderr,"%d %d %s %s\n",strlen(line+8),strlen(image->cmdline),line+8 , image->cmdline);
				
			}
		
		}
		fclose(header_file);
	}
	
	return 0;
}
/* load_boot_image - load android boot image into memory 
 * 
 * 
 * */
int load_boot_image(const char *filename, boot_image* image){

	unsigned char* data = NULL;
	int return_value = 0;
	size_t filesize = 0;
	image->start_addr = NULL;
	// Load the file into memory
	if((return_value = load_boot_image_into_memory(filename,&data,&filesize))){
		goto cleanup_and_return;
	}
	
	// Look for the Android Boot Magic
	unsigned char * magic_offset_p = find_in_memory(data,filesize,BOOT_MAGIC, BOOT_MAGIC_SIZE );
	if(!magic_offset_p){
		return_value = ENOEXEC;
		goto cleanup_and_return;
		
	}
	
	// set the image start to be a pointer to the data buffer in memory
	image->start_addr = data;
	
	// set the image total_size equal to the filesize
	image->total_size = filesize;
	
	// Populate the AOSP boot_img_hdr struct from the magic offset
	// then we can jiggery pokery the start of the header to the image magic
	boot_img_hdr* header = (boot_img_hdr*)magic_offset_p;
	memcpy(image->magic ,header,sizeof(boot_img_hdr));
	
	// Work out the header values
	image->header_size = sizeof(boot_img_hdr);
	image->header_offset = magic_offset_p - data; 
	image->header_addr = data + image->header_offset;
	image->header_padding = get_padding(image->header_size,image->page_size);	
	
	// Work out the kernel values	
	image->kernel_offset = image->header_offset + image->page_size;
	image->kernel_addr =  data + image->kernel_offset ;
	image->kernel_padding = get_padding(image->kernel_size,image->page_size);
	
	// Work out the ramdisk values
	image->ramdisk_offset = image->kernel_offset + image->kernel_size + image->kernel_padding;
	image->ramdisk_addr = data + image->ramdisk_offset;
	image->ramdisk_padding = get_padding(image->ramdisk_size,image->page_size);
	
	// Work out the second values
	if(image->second_size > 0){
		image->second_offset = image->ramdisk_offset + image->ramdisk_size + image->ramdisk_padding;
		image->second_addr = data + image->second_offset ;
		image->second_padding = get_padding(image->second_size,image->page_size);
	}else{
		image->second_offset = -1;
		image->second_addr = NULL ;
		image->second_padding = -1;
	}
	
	
	
	
	return 0;
	
cleanup_and_return:
	if(data){
		fprintf(stderr,"Freeing Memory\n");
		image->start_addr = NULL;
		free(data);
	}
	return return_value;
}

// check_phy_addr_values - checks the boot image structure to make 
// sure phy_addr_value have been set. If not we will use the defaults
int set_boot_image_defaults(boot_image* image){
	
	memcpy(image->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE);
	
	image->kernel_phy_addr = 0x10008000;
	
	image->ramdisk_phy_addr = 0x11000000;

	image->second_phy_addr = 0x10f00000;
	
	image->tags_phy_addr = 0x10000100;
	
	image->page_size = 2048;
	
	image->header_offset = 0;
	image->header_addr = image->magic;
	
	image->header_size = sizeof(boot_img_hdr);
	image->header_padding = get_padding(image->header_size,image->page_size);
	
	image->second_size = 0;
	image->second_addr = NULL;
	image->second_padding = 0;
	
	image->ramdisk_size = 0;
	image->kernel_size = 0;
	image->kernel_offset = image->page_size;
	
	image->name[0] = '\0';
	image->cmdline[0] = '\0';
	memset(image->id,0,8*sizeof(unsigned));
	
} 

int write_boot_image(const char *filename,boot_image* image){
	
	FILE* boot_image_file_fp = fopen(filename,"w+b");
	
	boot_img_hdr hdr;
	
	memcpy(&hdr,image->header_addr,sizeof(boot_img_hdr));
	
	if(fwrite(image->header_addr,1,image->header_size,boot_image_file_fp) !=  image->header_size) goto fail;
	
	if(fwrite(padding,1,image->header_padding,boot_image_file_fp) != image->header_padding) goto fail;
	
	if(fwrite(image->kernel_addr,1,image->kernel_size,boot_image_file_fp) !=  image->kernel_size) goto fail;
	
	if(image->kernel_padding > 0 )
		if(fwrite(padding,1,image->kernel_padding,boot_image_file_fp) != image->kernel_padding) goto fail;
	
	if(fwrite(image->ramdisk_addr,1,image->ramdisk_size,boot_image_file_fp) !=  image->ramdisk_size) goto fail;
	
	if(image->ramdisk_padding > 0 )
		if(fwrite(padding,1,image->ramdisk_padding,boot_image_file_fp) != image->ramdisk_padding) goto fail;	
	
	if(image->second_size){
		if(fwrite(image->second_addr,1,image->second_size,boot_image_file_fp) !=  image->second_size) goto fail;
	
		if(image->second_padding > 0 )	
			if(fwrite(padding,1,image->second_padding,boot_image_file_fp) != image->second_padding) goto fail;
	}
	fclose(boot_image_file_fp);
	return 0;
fail:
	fclose(boot_image_file_fp);
	return errno;	
}