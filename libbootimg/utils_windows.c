/*
 * utils_windows.c
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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils_windows.h>

unsigned char* read_from_block_device(const char *name, unsigned* data_size){
    return NULL;
}

int mkdir_and_parents(const char *path,unsigned mode)
{
        char opath[256];
        char *p;
        size_t len;

        strncpy(opath,(char*) path, sizeof(opath));
        len = strlen(opath);
        if(opath[len - 1] == '/')
                opath[len - 1] = '\0';
        for(p = opath; *p; p++)
                if(*p == '/') {
                        *p = '\0';
                        if(access(opath, F_OK))
                                mkdir(opath);
                        *p = '/';
                }
        if(access(opath, F_OK))         /* if path is not terminated with / */
                mkdir(opath);
        return 0;
}
int symlink_os(const char *source, size_t source_size,const char *path){
    
    
    /*D("CreateSymbolicLink source=%s path=%s\n",source,path); 
    if(!CreateSymbolicLink((LPSTR)path, (LPSTR)source, 0)){
        int error = GetLastError();
        D("CreateSymbolicLink Error:%d\n",error); 
        
    }
    return 0;
    */
    FILE *output_file_fp = fopen(path, "wb");
    if (output_file_fp != NULL)
    {
        fwrite("LNK:",4,1,output_file_fp);
        fwrite(source,source_size,1,output_file_fp);
        fwrite("\0",1,1,output_file_fp);
        fclose(output_file_fp);
    }
    
}
int readlink_os(const char *path, char *buf, size_t bufsiz){
    
    
    return 0;
}

int get_exe_path(char* buffer,size_t buffer_size){
    return 0 ;
}
