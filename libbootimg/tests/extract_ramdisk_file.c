// boot image loading test program.
// Load a boot
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <bootimage.h>
#include <utils.h>
int main(int argc, char** argv){

    char* filename; char* ramdisk_filename;
    boot_image image;
   
    fprintf(stderr,"\nBoot image ramdisk extract and unpack test\n");
    if(argc == 1){
        fprintf(stderr,"No filename : using default boot.img\n");
        filename="boot.img";
        fprintf(stderr,"No radisk_filename : using default init.rc\n");
        ramdisk_filename="init.rc";
    }else{
        fprintf(stderr,"filename %s\n",argv[1]);
        filename=argv[1];
    }
     if(argc == 2){
        fprintf(stderr,"No ramdisk filename : using default.prop\n");
        ramdisk_filename="default.prop";
    }else{
        if(!ramdisk_filename)
        ramdisk_filename = argv[2];
    }
    
    
    int return_value = load_boot_image(filename,&image);
    fprintf(stderr,"load_boot_image function returns %d %s\n",return_value,strerror(return_value));
    if(return_value != 0){
        if(image.start_addr != NULL  ) free(image.start_addr);
        return return_value;
    }
        
    ramdisk_image rimage; 
    return_value = load_ramdisk_image(image.ramdisk_addr,image.ramdisk_size,&rimage);
    fprintf(stderr,"load_ramdisk_image function returns %d %s\n",return_value,strerror(return_value));
    if(return_value != 0){
        if(rimage.start_addr != NULL  ) free(rimage.start_addr);
        return return_value;
    }
    fprintf(stderr,"\nramdisk_image struct values:\n");
    fprintf(stderr,"  start_addr       :%p\n",rimage.start_addr);
    fprintf(stderr,"  size             :%u\n",rimage.size);
    fprintf(stderr,"  compression_type :%s\n",str_ramdisk_compression(rimage.compression_type));
    fprintf(stderr,"  type             :%s\n",str_ramdisk_type(rimage.type));
    fprintf(stderr,"  entry_count      :%u\n",rimage.entry_count);
    fprintf(stderr,"  entries          :%p\n",rimage.entries);
    
    unsigned counter = 0;
    fprintf(stderr,"\nramdisk_entries struct values:\n");
    for(counter = 0 ; counter < rimage.entry_count ; counter++){
        // fprintf(stderr,"  rimage.entries[%02u].name         :%s\n",counter,rimage.entries[counter]->name_addr);
        if(!strlcmp(rimage.entries[counter]->name_addr,ramdisk_filename)){
            fprintf(stderr,"  rimage.entries[%02u]              :%p\n",counter,rimage.entries[counter]);
            fprintf(stderr,"  rimage.entries[%02u].start_addr   :%p\n",counter,rimage.entries[counter]->start_addr);
            fprintf(stderr,"  rimage.entries[%02u].name_addr    :%p\n",counter,rimage.entries[counter]->name_addr);
            fprintf(stderr,"  rimage.entries[%02u].data_addr    :%p\n",counter,rimage.entries[counter]->data_addr);
            fprintf(stderr,"  rimage.entries[%02u].next_addr    :%p\n",counter,rimage.entries[counter]->next_addr);
            fprintf(stderr,"  rimage.entries[%02u].mode         :%u\n",counter,rimage.entries[counter]->mode);
            fprintf(stderr,"  rimage.entries[%02u].name_size    :%u\n",counter,rimage.entries[counter]->name_size);
            fprintf(stderr,"  rimage.entries[%02u].name_padding :%u\n",counter,rimage.entries[counter]->name_padding);
            fprintf(stderr,"  rimage.entries[%02u].name         :%s\n",counter,rimage.entries[counter]->name_addr);
            fprintf(stderr,"  rimage.entries[%02u].data_size    :%u\n",counter,rimage.entries[counter]->data_size);
            fprintf(stderr,"  rimage.entries[%02u].data_padding :%u\n",counter,rimage.entries[counter]->data_padding);
            fprintf(stderr,"\ncontents of %s:\n\n",rimage.entries[counter]->name_addr);
            FILE *fp = fopen(ramdisk_filename,"wb");
            fwrite(rimage.entries[counter]->data_addr,rimage.entries[counter]->data_size,1,fp);
            fclose(fp);
            break;
            
        }
    }
    
    
    
    //fprintf(stderr,"  config_size      :%u\n",kimage.config_size);
    //fprintf(stderr,"  version          :%s\n",kimage.version);
    
    //fprintf(stderr,"\nSaving ramdisk entries disk\n");
    //save_ramdisk_entries_to_disk(&rimage,"ramdisk");
    
    //FILE* fp  = fopen("unpacked_kernel","w+b");
    //fwrite(kimage.start_addr,kimage.size,1,fp);
    //fclose(fp);
    
    //fprintf(stderr,"Saving kernel config.gz to config.gz\n");
    //fp  = fopen("config.gz","w+b");
    //fwrite(kimage.config_addr,kimage.config_size,1,fp);
    //fclose(fp);
    free(rimage.entries);
    free(rimage.start_addr);
    free(image.start_addr);
    return 0;   
        
    
}