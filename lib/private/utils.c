#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <private/api.h>

__LIBBOOTIMAGE_PRIVATE_API__  int mkdir_and_parents(const char *path,unsigned mode)
{
        errno = 0;
        char opath[PATH_MAX];
        char *p;
        size_t len;

        if(strnlen(path,PATH_MAX) >= PATH_MAX){
            errno = ENAMETOOLONG ;
            return -1;
        }

        // stat the full path, see if we have an existing directory
        struct stat statbuf ;
        if ( stat(path, &statbuf) == 0 ){
            if ( S_ISDIR(statbuf.st_mode) ){
                return 0 ;
            }else
                // path exists but not a directory
                //D("path %s found but not a directory\n",path);
                errno = ENOTDIR ;
                return -1;
        }

        strncpy(opath,(char*) path, sizeof(opath));
        len = strlen(opath);

        if(opath[len - 1] == '/'){
            // Replace a trailing slash with a null
            // we make the assumption that the user did not want to
            // create a directory that ended with a slash
            opath[len - 1] = '\0';
        }

        for(p = opath; *p; p++){
            if(*p == '/') {
                *p = '\0';
                if((strnlen(opath,sizeof(opath)) > 0) && (access(opath, F_OK))){
                    //D("in loop opath=%s\n",opath);
                    mkdir(opath, mode);
                }
                *p = '/';
           }
        }

        //D("opath=%s errno=%u %s\n",opath,errno,strerror(errno));
        if(access(opath, F_OK)){
            /* if path is not terminated with / */
            errno = 0 ;
            mkdir(opath, mode);
        }
        //D("opath=%s errno=%u %s\n",opath,errno,strerror(errno));
        return 0 ;
}
