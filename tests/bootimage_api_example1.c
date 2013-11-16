// boot image loading test program.
// Load a boot
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <bootimage.h>
#include <utils.h>
int main(int argc, char** argv){

	
    char* filename;
    boot_image* image = boot_image_allocate(); 
   
    fprintf(stderr,"\nBoot image kernel unpack test\n");
    if(argc == 1){
        fprintf(stderr,"No filename : using default boot.img\n");
        filename="boot.img";
    }else{
        fprintf(stderr,"filename %s\n",argv[1]);
        filename=argv[1];
    }
    boot_image_free(image);
  
   
    return 0;   
        
    
}