#include <string.h>
#include <utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <linux/fs.h>
#include <fcntl.h>
unsigned char* read_from_block_device(const char *name, unsigned* data_size){
    
    
    D("read_from_block_device data %s\n",name);
    unsigned char *data =NULL;
    unsigned long numblocks=0;
    unsigned size =0;
    int fd = open(name, O_RDONLY);
    if(!fd){
	D("read_from_block_device data faile %lu\n",numblocks);
	return NULL ;
    }
    ioctl(fd, BLKGETSIZE64, &numblocks);
    D("read_from_block_device data numblocks %lu\n",numblocks);
    if (numblocks) size =numblocks;
    if(numblocks > (BOOT_IMAGE_SIZE_MAX)){
	errno = EFBIG;
	return NULL;
    }
    data = calloc(size,sizeof(char));
    if ((*data_size = read(fd, data, size)) != size) goto oops;
    close(fd);
    return data;
oops:
    close(fd);
    //printf("Number of blocks: %lu, this makes %.3f GB\n",numblocks, (double)numblocks * 512.0 / (1024 * 1024 * 1024));
    return NULL;
}

int mkdir_and_parents(const char *path,unsigned mode)
{
        errno;
        char opath[256];
        char *p;
        size_t len;

        strncpy(opath,(char*) path, sizeof(opath));
        len = strlen(opath);
        if(opath[len - 1] == '/')
                opath[len - 1] = '\0';
        for(p = opath; *p; p++){
                if(*p == '/') {
                        *p = '\0';
                        if(access(opath, F_OK))
                                mkdir(opath, mode);
                        *p = '/';
                }
            }
        if(access(opath, F_OK))         /* if path is not terminated with / */
                mkdir(opath, mode);
        return 0 ;
}
int symlink_os(const char *source, size_t size,const char *path){
    
    
    char symlink_src[size+1];
    memcpy(symlink_src,source,size);
    symlink_src[size] ='\0';
    return symlink(symlink_src,path);
}
int readlink_os(const char *path, char *buf, size_t bufsiz){
    return readlink(path,buf,bufsiz);
}
