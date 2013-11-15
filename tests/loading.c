// boot image loading test program.
// Load a boot
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <bootimage.h>
int main(int argc, char** argv){

    char* filename;
    boot_image image;
   
    fprintf(stderr,"\nBoot image loading test\n");
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
    
    fprintf(stderr,"\nboot_image struct values:\n");
    fprintf(stderr," memory locations:\n");
    fprintf(stderr,"  start_addr       :%p\n",image.start_addr);
    fprintf(stderr,"  header_addr      :%p\n",image.header_addr); 
    fprintf(stderr,"  kernel_addr      :%p\n",image.kernel_addr); 
    fprintf(stderr,"  ramdisk_addr     :%p\n",image.ramdisk_addr); 
    fprintf(stderr,"  second_addr      :%p\n",image.second_addr); 
    
    fprintf(stderr,"\n header information:\n");
    fprintf(stderr,"  magic            :%8s\n",image.magic);
    fprintf(stderr,"  kernel_size      :%u\n",image.kernel_size);
    fprintf(stderr,"  kernel_phy_addr  :0x%08x\n",image.kernel_phy_addr);
    fprintf(stderr,"  ramdisk_size     :%u\n",image.ramdisk_size);
    fprintf(stderr,"  ramdisk_phy_addr :0x%08x\n",image.ramdisk_phy_addr);
    fprintf(stderr,"  second_size      :%u\n",image.second_size);
    fprintf(stderr,"  second_phy_addr  :0x%08x\n",image.second_phy_addr);
    fprintf(stderr,"  tags_phy_addr    :0x%08x\n",image.tags_phy_addr);
    fprintf(stderr,"  page_size        :%u\n",image.page_size);
    fprintf(stderr,"  name             :%s\n",image.name);
    fprintf(stderr,"  cmdline          :%s\n",image.cmdline);
    fprintf(stderr,"  id[0]            :%u\n",image.id[0]);
    fprintf(stderr,"  id[1]            :%u\n",image.id[1]);
    fprintf(stderr,"  id[2]            :%u\n",image.id[2]);
    fprintf(stderr,"  id[3]            :%u\n",image.id[3]);
    fprintf(stderr,"  id[4]            :%u\n",image.id[4]);
    fprintf(stderr,"  id[5]            :%u\n",image.id[5]);
    fprintf(stderr,"  id[6]            :%u\n",image.id[6]);
    fprintf(stderr,"  id[7]            :%u\n",image.id[7]);
    
    fprintf(stderr,"\n additonal information:\n");
    fprintf(stderr,"  total_size       :%08u\n",image.total_size);
    fprintf(stderr,"  header_size      :%08u\n\n",image.header_size);
    
    fprintf(stderr,"  header_offset    :%08u\n",image.header_offset);
    fprintf(stderr,"  header_padding   :%08u\n\n",image.header_padding);

    fprintf(stderr,"  kernel_offset    :%08u\n",image.kernel_offset);
    fprintf(stderr,"  kernel_padding   :%08u\n\n",image.kernel_padding);
    
    fprintf(stderr,"  ramdisk_offset   :%08u\n",image.ramdisk_offset);
    fprintf(stderr,"  ramdisk_padding  :%08u\n\n",image.ramdisk_padding);
    
    fprintf(stderr,"  second_offset    :%08d\n",image.second_offset);
    fprintf(stderr,"  second_padding   :%08d\n",image.second_padding);
    
    fprintf(stderr,"\nSaving header info to header.txt\n");
    write_boot_image_header_to_disk("header.txt",&image);
    
    fprintf(stderr,"\nSaving kernel image to kernel\n");
    FILE* fp  = fopen("kernel","w+b");
    fwrite(image.kernel_addr,image.kernel_size,1,fp);
    fclose(fp);
    
    fprintf(stderr,"Saving ramdisk to ramdisk.gz\n");
    fp  = fopen("ramdisk.gz","w+b");
    fwrite(image.ramdisk_addr,image.ramdisk_size,1,fp);
    fclose(fp);
    
    if(image.second_size > 0){
        fprintf(stderr,"Saving second to second\n");
        fp  = fopen("second","w+b");
        fwrite(image.second_addr,image.second_size,1,fp);
        fclose(fp);
    }else
        fprintf(stderr,"\n");
    
    
    free(image.start_addr);
    return 0;   
        
    
}