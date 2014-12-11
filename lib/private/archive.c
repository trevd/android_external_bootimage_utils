#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <private/api.h>
#include <private/archive.h>
#include <private/checks.h>

#include <archive.h>
#include <archive_entry.h>

__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all_memory( char* data , uint64_t data_size, DIR* target)
{

    struct archive *a = NULL ;
    fprintf(stdout,"archive_extract_all_memory a=%p\n",a);
    if ( check_archive_read_initialization(&a) == -1 ){
        return -1 ;
    }
    fprintf(stdout,"archive_extract_all_memory a=%p\n",a);
	int r = archive_read_open_memory(a, data,data_size);
	if (r != ARCHIVE_OK){
		return -1;
	}
     fprintf(stdout,"archive_extract_all_memory\n");
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
            fprintf(stdout,"name:%s\n", name);
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
