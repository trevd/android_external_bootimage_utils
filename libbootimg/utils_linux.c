#include <string.h>
#include <utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
void mkdir_and_parents(const char *path,unsigned mode)
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
                                mkdir(opath, mode);
                        *p = '/';
                }
        if(access(opath, F_OK))         /* if path is not terminated with / */
                mkdir(opath, mode);
}
int symlink_os(const char *source, unsigned size,const char *path){
    
    
    char symlink_src[size+1];
    memcpy(symlink_src,source,size);
    symlink_src[size] ='\0';
    return symlink(symlink_src,path);
}
