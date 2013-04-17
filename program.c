
#include <stdio.h>
#include <errno.h>

#include <program.h>

#define PROGRAM_TITLE "Android Boot Image Utilities"
#define PROGRAM_VERSION "x.xx Alpha Release"

#define PROGRAM_DESCRIPTON "A collection of utilities to manipulate every aspect of android boot images"

int print_program_title(){
    
    fprintf(stderr,"\n %s %s\n\n",PROGRAM_TITLE,PROGRAM_VERSION);
    return 0 ; 
}
int print_program_title_and_description(){
    
    fprintf(stderr,"\n %s %s\n",PROGRAM_TITLE,PROGRAM_VERSION);
    fprintf(stderr,"\n %s \n\n",PROGRAM_DESCRIPTON);
    return 0 ;
    
}

int print_program_error_file_type_not_recognized(char* filename){
    
    print_program_title();
    fprintf(stderr," Cannot process \"%s\" - file type not a recognized\n\n",filename);
    errno = EINVAL ;
    return errno ;
}
int print_program_error_file_name_not_found(char * filename){
    
    print_program_title();
    if(!filename)
        fprintf(stderr," no file specified!\n\n");
    else
        fprintf(stderr," %s - file not found!\n\n",filename);
    
    errno = EINVAL ;
    return errno;
}