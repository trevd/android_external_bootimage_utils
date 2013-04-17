
#include <stdio.h>

#include <program.h>

#define PROGRAM_TITLE "Android

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