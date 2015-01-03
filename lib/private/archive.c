/*
 * Copyright (C) 2014 Trevor Drake
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * file : lib/private/archive.c
 *
 */
#define  TRACE_TAG   TRACE_PRIVATE_ARCHIVE
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <private/api.h>


#include <archive.h>
#include <archive_entry.h>

#define ARCHIVE_ENTRY_DEFAULT_SIZE_32MB ( (1024 * 1024 ) *32 )
__LIBBOOTIMAGE_PRIVATE_API__ unsigned int archive_gzip_get_uncompressed_size(char* data,off_t data_size)
{

    if (data == NULL ){
        ENULLARG("data");
        return -1;
    }
    if (data_size == 0 ){
        ENULLARG("data");
        return -1;
    }
    int* last_four_bytes = data+(data_size-4);
    int last_four_bytes_int = *last_four_bytes;
    D("last_four_bytes_int=%d",last_four_bytes_int);

    return last_four_bytes_int;

}
__LIBBOOTIMAGE_PRIVATE_API__ unsigned int archive_list_memory_gzip_tar(char* archive_data,off_t archive_size)
{

    if (archive_data == NULL ){
        ENULLARG("data");
        return -1;
    }
    if (archive_size == 0 ){
        ENULLARG("data");
        return -1;
    }

    struct archive *a = NULL ;
    if ( check_archive_read_memory(&a,archive_data,archive_size) == -1 ){
        return -1 ;
    }


    return 0;

}
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_memory_factory_image_zip_file_to_memory(struct bootimage_utils* biu,struct factory_images* fi){

    if(biu == NULL){
        ENULLARG("data");
        return -1;
    }
    struct archive *a = NULL ;
    if ( check_archive_read_memory(&a,biu->compressed_data,biu->stat.st_size) == -1 ){
        return -1 ;
    }

   struct archive_entry *entry = NULL;
    int break_error = 0 ;
    int break_found = 0 ;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        /* Get the entry information we need */
        char* name = archive_entry_pathname(entry) ;

        D("archive_entry_pathname=%s fi.zip_name=%s",name,fi->zip_name);
        if ( !strncmp( name , fi->zip_name ,fi->zip_name_length) ){
            switch ( archive_entry_filetype(entry) ){
                case AE_IFREG:{ // Entry File Type is a Regular File
                    uint64_t entry_size = archive_entry_size(entry);
                    D("archive_entry_size=%d",entry_size);
                    break_found = 1 ;
                    char* entry_data = calloc(entry_size,sizeof(char));
                    ssize_t real_entry_size = archive_read_data(a,entry_data,entry_size);
                    //if ( archive_extract_entry_regular_file(a,entry,output_file_name) == -1 ) {
                    //    break_error = 1;
                    //}

                }
                default:
                    break ;
            }
        }
        if ( (break_found == 1)|| (break_error == 1) ){
            break ;
        }
    }
    archive_read_free(a);
    if ( break_error == 1 ) {
        return -1 ;
    }
    return 0;


}
__LIBBOOTIMAGE_PRIVATE_API__  static int archive_extract_entry_regular_file( struct archive *a, struct archive_entry *entry,char* output_file_name)
{
    if(entry == NULL){
         D("ERROR:entry argument is null");
        return -1 ;
    }
    if(output_file_name == NULL ) {
        D("INFO:using entry name");
        output_file_name = archive_entry_pathname(entry);
        if( output_file_name == NULL ){
            D("ERROR:archive_entry_pathname failed");
            return -1;
        }
    }

    uint64_t entry_size = archive_entry_size(entry);
    if(entry_size == 0 ){
        /* we could not get the entry size from archive entry
           so we will use the default size of 32MB instead */
        entry_size = ARCHIVE_ENTRY_DEFAULT_SIZE_32MB;
        D("WARNING:Using Default Entry Size");
    }

    D("archive_entry_size=%lu",entry_size);

    /* Get the output directory name */
    char* output_dir_name = utils_dirname(output_file_name) ;
    D("output_dir_name=%s",output_dir_name);

    if(output_dir_name != NULL ) {
        /* Create the output directory if needed */
        if ( utils_mkdir_and_parents_umask(output_dir_name,0755,0) == -1 ){
            D("ERROR:Failed to create output directory %s",output_dir_name);
            free(output_dir_name);
            return -1 ;
        }
        free(output_dir_name);
    }
    /* Allocate the memory for the uncompressed entry data */
    unsigned char* entry_data = calloc(entry_size,sizeof(unsigned char));
    if ( entry_data == NULL ){
        D("ERROR:Failed to allocate memory for entry data");
        return -1 ;
    }

    mode_t mode = archive_entry_mode(entry) ;
    int fd =  open(output_file_name, O_CREAT | O_TRUNC | O_WRONLY, mode);

    D("fd=%d",fd);
    if ( fd == -1 ){
        D("ERROR:Failed to open output file %s",output_file_name);
        free(entry_data);
        return -1 ;
    }
    ssize_t real_entry_size = archive_read_data(a,entry_data,entry_size);
    if (  real_entry_size == ARCHIVE_FATAL ){
        /* there was a fatal error; the archive should be closed immediately */
        D("archive_read_data=ARCHIVE_FATAL");
        free(entry_data);
        close(fd);
        return -1 ;
    }
    if ( utils_write_all(fd,entry_data,real_entry_size) == -1 ){
        D("ERROR:Failed to write entry data real_entry_size:%lu",real_entry_size);
        free(entry_data);
        close(fd);
        return -1 ;
    }
    close(fd);
    free(entry_data);
    return 0 ;



}

__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_memory_file( char* archive_data , uint64_t archive_size, char* entry_name, char* output_file_name)
{



    if ( check_output_name(output_file_name)  == -1 ) {
		return -1 ;
	}
    int entry_name_len = check_output_name(entry_name) ;
    if ( entry_name_len == -1 ) {
		return -1 ;
	}


    struct archive *a = NULL ;
    if ( check_archive_read_memory(&a,archive_data,archive_size) == -1 ){
        return -1 ;
    }
    D("a=%p\n",a);

    struct archive_entry *entry = NULL;
    int break_error = 0 ;
    int break_found = 0 ;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        /* Get the entry information we need */
        char* name = archive_entry_pathname(entry) ;
        D("archive_entry_pathname=%s",name);
        if ( !strncmp( name , entry_name,entry_name_len) ){
            switch ( archive_entry_filetype(entry) ){
                case AE_IFREG:{ /* Entry File Type is a Regular File */
                    break_found = 1 ;
                    if ( archive_extract_entry_regular_file(a,entry,output_file_name) == -1 ) {
                        break_error = 1;
                    }

                }
                default:
                    break ;
            }
        }
        if ( (break_found == 1)|| (break_error == 1) ){
            break ;
        }
    }
    archive_read_free(a);
    if ( break_error == 1 ) {
        return -1 ;
    }
    return 0;


}

__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all_memory_directory( char* archive_data , uint64_t archive_size, char* output_dir_name)
{

    if ( check_output_name(output_dir_name)  == -1 ) {
		return -1 ;
	}
    struct archive *a = NULL ;
    if ( check_archive_read_memory(&a,archive_data,archive_size) == -1 ){
        return -1 ;
    }

    D("archive_compression_name=%s",archive_compression_name(a));
    if ( archive_extract_all(a,output_dir_name) == -1 ){
        int en = errno ;
        archive_read_free(a);
        errno = en ;
        return -1 ;
    }
    archive_read_free(a);
    return 0;
}

__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all(struct archive *a,char* output_dir_name)
{
    if ( check_output_name(output_dir_name)  == -1 ) {
		return -1 ;
	}
    if ( utils_mkdir_and_parents_umask(output_dir_name,0755,0) == -1 ){
        D("ERROR:Failed to create output directory %s",output_dir_name);
        return -1 ;
    }
    char* cwd = calloc(PATH_MAX,sizeof(char));
    if( cwd == NULL ) {
        D("ERROR:Failed to allocate memory for cwd");
        return -1 ;
    }
    if ( getcwd(cwd, PATH_MAX) == NULL ){
        D("ERROR:getcwd failed");
        free(cwd);
        return -1;
    }
    if ( chdir(output_dir_name) == -1 ){
        D("ERROR:chdir failed errno=%d",errno);
        free(cwd);
        return -1;
    }

    struct archive_entry *entry = NULL;

        while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
            switch ( archive_entry_filetype(entry) ){
                case AE_IFDIR:{ /* Entry File Type is a Directory */
                    mode_t mode = archive_entry_mode(entry) ;
                     char* name = archive_entry_pathname(entry) ;
                    utils_mkdir_and_parents(name,mode);
                    archive_read_data_skip(a);
                    break;
                }
                case AE_IFLNK:{ /* Entry File Type is a Symbolic Link */
                    char* name = archive_entry_pathname(entry) ;
#ifndef _WIN32
                    symlink(archive_entry_symlink(entry), name);
#endif
                    archive_read_data_skip(a);
                    break;
                }
                case AE_IFREG:{ /* Entry File Type is a Regular File */
                    archive_extract_entry_regular_file(a,entry,NULL);
                    break;

                }
                default:{ /* Entry File Type is a something else and unsupported */

                    archive_read_data_skip(a);
                    break;
                }
            }
        }
    if ( chdir(cwd) == -1 ){
        D("ERROR:chdir cwd failed errno=%d",errno);
        free(cwd);
        return -1;
    }
    free(cwd);
    return 0 ;
}
