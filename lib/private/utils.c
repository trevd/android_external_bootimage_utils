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
 * file : lib/private/utils.c
 *
 */
#define  TRACE_TAG   TRACE_PRIVATE_UTILS
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <private/api.h>

#ifdef _WIN32
#define mkdir_os(path,mode) mkdir(path)
#define symlink(source,path) ((void)0)
#else
#define mkdir_os(path,mode) mkdir(path,mode)
#endif

__LIBBOOTIMAGE_PRIVATE_API__  int utils_mkdirat_umask(const char *path,unsigned mode, mode_t mask)
{
    mode_t oldumask = umask(mask);
    int ret = utils_mkdir_and_parents(path,mode);
    umask(oldumask);
    return ret;
}

__LIBBOOTIMAGE_PRIVATE_API__  int utils_mkdir_and_parents_umask(const char *path,unsigned mode, mode_t mask)
{
    mode_t oldumask = umask(mask);
    int ret = utils_mkdir_and_parents(path,mode);
    umask(oldumask);
    return ret;
}
__LIBBOOTIMAGE_PRIVATE_API__  int utils_mkdir_and_parents(const char *path,unsigned mode)
{
        errno = 0;
        char opath[PATH_MAX];
        char *p;
        size_t len;

        if(strnlen(path,PATH_MAX) >= PATH_MAX){
            errno = ENAMETOOLONG ;
            return -1;
        }

        /* stat the full path, see if we have an existing directory */
        struct stat statbuf ;
        if ( stat(path, &statbuf) == 0 ){
            if ( S_ISDIR(statbuf.st_mode) ){
                return 0;
            }else
                /* path exists but not a directory */
                /* D("path %s found but not a directory\n",path); */
                errno = ENOTDIR ;
                return -1;
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
                    mkdir_os(opath, mode);
                }
                *p = '/';
           }
        }

        /* D("opath=%s errno=%u %s\n",opath,errno,strerror(errno)); */
        if(access(opath, F_OK)){
            /* if path is not terminated with / */
            errno = 0 ;
            mkdir_os(opath, mode);
        }
        /* D("opath=%s errno=%u %s\n",opath,errno,strerror(errno)); */

        return 0;

}
__LIBBOOTIMAGE_PRIVATE_API__ int utils_paranoid_strnlen(char* s,int maxlen)
{
    /* The Paranoid strnlen function runs strnlen then checks the returned
       string again for non printable values and breaks at the first one it find
       length */
    D("s=%s maxlen=%d",s,maxlen);
    int len = strnlen(s,maxlen);
    if ( len == maxlen ) {
        D("s=%s maxlen=%d len=%d",s,maxlen,len);
        s[len-1] = '\0';
        return 0;
    }
    D("len=%d",len);
    char *p = s;
    int i = 0;
    for(i = 0 ; i <= len ; i++){
        if ( ( s[i] < '\x20' ) || ( s[i] > '\x7E' ) ) {
            if ( len > i ){
                D("non ascii char found %d %c",i,s[i]);
                len = i ;
                s[i] = '\0';
            }
            break ;
        }

    }
    D("returning len=%d",len);
    return len ;

}
__LIBBOOTIMAGE_PRIVATE_API__ char* utils_dirname(char* s)
{
    D("s=%s",s);
    if(s == NULL ){
        return NULL ;
    }

    char* d = strrchr(s,'/');

    D("d=%s",d);
    if(d == NULL ){
        return NULL;
    }
    int len = d-s ;
    D("len=%d",len);
    char* r = calloc(len , sizeof(char));
    strncpy(r,s,len);
    D("r=%s",r);
    return r;

}
__LIBBOOTIMAGE_PRIVATE_API__ ssize_t utils_write_all (int fd, const void* buffer, size_t count)
{
  ssize_t left_to_write = count;
  while (left_to_write > 0) {
    ssize_t written = write (fd, buffer, count);
    if (written == -1)
      /* An error occurred; bail.  */
      return -1;
    else
      /* Keep count of how much more we need to write.  */
      left_to_write -= written;
  }
  /* We should have written no more than COUNT bytes!   */
  assert (left_to_write == 0);
  /* The number of bytes written is exactly COUNT.  */
  return count;
}
