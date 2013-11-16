// boot image loading test program.
// Load a boot
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <bootimage.h>
#include <utils.h>
int main(int argc, char** argv){


    char* filename;

    
    fprintf(stderr,"\nBoot image kernel unpack test\n");
    if(argc == 1){
        fprintf(stderr,"No filename : using default boot.img\n");
        filename="boot.img";
    }else{
        fprintf(stderr,"filename %s\n",argv[1]);
        filename=argv[1];
    }
    boot_image* image = abu_bootimage_init(); 
	abu_bootimage_open_archive(image,filename);
    int return_value = 0; //load_boot_image_from_file(filename,image);
    fprintf(stderr,"load_boot_image function returns %d %s\n",return_value,strerror(return_value));
    if(return_value != 0){
		
        if(image != NULL  ) abu_bootimage_destroy(image);
        return return_value;
    }
    //print_boot_image_header_info(image);
    //print_boot_image_additional_info(image);
    
    abu_bootimage_destroy(image);
    return 0;   
        
    
}
