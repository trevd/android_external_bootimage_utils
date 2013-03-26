// boot image loading test program.
// Load a boot

/* header information:
  magic            :ANDROID!
  kernel_size      :4857856
  kernel_phy_addr  :0x10008000
  ramdisk_size     :2669868
  ramdisk_phy_addr :0x11000000
  second_size      :0
  second_phy_addr  :0x10f00000
  tags_phy_addr    :0x10000100
  page_size        :2048
  name             :
  cmdline          :androidboot.test=1
  id[0]            :2498218225
  id[1]            :1376764871
  id[2]            :1794902697
  id[3]            :1962084668
  id[4]            :4038715985
  id[5]            :0
  id[6]            :0
  id[7]            :0

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <bootimage.h>
int main(int argc, char** argv){

    char* filename;
    char* kernel_filename;
    char* ramdisk_filename;
    boot_image image;
    
   
    fprintf(stderr,"\nBoot image saving test\n");
    if(argc == 1){
        fprintf(stderr,"No filename : using default boot.img\n");
        fprintf(stderr,"No kernel filename : using default kernel\n");
        fprintf(stderr,"No ramdisk filename : using default ramdisk.gz\n");
        filename="boot.img";
        ramdisk_filename="ramdisk.gz";
        kernel_filename="kernel";
                
    }else{
        fprintf(stderr,"filename %s\n",argv[1]);
        filename=argv[1];
    }
   
    set_boot_image_defaults(&image);
    
    // Stat the file for the size
    struct stat ksb;
	if (stat(kernel_filename, &ksb) == -1) {
		return -1;
    }
    
    unsigned kernel_size = ksb.st_size;
    char kmem[kernel_size];
    FILE* kernel_fp = fopen(kernel_filename,"r+b");
    if(kernel_fp) fread(kmem,kernel_size,1,kernel_fp);
    fprintf(stderr,"Kernel Size: %u\n",kernel_size);
    image.kernel_addr=&kmem;
    image.kernel_size=kernel_size;
    fclose(kernel_fp);
    
    // Stat the file for the size
    struct stat rsb;
	if (stat(ramdisk_filename, &rsb) == -1) {
		return -1;
    }
    unsigned ramdisk_size = rsb.st_size;
    char rmem[ramdisk_size];
    FILE* ramdisk_fp = fopen(ramdisk_filename,"r+b");
    if(ramdisk_fp) fread(rmem,ramdisk_size,1,ramdisk_fp);
    fprintf(stderr,"Kernel Size: %u\n",ramdisk_size);
    image.ramdisk_addr=&rmem;
    image.ramdisk_size=ramdisk_size;
    fclose(ramdisk_fp);
    
    set_boot_image_padding(&image);
    set_boot_image_content_hash(&image);
    set_boot_image_offsets(&image);
    
     
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
    
    write_boot_image(filename,&image);
    //free(image.start_addr);
    return 0;   
        
    
}