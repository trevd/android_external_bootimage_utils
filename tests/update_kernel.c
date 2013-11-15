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
    fprintf(stderr,"\nkernel info:\n");
    fprintf(stderr,"  kernel_addr      :%p\n",image.kernel_addr); 
    fprintf(stderr,"  kernel_size      :%u\n",image.kernel_size); 
    fprintf(stderr,"  kernel version   :%s\n",kimage.version);
      
    free(kimage.start_addr);
    
    fprintf(stderr,"\nloading new-kernel:\n");
    FILE* new_kernel = fopen("new-kernel","r+b");
    fseek(new_kernel,0,SEEK_END);
    unsigned size = ftell(new_kernel);
    fprintf(stderr,"  new_kernel size     :%u\n",size); 
    fseek(new_kernel,0,SEEK_SET);
    unsigned char kmem[size];
    fread(kmem,size,1,new_kernel);
    fclose(new_kernel);
    image.kernel_addr = &kmem;
    image.kernel_size = size;
    fprintf(stderr,"  kernel_addr      :%p\n",image.kernel_addr); 
    set_boot_image_padding(&image);
    set_boot_image_offsets(&image);
    set_boot_image_content_hash(&image);
    
    write_boot_image("newboot.img",&image);
    free(image.start_addr);
    
    boot_image newimage;
    return_value = load_boot_image("newboot.img",&newimage);
    fprintf(stderr,"load_boot_image function returns %d %s\n",return_value,strerror(return_value));
    if(return_value != 0){
        if(newimage.start_addr != NULL  ) free(newimage.start_addr);
        return return_value;
    }
    
    
    kernel_image knewimage; 
    return_value = load_kernel_image(newimage.kernel_addr,image.kernel_size,&knewimage);
    fprintf(stderr,"unpack_kernel_image function returns %d %s\n",return_value,strerror(return_value));
    if(return_value != 0){
        if(knewimage.start_addr != NULL  ) free(knewimage.start_addr);
        return return_value;
    }
    fprintf(stderr,"  version          :%s\n",knewimage.version);
    
    free(knewimage.start_addr);
    free(newimage.start_addr);
    return 0;   
        
    
}