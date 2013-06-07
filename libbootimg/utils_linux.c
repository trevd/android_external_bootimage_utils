/*
 * utils_linux.c
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
        errno = 0;
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
                        if((strlen(opath) > 0) && (access(opath, F_OK))){
                            D("in loop opath=%s\n",opath);  
                            mkdir(opath, mode);
                            
                        }
                        *p = '/';
                        
                }
            }
            
        D("opath=%s\n" ,opath);  
        if(access(opath, F_OK)){ 
            /* if path is not terminated with / */
            errno = 0 ;     
            mkdir(opath, mode);
        }
        D("opath=%s errno=%u %s\n",opath,errno,strerror(errno));
        return 0 ;
}
int symlink_os(const char *source, size_t source_size,const char *path){
    
    
    char symlink_src[source_size+1];
    memcpy(symlink_src,source,source_size);
    symlink_src[source_size] ='\0';
    return symlink(symlink_src,path);
}
int readlink_os(const char *path, char *buf, size_t bufsiz){
    return readlink(path,buf,bufsiz);
}

int get_exe_path(char* buffer,size_t buffer_size){
    
    readlink_os("/proc/self/exe",buffer,buffer_size);
    D("buffer=%s\n",buffer);
    return 0 ;
}
