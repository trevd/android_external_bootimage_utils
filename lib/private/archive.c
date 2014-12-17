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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <private/api.h>


#include <archive.h>
#include <archive_entry.h>

__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_memory_file( char* data , uint64_t data_size, char* entry_name, FILE* target)
{


    int entry_name_len = check_output_name(entry_name) ;
    if ( entry_name_len == -1 ) {
		return -1 ;
	}


    struct archive *a = NULL ;
    if ( check_archive_read_memory(&a,data,data_size) == -1 ){
        return -1 ;
    }
    D("a=%p\n",a);

    struct archive_entry *entry = NULL;

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        /* Get the entry information we need */
        char* name = archive_entry_pathname(entry) ;
        if ( !strncmp( name , entry_name,entry_name_len) ){
            switch ( archive_entry_filetype(entry) ){
                case AE_IFREG:{ /* Entry File Type is a Regular File */
                    uint64_t size = archive_entry_size(entry);

                    /* Read the entry data if we have a regular file */
                    unsigned char* data = calloc(size,sizeof(unsigned char));
                    if ( data == NULL ){
                        /* calloc failed. Probably not a good thing. So Bailout */
                        return -1 ;
                    }
                    if ( archive_read_data(a,data,size) == ARCHIVE_FATAL ){
                        /* there was a fatal error; the archive should be closed immediately */
                        free(data);
                        return -1 ;
                    }


                }
                default:
                    break ;
            }
        }

    }


    return 0;


}

__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all_memory_directory( char* data , uint64_t data_size, DIR* target)
{

    struct archive *a = NULL ;
    if ( check_archive_read_memory(&a,data,data_size) == -1 ){
        return -1 ;
    }

    D("archive_compression_name=%s\n",archive_compression_name(a));
    if ( archive_extract_all(a,target) == -1 ){
        int en = errno ;
        archive_read_free(a);
        errno = en ;
        return -1 ;
    }
    archive_read_free(a);
    return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all(struct archive *a,DIR* target)
{
        int dfd = dirfd(target);
        if ( dfd == -1 ){
            return -1 ;
        }
        struct archive_entry *entry = NULL;

        while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {


            /* Get the entry information we need */
            char* name = archive_entry_pathname(entry) ;
            D("name:%s\n", name);
            uint64_t size = archive_entry_size(entry);
            mode_t mode = archive_entry_mode(entry) ;
            switch ( archive_entry_filetype(entry) ){
                case AE_IFDIR:{ /* Entry File Type is a Directory */
                    mkdirat(dfd,name,mode);
                    archive_read_data_skip(a);
                    break;
                }
                case AE_IFLNK:{ /* Entry File Type is a Symbolic Link */
                    symlinkat(archive_entry_symlink(entry),dfd, name);
                    archive_read_data_skip(a);
                    break;
                }
                case AE_IFREG:{ /* Entry File Type is a Regular File */
                    /* Read the entry data if we have a regular file */
                    unsigned char* data = calloc(size,sizeof(unsigned char));
                    if ( data == NULL ){
                        /* calloc failed. Probably not a good thing. So Bailout */
                        return -1 ;
                    }
                    if ( archive_read_data(a,data,size) == ARCHIVE_FATAL ){
                        /* there was a fatal error; the archive should be closed immediately */
                        free(data);
                        return -1 ;
                    }

                    int ffd =  openat(dfd,name, O_CREAT | O_TRUNC | O_WRONLY, mode);
                    if ( ffd == -1 ){
                        free(data);
                        continue ;
                    }
                    write(ffd,data,size);
                    close(ffd);
                    break;
                }
                default:{ /* Entry File Type is a something else and unsupported */

                    archive_read_data_skip(a);
                    break;
                }
            }
        }
        return 0 ;
}
