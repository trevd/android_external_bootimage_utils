#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <private/api.h>
#include <private/utils.h>

__LIBBOOTIMAGE_PRIVATE_API__  DIR* mkdirat_umask(const char *path,unsigned mode, mode_t mask)
{
    mode_t oldumask = umask(mask);
    DIR* ret = mkdir_and_parents(path,mode);
    umask(oldumask);
    return ret;
}

__LIBBOOTIMAGE_PRIVATE_API__  DIR* mkdir_and_parents_umask(const char *path,unsigned mode, mode_t mask)
{
    mode_t oldumask = umask(mask);
    DIR* ret = mkdir_and_parents(path,mode);
    umask(oldumask);
    return ret;
}
__LIBBOOTIMAGE_PRIVATE_API__  DIR* mkdir_and_parents(const char *path,unsigned mode)
{
        errno = 0;
        char opath[PATH_MAX];
        char *p;
        size_t len;

        if(strnlen(path,PATH_MAX) >= PATH_MAX){
            errno = ENAMETOOLONG ;
            return NULL;
        }

        /* stat the full path, see if we have an existing directory */
        struct stat statbuf ;
        if ( stat(path, &statbuf) == 0 ){
            if ( S_ISDIR(statbuf.st_mode) ){
                return opendir(path);
            }else
                /* path exists but not a directory */
                /* D("path %s found but not a directory\n",path); */
                errno = ENOTDIR ;
                return NULL;
        }

        strncpy(opath,(char*) path, sizeof(opath));
        len = strnlen(opath,PATH_MAX);

        if(opath[len - 1] == '/'){
            /* Replace a trailing slash with a null
             we make the assumption that the user did not want to
             create a directory that ended with a slash */
            opath[len - 1] = '\0';
        }
        for(p = opath; *p; p++){
            if(*p == '/') {
                *p = '\0';
                if((strnlen(opath,sizeof(opath)) > 0) && (access(opath, F_OK))){
                    /* D("in loop opath=%s\n",opath); */
                    mkdir(opath, mode);
                }
                *p = '/';
           }
        }

        /* D("opath=%s errno=%u %s\n",opath,errno,strerror(errno)); */
        if(access(opath, F_OK)){
            /* if path is not terminated with / */
            errno = 0 ;
            mkdir(opath, mode);
        }
        /* D("opath=%s errno=%u %s\n",opath,errno,strerror(errno)); */

        return opendir(path);

}
