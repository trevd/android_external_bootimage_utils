/*
 * ramdisk.c
 * 
 * Copyright 2013 Trevor Drake <trevd1234@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
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
#include <kernel.h>

#include <private/android_filesystem_config.h>



#define RECOVERY_FILE_NAME "sbin/recovery"

#define RECOVERY_MAGIC_CLOCKWORK "ClockworkMod Recovery v"
#define RECOVERY_MAGIC_SIZE_CLOCKWORK 23
#define RECOVERY_MAGIC_STOCK "Android system recovery "
#define RECOVERY_MAGIC_SIZE_STOCK 24
#define RECOVERY_MAGIC_COT "Cannibal Open Touch v"
#define RECOVERY_MAGIC_SIZE_COT 21
#define RECOVERY_MAGIC_TWRP "Starting TWRP \%s on \%s"
#define RECOVERY_MAGIC_SIZE_TWRP 22
#define RECOVERY_MAGIC_CWM "CWM-based Recovery v"
#define RECOVERY_MAGIC_SIZE_CWM 20
#define RECOVERY_MAGIC_TEAMWIN "Team Win Recovery Project v%s"
#define RECOVERY_MAGIC_SIZE_TEAMWIN 29
#define RECOVERY_MAGIC_4EXT "4EXT"
#define RECOVERY_MAGIC_SIZE_4EXT 4
#define RECOVERY_MAGIC_4EXT_VERSION "\0\"v"
#define RECOVERY_MAGIC_SIZE_4EXT_VERSION 3

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

ramdisk_image* get_initialized_ramdisk_image(){
    
    ramdisk_image* image = calloc(1,sizeof(ramdisk_image));
    init_ramdisk_image(image);
    return image;
    
}

unsigned init_ramdisk_image(ramdisk_image* image){
    
    image->compression_type = RAMDISK_COMPRESSION_UNKNOWN ;
    image->start_addr = NULL ;
    image->size = 0 ;
    image->type = RAMDISK_TYPE_UNKNOWN;
    image->recovery_brand = RECOVERY_BRAND_UNKNOWN;
    image->recovery_version = NULL;
    image->recovery_version_size = 0 ;
        
    image->entry_count = 0 ; 
    image->entries = NULL ;
    return 0;
}

int get_recovery_version_number(ramdisk_image* image, unsigned char * version_number_offset){
    
    // check for a digit and a dot -- A bit of future proofing in case the version gets into double figures
    if((isdigit(version_number_offset[0]) ||  version_number_offset[0]=='<')  && ( isdigit(version_number_offset[1]) || version_number_offset[1]=='.' )){
            
    // do a sanity check on the version number length... if it is something silly i.e > 20 then we will disregard it
    
    char * version_number = (char*)version_number_offset;
    
    int version_len = strlen(version_number);
    D("version_len %d version_number_offset[%d]='%c'\n",version_len,version_len-1,version_number_offset[version_len-1]);
    // remove unwanted chars from the end of the struing
    if(version_number_offset[version_len-1]=='\"'){
        //D("Found a quote\n");
        version_len -=1 ; 
    }
    
    if(version_len < 20 ){
        image->recovery_version = version_number;
        image->recovery_version_size = version_len ;
        D("Recovery image recovery_name_offset=%p recovery_version=%s version_len=%d\n",image->recovery_version ,image->recovery_version,version_len);
        
    }else{
        image->recovery_version_size = 0;
        image->recovery_version = NULL ; 
        D("Version Length Too Long recovery_name_offset=%p recovery_version=%s version_len=%d\n",image->recovery_version ,image->recovery_version,version_len);
    }
    }else{
    D("Version Number not found at expected position %p\n",version_number_offset);
    image->recovery_version = NULL ; 
    image->recovery_version_size = 0;
    }
    return 0; 
}

// get_ramdisk_type - heuristically work out the type of ramdisk
// the following checks are carried out
// does a recovery binary exist. 
unsigned get_ramdisk_type(ramdisk_image* image){
    

    
    image->type = RAMDISK_TYPE_NORMAL ;
    image->recovery_brand = RECOVERY_BRAND_NONE ;
    image->recovery_version = NULL ;
    
    unsigned i =0;
    for(i = 0; i < image->entry_count ; i++) {
    if(!strlcmp((char*)image->entries[i]->name_addr,RECOVERY_FILE_NAME)){
        
        image->type = RAMDISK_TYPE_RECOVERY ;
        image->recovery_brand = RECOVERY_BRAND_UNKNOWN ;
        image->recovery_version = NULL ;
        D("recovery image found at %d\n",i);
        unsigned char * recovery_name_offset = NULL; 
        if((recovery_name_offset = find_in_memory(image->entries[i]->data_addr,image->entries[i]->data_size,RECOVERY_MAGIC_TWRP,RECOVERY_MAGIC_SIZE_TWRP))){
        
        
        // Found A TWRP Recovery. Advanced our offset along as the next string should be the version number
        // we need to advanced size +1 for twrp because the version should be the next string after the NULL Terminator
        recovery_name_offset += (RECOVERY_MAGIC_SIZE_TWRP+1);
        // set the recovery brand 
        image->recovery_brand = RECOVERY_BRAND_TWRP ;
        
        // get the version number
        get_recovery_version_number(image,recovery_name_offset);
        
        
        }else if((recovery_name_offset = find_in_memory(image->entries[i]->data_addr,image->entries[i]->data_size,RECOVERY_MAGIC_TEAMWIN,RECOVERY_MAGIC_SIZE_TEAMWIN))){
        
        // Found An Older TWRP Recovery. Advanced our offset along as the end of this string should be the version number
        recovery_name_offset += RECOVERY_MAGIC_SIZE_TEAMWIN;
        // set the recovery brand to TWRP
        image->recovery_brand = RECOVERY_BRAND_TWRP ;
        
        // get the version number
        get_recovery_version_number(image,recovery_name_offset);
        
        }else if((recovery_name_offset = find_in_memory(image->entries[i]->data_addr,image->entries[i]->data_size,RECOVERY_MAGIC_CLOCKWORK,RECOVERY_MAGIC_SIZE_CLOCKWORK))){
        D("Clockworkmod Recovery image recovery_name_offset=%s\n",recovery_name_offset);
        
        // Found An Official ClockworkMod Recovery. Advanced our offset along as the end of this string should be the version number
        recovery_name_offset += RECOVERY_MAGIC_SIZE_CLOCKWORK;
        
        // set the recovery brand to Clockworkmod
        image->recovery_brand = RECOVERY_BRAND_CLOCKWORK ;
        
        // get the version number
        get_recovery_version_number(image,recovery_name_offset);
        
        
        
        }else if((recovery_name_offset = find_in_memory(image->entries[i]->data_addr,image->entries[i]->data_size,RECOVERY_MAGIC_COT,RECOVERY_MAGIC_SIZE_COT))){
        D("COT Recovery image recovery_name_offset=%s\n",recovery_name_offset);
        
        // Found A COT ( Cannibal Open Touch ) Recovery. Advanced our offset along as the end of this string should be the version number
        recovery_name_offset += RECOVERY_MAGIC_SIZE_COT;
        
        // set the recovery brand to COT
        image->recovery_brand = RECOVERY_BRAND_COT ;
        
        // get the version number
        get_recovery_version_number(image,recovery_name_offset);
        
        }else if((recovery_name_offset = find_in_memory(image->entries[i]->data_addr,image->entries[i]->data_size,RECOVERY_MAGIC_STOCK,RECOVERY_MAGIC_SIZE_STOCK))){
        D("Stock Recovery image recovery_name_offset=%s\n",recovery_name_offset);
        
        // Found A Stock Recovery. Advanced our offset along as the end of this string should be the version number
        recovery_name_offset += RECOVERY_MAGIC_SIZE_STOCK;
        
        // set the recovery brand to Stock
        image->recovery_brand = RECOVERY_BRAND_STOCK ;
        
        // get the version number
        get_recovery_version_number(image,recovery_name_offset);
        }
        else if((recovery_name_offset = find_in_memory(image->entries[i]->data_addr,image->entries[i]->data_size,RECOVERY_MAGIC_CWM,RECOVERY_MAGIC_SIZE_CWM))){
        D("CWM-based Recovery image recovery_name_offset=%s\n",recovery_name_offset);
        
        // Found A CWM-based Recovery. Advanced our offset along as the end of this string should be the version number
        recovery_name_offset += RECOVERY_MAGIC_SIZE_CWM;
        
        // set the recovery brand to Stock
        image->recovery_brand = RECOVERY_BRAND_CWM ;
        
        // get the version number
        get_recovery_version_number(image,recovery_name_offset);
        }
        else if((recovery_name_offset = find_in_memory(image->entries[i]->data_addr,image->entries[i]->data_size,RECOVERY_MAGIC_4EXT,RECOVERY_MAGIC_SIZE_4EXT))){
        D("4EXT Recovery image recovery_name_offset=%s %p\n",recovery_name_offset,recovery_name_offset);
        
        // Found A 4EXT Recovery. look for the version number string
        recovery_name_offset = find_in_memory(image->entries[i]->data_addr,image->entries[i]->data_size,RECOVERY_MAGIC_4EXT_VERSION,RECOVERY_MAGIC_SIZE_4EXT_VERSION);
        if(recovery_name_offset) recovery_name_offset += RECOVERY_MAGIC_SIZE_4EXT_VERSION;
        
        D("4EXT Version offset recovery_name_offset=%s %p\n",recovery_name_offset,recovery_name_offset);
        
        // set the recovery brand to Stock
        image->recovery_brand = RECOVERY_BRAND_4EXT ;
        
        // get the version number
        get_recovery_version_number(image,recovery_name_offset);
        }
        break;
        
    }
    }
    return 0 ;
}

unsigned carve_out_entry_space(ramdisk_image* image){

    errno = 0;
    if(!image){
    errno = ENOEXEC;
    return errno;
    }
    if(!image->entry_count){
    errno = EINVAL ;
    return errno;
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
    return errno;
    }
    if(!image->entry_count){
    errno = EINVAL ;
    return errno;
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
        image->entries[counter]->data_addr = image->entries[counter]->name_addr +   image->entries[counter]->name_size + image->entries[counter]->name_padding ;
        image->entries[counter]->data_size = get_long_from_hex_field(header->c_filesize);
        image->entries[counter]->data_padding = ((4 - ((image->entries[counter]->data_size ) % 4)) % 4);
        image->entries[counter]->next_addr = image->entries[counter]->data_addr + (image->entries[counter]->data_size + image->entries[counter]->data_padding);
        //D("name=%s data_size=%u padding=%u\n",image->entries[counter]->name_addr ,image->entries[counter]->data_size,image->entries[counter]->data_padding);
        
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

unsigned load_ramdisk_image_from_archive_file(const char *filename, ramdisk_image* image){
     
    errno = 0;
    unsigned data_size = 0;
    unsigned char* ramdisk_addr = read_item_from_disk(filename,&data_size);
    if(!ramdisk_addr){
    fprintf(stderr,"Error %d\n",errno);
    
    return errno;

    }
    fprintf(stderr,"ramdisk_addr %p %u\n",ramdisk_addr,data_size);
    unsigned return_value = load_ramdisk_image_from_archive_memory(ramdisk_addr,data_size,image);

    return  return_value; 
}
unsigned load_ramdisk_image_from_cpio_file(const char *filename, ramdisk_image* image){
    
    errno = 0;
    unsigned data_size = 0;
    unsigned char* ramdisk_addr = read_item_from_disk(filename,&data_size);
    if(!ramdisk_addr){
    fprintf(stderr,"Error %d\n",errno);
    return errno;

    }
    fprintf(stderr,"ramdisk_addr %p %u\n",ramdisk_addr,data_size);
    int return_value = load_ramdisk_image_from_cpio_memory(ramdisk_addr,data_size,image);

    return  return_value; 
}
unsigned load_ramdisk_image_from_cpio_memory(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image ){
    
    errno = 0 ; 
    if(!ramdisk_addr || !ramdisk_size ){
    errno = EINVAL ;
    if(image) image->start_addr = NULL ; 
    }else if(!image)
    errno = ENOMEM ;
    
    if(errno) return errno ;
    
    
    D("ramdisk_size %u\n", ramdisk_size);
    // now we have uncompressed data check we have a cpio file look for the magic
    if(memcmp(ramdisk_addr,CPIO_HEADER_MAGIC,CPIO_HEADER_MAGIC_SIZE)){
    D("CPIO_HEADER_MAGIC not found\n");
    goto error_cpio_magic;
    }
    
    // and also look for the trailer
    if(!find_in_memory(ramdisk_addr,ramdisk_size,CPIO_TRAILER_MAGIC,CPIO_TRAILER_MAGIC_SIZE)){
    D("CPIO_TRAILER_MAGIC not found\n");
    goto error_cpio_magic;
    }
    
    image->size = ramdisk_size;
    image->start_addr = ramdisk_addr ;
    
    image->entry_count = count_ramdisk_archive_entries(image);
    D("image->entry_count %u\n", image->entry_count);
    
    if(!image->entry_count){
    errno = EINVAL;
    return EINVAL;
    }
    
    carve_out_entry_space(image);
    carve_out_entry_space(image);
    
    populate_ramdisk_entries(image);
    
    get_ramdisk_type(image);
    
    return 0;

error_cpio_magic:
    image->start_addr = NULL;
    errno = ENOEXEC;
    return ENOEXEC;
}

// get_archive_compression_type_and_offset - search the memory from ramdisk_addr for known compression magic 
// numbers. returns the offset to the start of the archive or NULL if no known archive is found 
// image->compression_type is also updated
unsigned char* get_archive_compression_type_and_offset(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image){
    
    // First we need to make sure we have an actual ramdisk and not some thing else
    // which can contain archives, such as a kernel image
    unsigned char * kernel_magic_offset_p = find_in_memory(ramdisk_addr,ramdisk_size,KERNEL_ZIMAGE_MAGIC, KERNEL_ZIMAGE_MAGIC_SIZE );
    if(kernel_magic_offset_p){
    image->compression_type = RAMDISK_COMPRESSION_UNKNOWN ;
    return NULL ;
    }
    
    
    D("ramdisk_addr=%p ramdisk_size=%u\n", ramdisk_addr,ramdisk_size);
    unsigned char * archive_magic_offset_p = find_in_memory(ramdisk_addr,ramdisk_size,GZIP_DEFLATE_MAGIC, GZIP_DEFLATE_MAGIC_SIZE );
    if(archive_magic_offset_p){
        D("compression_type=GZIP\n");
        image->compression_type = RAMDISK_COMPRESSION_GZIP ;
        return archive_magic_offset_p;
    }
    
    archive_magic_offset_p = find_in_memory(ramdisk_addr,ramdisk_size,LZOP_MAGIC, LZOP_MAGIC_SIZE);
    if(archive_magic_offset_p){
        D("compression_type=LZO\n");
        image->compression_type = RAMDISK_COMPRESSION_LZO ;
        return archive_magic_offset_p;
    }
    archive_magic_offset_p = find_in_memory(ramdisk_addr,ramdisk_size,XZ_MAGIC, XZ_MAGIC_SIZE);
    if(archive_magic_offset_p){
        D("compression_type=XZ\n");
        image->compression_type = RAMDISK_COMPRESSION_XZ ;
        return archive_magic_offset_p;
    }
    D("compression_type=NOT FOUND\n");
    return NULL;
}

unsigned load_ramdisk_image_from_archive_memory(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image ){

    D("ramdisk_addr=%p ramdisk_size=%u\n", ramdisk_addr,ramdisk_size);
   
    errno = 0;
    // look for a gzip magic to make sure the ramdisk is the correct type
    unsigned char * archive_magic_offset_p = get_archive_compression_type_and_offset(ramdisk_addr,ramdisk_size,image);
    if(!archive_magic_offset_p){
        errno = ENOEXEC ;
        return ENOEXEC;
    }
       
    unsigned char *uncompressed_ramdisk_data = calloc(MAX_RAMDISK_SIZE,sizeof(unsigned char)) ;
    unsigned uncompressed_ramdisk_size = 0;
    
    switch(image->compression_type){
        case RAMDISK_COMPRESSION_GZIP:
           uncompressed_ramdisk_size = uncompress_gzip_memory(archive_magic_offset_p,ramdisk_size,uncompressed_ramdisk_data,MAX_RAMDISK_SIZE);
           break ;
        case RAMDISK_COMPRESSION_LZO:{
            uncompressed_ramdisk_size = uncompress_lzo_memory(archive_magic_offset_p,ramdisk_size,uncompressed_ramdisk_data,MAX_RAMDISK_SIZE);
            break;
        }
        case RAMDISK_COMPRESSION_XZ:{
            uncompressed_ramdisk_size = uncompress_xz_memory(archive_magic_offset_p,ramdisk_size,uncompressed_ramdisk_data,MAX_RAMDISK_SIZE);
            break;
        }
        
        default:
            break;
    }
    
    if(!uncompressed_ramdisk_size){
        D("uncompressed_ramdisk_size error\n");
        goto error_uncompressed;
    }
    
    if(load_ramdisk_image_from_cpio_memory(uncompressed_ramdisk_data,uncompressed_ramdisk_size,image)){
        D("load_ramdisk_image_from_cpio_memory error\n");
        goto error_uncompressed;
    }
    
    D("image->entry_count %u\n", image->entry_count);
    return 0;
    
error_uncompressed:
    image->start_addr = NULL ;
    free(uncompressed_ramdisk_data);
    errno = EINVAL;
    D("error_uncompressed %s\n", strerror(errno));
    return  EINVAL;

}
unsigned save_ramdisk_entries_to_disk(ramdisk_image* image,char *directory_name){

    D("called with image=%p directory_name=%s\n", image,directory_name);
    char cwd[PATH_MAX];
    errno = 0;
    if(directory_name){
        if( mkdir_and_parents(directory_name,0777) == -1 ){
            D("failed mkdir_and_parents %s\n", directory_name);
            return errno;        
        }
        
        getcwd(cwd,PATH_MAX);
        chdir((char *)directory_name);
    }
    
    unsigned i = 0;
    D("ramdisk_entry_count=%u\n", image->entry_count);
    for(i = 0 ; i < image->entry_count ; i++){
    
    
    
    int is_dir = S_ISDIR(image->entries[i]->mode);
    
    write_item_to_disk_extended(image->entries[i]->data_addr,
                    image->entries[i]->data_size,
                    image->entries[i]->mode, 
                    (char*)image->entries[i]->name_addr,
                    image->entries[i]->name_size);
    
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
            
            // Skip self and previous directory entries
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
        
        
        total_size += 0;
        
        names[total]=strdup(path+root_len);
        D("names[%u]:%s\n",total,names[total]);
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
          namesize  ,
           0,filename
           );     
    D("namealign:%s %lu %p %p\n",filename,namealign,output_header,output_header);
    output_header+=(sizeof(cpio_newc_header)+namesize);
    strncat((char*)output_header,"\0\0\0\0",namealign);
    //fprintf(stderr,"Out:%p\n", output_header);
    output_header+=namealign;
    return output_header;
}
static unsigned char* append_file_contents_to_stream(struct stat s,char *filename, unsigned char* output_header){
    
    
    if(S_ISDIR(s.st_mode)){
    }else {
    unsigned long filealign = ((4 - ((s.st_size) % 4)) % 4);
    if(S_ISREG(s.st_mode)){
        D("open reg %s\n",filename);
        FILE* fp = fopen(filename,"r+b");
        fread(output_header,s.st_size,1,fp);
        output_header+=s.st_size;
        fclose(fp);
    }else if(S_ISLNK(s.st_mode)){
        
        readlink(filename,(char*)output_header,PATH_MAX);
        
        output_header+=s.st_size;
    }
    output_header+=filealign;
    }
    
    return output_header; 
    
}

unsigned char *pack_ramdisk_directory(char* directory_name, unsigned *cpio_size){
    
    
    
    struct stat sb;
    errno = 0 ;
    if(lstat(directory_name,&sb) == -1){
        errno = EBADF;
        return NULL;
    }
    
    // STEP 1: Information Gathering.
    // Find out how many files we as dealing with
    size_t filesystem_entries =0 ;
    count_filesystem_entries(directory_name,0);
    D("filesystem_entries %u %u\n",filesystem_entries,total);
    filesystem_entries = total;
    unsigned i; 
    
    // allocate the memory required for the filenames list
    names = calloc(filesystem_entries, sizeof(names));
    for(i = 0; i < filesystem_entries; i++) {
        names[i] = (char *)calloc(PATH_MAX,sizeof(char));   
        if (names[i] == NULL) {
            D("Memory cannot be allocated to arr[]");
        }
     D("i: %d \n",i);
    }
    total = 0;
    get_filesystem_entry_names(directory_name,0);
    qsort(names, filesystem_entries, sizeof(char*), qsort_comparer);

    errno = 0;
    char cwd[PATH_MAX];
    getcwd(cwd,PATH_MAX);
    chdir(directory_name);
    unsigned char* cpio_data = calloc(MAX_RAMDISK_SIZE,sizeof(char));
    unsigned char* nextbyte = &cpio_data[0];
    for(i = 0; i < filesystem_entries; i++){
    
        struct stat sb;
        if(lstat(names[i],&sb) == -1)
            continue;
        
        D("nextbyte %p errno=%d\n",nextbyte,errno);
        nextbyte = append_cpio_header_to_stream(sb,names[i],nextbyte);
        nextbyte = append_file_contents_to_stream(sb,names[i],nextbyte);
    
    }
    struct stat s ; memset(&s, 0, sizeof(s));
    D("errno:%d\n",errno); 
    nextbyte =append_cpio_header_to_stream(s,CPIO_TRAILER_MAGIC,nextbyte);
    D("errno:%d chrdir(%s)\n",errno,cwd); 
    chdir(cwd) ;
    
    (*cpio_size) = nextbyte - &cpio_data[0] ;
    // align the file size to the next 256 boundary
    while((*cpio_size) & 0xff) 
        (*cpio_size)++;
    
    D("cpio_size %u %p\n",cpio_size,nextbyte);
    
    return cpio_data;
    
    
    
}
unsigned print_ramdisk_info(ramdisk_image* rimage){
    
    //fprintf(stderr,"\nramdisk_image struct values:\n");
    D("start_addr=%p\n",rimage->start_addr);
    D("entries=%p\n",rimage->entries);
    fprintf(stderr,"  uncompressed size :%u\n",rimage->size);
    fprintf(stderr,"  compression type  :%s\n",str_ramdisk_compression(rimage->compression_type));
    fprintf(stderr,"  image type        :%s\n",str_ramdisk_type(rimage->type));
    if(rimage->type == RAMDISK_TYPE_RECOVERY){
    fprintf(stderr,"  recovery brand    :%s\n",str_recovery_brand(rimage->recovery_brand));
    if(rimage->recovery_version && rimage->recovery_brand != RECOVERY_BRAND_UNKNOWN){
        fprintf(stderr,"  recovery version  :%.*s",rimage->recovery_version_size,rimage->recovery_version);
        // Do we need a new line
        if(rimage->recovery_version[rimage->recovery_version_size-1]!='\n'){
        fprintf(stderr,"\n");
        }
        D("rimage->recovery_version_size=%d rimage->recovery_version[%d]='%d'\n",
            rimage->recovery_version_size,rimage->recovery_version_size-1,rimage->recovery_version[rimage->recovery_version_size-1]);
    }
    }
    fprintf(stderr,"  entry_count       :%u\n",rimage->entry_count);
    
    
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
char *str_recovery_brand(int ramdisk_brand){
    
    switch(ramdisk_brand){
    case RECOVERY_BRAND_UNKNOWN: return "unknown"; 
    case RECOVERY_BRAND_NONE: return "none";
    case RECOVERY_BRAND_STOCK: return "Android system recovery";
    case RECOVERY_BRAND_CLOCKWORK: return "ClockworkMod Recovery";
    case RECOVERY_BRAND_CWM: return "CWM-Based Recovery";
    case RECOVERY_BRAND_COT: return "Cannibal Open Touch Recovery";
    case RECOVERY_BRAND_TWRP: return "Team Win Recovery Project";
    case RECOVERY_BRAND_4EXT: return "4EXT Touch Recovery";
    default: return "unknown";
    }
    return "unknown";
}
char *str_ramdisk_compression(int compression_type){
  
  switch(compression_type){
    case RAMDISK_COMPRESSION_GZIP: return "gzip" ; 
    case RAMDISK_COMPRESSION_LZMA: return "lzma" ; 
    case RAMDISK_COMPRESSION_LZO: return "lzo" ; 
    case RAMDISK_COMPRESSION_BZIP2: return "bz2";
    case RAMDISK_COMPRESSION_XZ: return "xz";
    default: return "none";
    }
    return "unknown";
    
}
unsigned int_ramdisk_compression(char * compression_type){
    
    D("compression_type=%s\n",compression_type);
    unsigned compression_index = RAMDISK_COMPRESSION_UNKNOWN;
    if(!compression_type){
    return compression_index;
    }
    D("compression_type[0]=%c\n",compression_type[0]);
    switch(compression_type[0]){
    case 'g':    compression_index = RAMDISK_COMPRESSION_GZIP;  break;      
    case 'l':{
        D("compression_type[2]=%c\n",compression_type[2]);
        if(compression_type[1]=='z' && compression_type[2]){
            if(compression_type[2]=='m')
                compression_index = RAMDISK_COMPRESSION_LZMA;
            if(compression_type[2]=='o')
                compression_index = RAMDISK_COMPRESSION_LZO;
            if(compression_type[2]=='4')
                compression_index = RAMDISK_COMPRESSION_LZ4;
        }
        break;      
    }    
    case 'b':    compression_index = RAMDISK_COMPRESSION_BZIP2;  break;
    case 'x':    compression_index = RAMDISK_COMPRESSION_XZ;    break;
    default: break;
    }
    D("compression_index=%d\n",compression_index);
    return compression_index;
    
}

unsigned update_ramdisk_entry_cpio_newc_header_info(ramdisk_entry* entry){
    
    char filesize_string[8];
    
    sprintf(filesize_string,"%08x",entry->data_size);

    D("old data_padding:%u\n",entry->data_padding);
    entry->data_padding = ((4 - ((entry->data_size ) % 4)) % 4);
    
    
    cpio_newc_header* head = (cpio_newc_header*)entry->start_addr;
    
    D("new data_padding:%u\n",entry->data_padding);
    D("old filesize:%.*s new filesize:%.*s\n",8,head->c_filesize,8,filesize_string);
    memmove(head->c_filesize,filesize_string,8);
    return 0;
}
unsigned char* pack_noncontiguous_ramdisk_entries(ramdisk_image* rimage){
    
    unsigned i = 0;
    unsigned char* data = calloc(rimage->size , sizeof(unsigned char));
    
    unsigned char* data_start = data;
    unsigned cpio_size = 0;
    D("cpio_size:%u %p %p\n",cpio_size,data ,data_start);
    for ( i = 0 ; i < rimage->entry_count ; i++ ){
    ramdisk_entry* entry = rimage->entries[i];  
    //D("%d %.*s\n",sizeof(cpio_newc_header),sizeof(cpio_newc_header),entry->start_addr);   
    //D("name_size:%u name:%.*s name_pad:%u data_size:%u data_pad:%u\n",entry->name_size,entry->name_size,entry->name_addr,
    //              entry->name_padding,
    //              entry->data_size,
    //              entry->data_padding);   
    //
    memcpy(data,entry->start_addr,sizeof(cpio_newc_header));
    
    cpio_size += sizeof(cpio_newc_header);
    data += sizeof(cpio_newc_header);
    
    memcpy(data,entry->name_addr,entry->name_size);
    
    cpio_size += entry->name_size;
    data += entry->name_size;
    
    memcpy(data,"\0\0\0\0",entry->name_padding);
    cpio_size += entry->name_padding;
    data += entry->name_padding;
    
    memcpy(data,entry->data_addr,entry->data_size);
    cpio_size += entry->data_size;
    data += entry->data_size;
    
    memcpy(data,"\0\0\0\0",entry->data_padding);
    cpio_size += entry->data_padding;
    data += entry->data_padding;
    
    
    
    
    } 
    D("cpio_size:%u %p %p\n",cpio_size,data ,data_start);
    // align the file size to the next 256 boundary
    while(cpio_size & 0xff)
       cpio_size++;
        
   
    
    
    return data_start ;
}
