// boot image loading test program.
// Load a boot
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <bootimage.h>
int main(int argc, char** argv){

    char* filename;
    boot_image image;
   
    fprintf(stderr,"\nBoot image kernel unpack test\n");
    if(argc == 1){
        fprintf(stderr,"No filename : using default boot.img\n");
        filename="boot.img";
    }else{
        fprintf(stderr,"filename %s\n",argv[1]);
        filename=argv[1];
    }
    
    int return_value = load_boot_image(filename,&image);
    fprintf(stderr,"load_boot_image function returns %d %s\n",return_value,strerror(return_value));
    if(return_value != 0){
        if(image.start_addr != NULL  ) free(image.start_addr);
        return return_value;
    }
    
    
    kernel_image kimage; 
    return_value = load_kernel_image(image.kernel_addr,image.kernel_size,&kimage);
    fprintf(stderr,"unpack_kernel_image function returns %d %s\n",return_value,strerror(return_value));
    if(return_value != 0){
        if(kimage.start_addr != NULL  ) free(kimage.start_addr);
        return return_value;
    }
    fprintf(stderr,"\nkernel_image struct values:\n");
    fprintf(stderr,"  start_addr       :%p\n",kimage.start_addr);
    fprintf(stderr,"  config_addr      :%p\n",kimage.config_addr);
    fprintf(stderr,"  size             :%u\n",kimage.size);
    fprintf(stderr,"  compression_type :%s\n",str_kernel_compression(kimage.compression_type));
    fprintf(stderr,"  config_size      :%u\n",kimage.config_size);
    fprintf(stderr,"  version          :%s\n",kimage.version);
    
    fprintf(stderr,"\nSaving unpacked kernel image to unpacked_kernel\n");
    FILE* fp  = fopen("unpacked_kernel","w+b");
    fwrite(kimage.start_addr,kimage.size,1,fp);
    fclose(fp);
    
    fprintf(stderr,"Saving kernel config.gz to config.gz\n");
    fp  = fopen("config.gz","w+b");
    fwrite(kimage.config_addr,kimage.config_size,1,fp);
    fclose(fp);
    
    free(kimage.start_addr);
    free(image.start_addr);
    return 0;   
        
    
}