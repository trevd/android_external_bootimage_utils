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

#include <private/api.h>

#ifdef _WIN32
#define mkdir_os(path,mode) mkdir(path)
#define symlink(source,path) ((void)0)
#else
#define mkdir_os(path,mode) mkdir(path,mode)
#endif


__LIBBOOTIMAGE_PRIVATE_API__  int utils_mkdirat_umask( char *path,unsigned mode, mode_t mask)
{
    mode_t oldumask = umask(mask);
    int ret = utils_mkdir_and_parents(path,mode);
    umask(oldumask);
    return ret;
}

__LIBBOOTIMAGE_PRIVATE_API__  int utils_mkdir_and_parents_umask( char *path,unsigned mode, mode_t mask)
{
    mode_t oldumask = umask(mask);
    int ret = utils_mkdir_and_parents(path,mode);
    umask(oldumask);
    return ret;
}
__LIBBOOTIMAGE_PRIVATE_API__  int utils_mkdir_and_parents( char *path,unsigned mode)
{
        errno = 0;
        char opath[PATH_MAX];
        char *p;
        size_t len;

        if( utils_sanitize_string(path,PATH_MAX) == SSIZE_MAX ){
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
/* utils_sanitize_string - sanitizes the s argunement
 * returns the string length and also zero terminates the input string  */
__LIBBOOTIMAGE_PUBLIC_API__ ssize_t utils_sanitize_string(char* s,ssize_t maxlen)
{
	D("s=%s",s);
	if ( s == NULL ){
		errno = EBIOUTNAME ;
		return -1;
	}
    /* The Paranoid strnlen function runs strnlen then checks the returned
       string again for non printable values and breaks at the first one it find
       length */

    const char* cs = s;
    D("s=%s maxlen=%zu",s,maxlen);
    ssize_t len = strnlen(cs,maxlen);
    if ( len == maxlen ) {
        D("s=%s maxlen=%zu len=%zu",s,maxlen,len);
        s[len-1] = '\0';
        return len;
    }
    D("len=%zu",len);
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
    D("returning len=%zu",len);
    return len ;

}
__LIBBOOTIMAGE_PRIVATE_API__ char* utils_dirname(char* s)
{
    D("s=%s",s);
    size_t len = utils_sanitize_string(s,PATH_MAX);
    if(len == SIZE_MAX ){
        return NULL ;
    }

    char* d = strrchr(s,'/');

    D("d=%s",d);
    if(d == NULL ){
        return NULL;
    }
    int dir_len = d-s ;
    D("dir_len=%d",dir_len);
    char* r = calloc(dir_len , sizeof(char)+1);
    strncpy(r,s,dir_len);
    D("r=%s",r);
    return r;

}
__LIBBOOTIMAGE_PRIVATE_API__ char* utils_basename(char* s)
{
    D("s=%s",s);
    size_t len = utils_sanitize_string(s,PATH_MAX);
    if(len == SIZE_MAX ){
        return NULL ;
    }
    char* r = strrchr(s,'/');
    if( r == NULL ){
        return s;
    }
    return r+1;

}
__LIBBOOTIMAGE_PRIVATE_API__ ssize_t utils_write_all_fd (int fd, const void* buffer, ssize_t count)
{

    if ( fd <= 0 ){
        E("fd %d",fd);
        return -1 ;
    }
    if ( buffer == NULL ){
        ENULLARG("buffer");
        return -1;
    }
    if ( count <= 0 ){
        E("count %zu",count);
        return -1 ;
    }

  ssize_t left_to_write = count;
  while (left_to_write > 0) {
    ssize_t written = write (fd, buffer, count);
    if (written == -1){
      /* An error occurred; bail.  */
      return -1;
    }
    /* Keep count of how much more we need to write.  */
    left_to_write -= written;
  }
  /* We should have written no more than COUNT bytes!   */
  assert (left_to_write == 0);
  /* The number of bytes written is exactly COUNT.  */
  return count;
}
__LIBBOOTIMAGE_PRIVATE_API__ ssize_t utils_write_all (char* file_name,mode_t mode, const void* buffer, ssize_t count)
{
  	/* Open the file as read only, read for mmapping */
    ssize_t len = utils_sanitize_string(file_name,PATH_MAX);
    if( len == -1 ){
        return -1 ;
    }
    char* dirname = utils_dirname(file_name) ;
    if ( dirname != NULL ) {
        utils_mkdir_and_parents_umask(dirname,0755,0);
    }

	int fd =  open(file_name, O_CREAT | O_TRUNC | O_WRONLY, mode);
	D("fd=%d",fd);
	if(fd < 0 ){
        D("could not open file_name %s",file_name);
		/* Could not open file. errno should be set already so return -1 */
		return -1;
	}
    ssize_t written = utils_write_all_fd(fd,buffer,count);
    close(fd);
    return written;
}
/* utils_read_all - Populate buffer with data and st with stat info for filename  */
__LIBBOOTIMAGE_PRIVATE_API__ int utils_read_all(char* file_name,char** buffer, struct stat* st)
{

    if ( utils_sanitize_string(file_name,PATH_MAX) == -1 ){
        errno = EBIFNAME ;
        return -1 ;
    }
    /* Does the file exist? , can we read it? */
	if ( access(file_name , R_OK ) == -1 ) {
		errno = EBIFACCESS ;
		return -1 ;
	}
    if (stat(file_name, st) == -1){
		E("stat");
		errno = EBISTAT;
		return -1 ;
	}

    if ( st->st_size <= 0 ) {
        D("wrong file_size %jd",st->st_size);
            return -1;
    }
	/* Open the file as read only, read for mmapping */
	int fd = open(file_name,O_RDONLY);
	D("fd=%d",fd);
	if(fd < 0 ){
        D("could not open file_name %s",file_name);
		/* Could not open file. errno should be set already so return -1 */
		return -1;
	}



    buffer[0] = calloc(st->st_size,sizeof(char));
    D("dest=%p",buffer[0] );
    ssize_t left_to_read = st->st_size;
    while (left_to_read > 0 ) {
        ssize_t tmp_bytes_read = read(fd, buffer[0], left_to_read);
        if ( tmp_bytes_read <= 0 ){
            D("bytes_read=%zu errno=%d %s",tmp_bytes_read , errno, strerror(errno) );
            close(fd);
            return -1;
        }
        left_to_read -= tmp_bytes_read;
        buffer += tmp_bytes_read;


    }
    close(fd);
    //D("buffer 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",buffer[0][0],buffer[0][1],buffer[0][2],buffer[0][3],buffer[0][4],buffer[0][5]);
    D("left_to_read=%zu",left_to_read );
    return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__ unsigned char *utils_memmem(unsigned char *haystack, unsigned haystack_len, char* needle, unsigned needle_len)
{

    errno = 0;
    if(!haystack)
        errno = ENOMEM ;
    else if(!needle)
        errno = ENOMEM ;
    else if(haystack_len < needle_len || !haystack_len || !needle_len)
        errno = EINVAL ;

    if(errno) return NULL ;



    size_t begin=0;
    unsigned char* uneedle = (unsigned char *)needle ;
    //D("find_in_memory haystack=%p haystack_len=%u needle=%p needle_len=%u\n",haystack,haystack_len,needle,needle_len);
    //fprintf(stderr,"Memory HS:%p HL:%u\n",haystack,   haystack_len);
    //D("haystack[0]='%x' needle[0]='%x'\n",haystack[0],uneedle[0]);

    for(begin=0 ; begin < haystack_len; begin++){
        // make sure we are comparing apples with apples
        if(haystack[begin]==uneedle[0]){

            if(!memcmp(uneedle,haystack+begin,needle_len)){
                //D("haystack[%d]='%x'\n",begin,haystack[begin]);
                return haystack+begin;
            }
        }
    }

    //D("INFO: needle Not Found In Memory\n");
    return NULL;
}
