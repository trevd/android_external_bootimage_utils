
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <program.h>

#define PROGRAM_TITLE "Android Boot Image Utilities"
#define PROGRAM_VERSION "x.xx Alpha Release"

#define PROGRAM_DESCRIPTON "A collection of utilities to manipulate every aspect of android boot images"

unsigned print_program_title(){
    
    static unsigned printed ;
    if(!printed){
        fprintf(stderr,"\n %s %s\n\n",PROGRAM_TITLE,PROGRAM_VERSION);
        printed = 1 ;
    }
    return 0 ; 
}
unsigned print_program_title_and_description(){
    
    static unsigned printed ;
    if(!printed){
        fprintf(stderr,"\n %s %s\n",PROGRAM_TITLE,PROGRAM_VERSION);
        fprintf(stderr,"\n %s \n\n",PROGRAM_DESCRIPTON);
        printed = 1 ; 
    }
    return 0 ;
    
}

unsigned print_program_error_processing(char* filename){
    
    //  file too large error. no pounsigned in contining
    unsigned save_err = errno ; 
	print_program_title();
	fprintf(stderr," Cannot process \"%s\" - error : %d %s\n\n",filename,save_err,strerror(save_err));
    return save_err;
}
unsigned print_program_error_file_type_not_recognized(char* filename){
    
    print_program_title();
    fprintf(stderr," Cannot process \"%s\" - file type not a recognized\n\n",filename);
    errno = ENOEXEC ;
    return errno ;
}
unsigned print_program_error_file_name_not_found(char * filename){
    
    print_program_title();
    if(!filename){
        fprintf(stderr," no file specified!\n\n");
        errno = EINVAL ;
    }else{
        fprintf(stderr," %s - file not found!\n\n",filename);
        errno = ENOENT ;
    }
    return errno;
}
unsigned print_program_error_file_not_boot_image(char * filename){
    
    unsigned save_err = errno ; 
    print_program_title();
    fprintf(stderr," cannot open file \"%s\" as boot image - error %d - %s\n\n",filename ,errno , strerror(errno));
    return errno;
}
unsigned print_program_error_file_write_boot_image(char * filename){
    
    unsigned save_err = errno ; 
    print_program_title();
    fprintf(stderr,"write_boot_image failed %d %s\n",errno,strerror(errno));
    return errno;
}