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

struct bootimage_utils_archive {
    struct archive* archive;
    struct archive_entry* archive_entry;
};

struct bootimage_utils_archive_entry {
    char* name ;
    int should_extract ;
    char* data ;

};
struct bootimage_utils_archive_entries {

    struct archive* archive;
    struct bootimage_utils_archive_entry** archive_entries;
};


__LIBBOOTIMAGE_PRIVATE_API__ static struct archive*  __archive_read_init()
{
	struct archive* a = archive_read_new();
    if( a == NULL ){
        /* Failed to initialize libarchive reading */
        errno = EBIARCHIVEREAD;
        return NULL ;
    }
	/*  */
	if ( archive_read_support_filter_all(a) == ARCHIVE_WARN ){
		errno = EBIARCHIVEREADFILTER  ;
        archive_read_free(a);
        return NULL ;
    }

    if ( archive_read_support_format_all(a) == ARCHIVE_FATAL ){
		errno = EBIARCHIVEREADFORMAT ;
        archive_read_free(a);
        return NULL ;
    }
    if ( archive_read_support_format_raw(a) == ARCHIVE_FATAL ){
        archive_read_free(a);
		errno = EBIARCHIVEREADFORMATRAW  ;
        return NULL;
    }
    errno = EBIOK;

    return a;


}

__LIBBOOTIMAGE_PRIVATE_API__ static struct archive* __archive_read_memory(char* data , ssize_t data_size)
{
	struct archive* a = __archive_read_init();
	if ( a == NULL ){
		return NULL;
	}
	int r = archive_read_open_memory(a, data,data_size);
	if (r != ARCHIVE_OK){
		archive_error_string(a);
		D("r=%d %s",r,archive_error_string(a));
        archive_read_free(a);
		return NULL;
    }
	return a;
}

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
__LIBBOOTIMAGE_PRIVATE_API__ unsigned int archive_list_entries(char* archive_data,off_t archive_size,char** entries)
{

    if (archive_data == NULL ){
        ENULLARG("data");
        return -1;
    }
    if (archive_size == 0 ){
        ENULLARG("data");
        return -1;
    }


    struct archive *a = __archive_read_memory(archive_data,archive_size);
    if (  a == NULL ){
        return -1 ;
    }


    return 0;

}



__LIBBOOTIMAGE_PRIVATE_API__  static char* __archive_extract_entry( struct archive *a, struct archive_entry *entry,size_t* entry_size)
{
    if(entry == NULL){
         D("ERROR:entry argument is null");
        return NULL ;
    }


    *entry_size = archive_entry_size(entry);
    if(*entry_size == 0 ){
        /* we could not get the entry size from archive entry
           so we will use the default size of 32MB instead */
        *entry_size = ARCHIVE_ENTRY_DEFAULT_SIZE_32MB;
        D("WARNING:Using Default Entry Size");
    }

    D("archive_entry_size=%lu",*entry_size);


    /* Allocate the memory for the uncompressed entry data */
    char* entry_data = calloc(*entry_size,sizeof(unsigned char));
    if ( entry_data == NULL ){
        D("ERROR:Failed to allocate memory for entry data");
        return NULL ;
    }


    *entry_size = archive_read_data(a,entry_data,*entry_size);
    if (  *entry_size == ARCHIVE_FATAL ){
        /* there was a fatal error; the archive should be closed immediately */
        D("archive_read_data=ARCHIVE_FATAL");
        free(entry_data);
        return NULL;
    }
    return entry_data ;



}

__LIBBOOTIMAGE_PRIVATE_API__  static int __archive_extract_entry_file( struct archive *a, struct archive_entry *entry,char* output_file_name)
{
     if(output_file_name == NULL ) {
        D("INFO:using entry name");
        output_file_name = archive_entry_pathname(entry);
        if( output_file_name == NULL ){
            D("ERROR:archive_entry_pathname failed");
            return -1;
        }
    }
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
    ssize_t entry_size =0 ;
    char* entry_data = __archive_extract_entry(a,entry,&entry_size);


    mode_t mode = archive_entry_mode(entry) ;
    if ( utils_write_all(output_file_name,mode,entry_data,entry_size) == -1 ){
        D("ERROR:Failed to write entry data real_entry_size:%lu",entry_size);
        free(entry_data);
        return -1 ;
    }
    free(entry_data);
    return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ static struct bootimage_utils_archive* __archive_find_entry_in_archive_data(struct bootimage_utils_archive* biua ,char* name,size_t name_length)
{
       int break_error = 0 ;
    int break_found = 0 ;
    while (archive_read_next_header( biua->archive, &biua->archive_entry) == ARCHIVE_OK) {
        /* Get the entry information we need */
        char* entry_name = archive_entry_pathname(biua->archive_entry) ;

        D("archive_entry_pathname=%s fi.zip_name=%s",entry_name,name,name_length);
        if ( !strncmp( entry_name , name ,name_length) ){
            break_found = 1 ;
            break;
        }
    }
    if(break_found == 0 ){
        archive_read_free(biua->archive);
       free(biua);
    }
    return biua;
}
__LIBBOOTIMAGE_PRIVATE_API__ static struct bootimage_utils_archive* __archive_find_entry(char* data,off_t data_size,char* name,size_t name_length)
{
    struct bootimage_utils_archive* biua = calloc(1,sizeof(struct bootimage_utils_archive));
    biua->archive = __archive_read_memory(data,data_size) ;
    if (   biua->archive == NULL ){
        return NULL ;
    }
    if ( name_length == 0 ){
        name_length = utils_sanitize_string(name,PATH_MAX);
    }
    return __archive_find_entry_in_archive_data(biua,name,name_length);


}
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_file(char* archive_data,off_t archive_data_size,char* name,size_t name_length)
{


       struct bootimage_utils_archive* biua  = __archive_find_entry(archive_data,archive_data_size,name,name_length);
       if( biua == NULL ){
           return 0;
       }
       char*  entry_data = NULL ;
        switch ( archive_entry_filetype(biua->archive_entry) ){
                case AE_IFREG:{ // Entry File Type is a Regular File
                    __archive_extract_entry_file(biua->archive,biua->archive_entry,NULL);


                }
                default:
                    break ;
            }

        archive_read_free(biua->archive);
       free(biua);

    return 0;
}


__LIBBOOTIMAGE_PRIVATE_API__  char* archive_extract_entry(char* data,off_t data_size,char* name,size_t name_length,size_t* entry_size){


       struct bootimage_utils_archive* biua  = __archive_find_entry(data,data_size,name,name_length);
       if( biua == NULL ){
           return NULL;
       }
       char*  entry_data = NULL ;
        switch ( archive_entry_filetype(biua->archive_entry) ){
                case AE_IFREG:{ // Entry File Type is a Regular File
                    entry_data = __archive_extract_entry(biua->archive,biua->archive_entry,entry_size);
                    D("entry_data=%p",entry_data);

                }
                default:
                    break ;
            }


        archive_read_free(biua->archive);
       free(biua);

      return entry_data;


}
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_memory_file( char* archive_data , uint64_t archive_size, char* entry_name, char* output_file_name)
{



   /* if ( check_output_name(output_file_name)  == -1 ) {
		return -1 ;
	}
    int entry_name_len = check_output_name(entry_name) ;
    if ( entry_name_len == -1 ) {
		return -1 ;
	}

    struct archive *a = archive_read_memory(archive_data,archive_size);
    if (  a == NULL ){
        return -1 ;
    }
    D("a=%p\n",a);

    struct archive_entry *entry = NULL;
    int break_error = 0 ;
    int break_found = 0 ;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {*/
        /* Get the entry information we need */
        /*char* name = archive_entry_pathname(entry) ;
        D("archive_entry_pathname=%s",name);
        if ( !strncmp( name , entry_name,entry_name_len) ){
            switch ( archive_entry_filetype(entry) ){
                case AE_IFREG:{ /* Entry File Type is a Regular File */
       /*             break_found = 1 ;
                    if ( __archive_extract_entry_file(a,entry,output_file_name) == -1 ) {
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
    }*/
    return 0;


}

__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all_memory_directory( char* archive_data , uint64_t archive_size, char* output_dir_name)
{

    if ( check_output_name(output_dir_name)  == -1 ) {
		return -1 ;
	}

    struct archive *a = __archive_read_memory(archive_data,archive_size);
    if (  a == NULL ){
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
                    __archive_extract_entry_file(a,entry,NULL);
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
