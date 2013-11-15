// boot image loading test program.
// Load a boot
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <bootimage.h>
#include <utils.h>
int main(int argc, char** argv){

	utils_debug = 1;
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
    
    int return_value = load_boot_image_from_file(filename,image);
    fprintf(stderr,"load_boot_image function returns %d %s\n",return_value,strerror(return_value));
    if(return_value != 0){
        if(image != NULL  ) boot_image_free(image);
        return return_value;
    }
    print_boot_image_header_info(image);
    print_boot_image_additional_info(image);
    boot_image_free(image);
    return 0;   
        
    
}
