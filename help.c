/*
 * help.c
 * 
 * Copyright 2013 Trevor Drake <trevd1234@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h> 
#include <utils.h>

#include <actions.h>
#include <program.h>
#include <help.h>



// print_program_action - Helper function to print
// the program_name Prints the action specified
static void print_program_name_action(program_options* options)
{
    fprintf(stderr," %s ",options->program_name_action[0]);
    if(options->program_name_action[1] != NULL ){
        fprintf(stderr,"%s ",options->program_name_action[1]);
    }
    
    return;
}

static int print_action_help_usage(program_options* options){
        action_info* ai = options->action_info;
        
        fprintf(stderr," Usage: %s ",options->program_name_action[0]);
        if(options->program_name_action[1] != NULL ){
                
                fprintf(stderr," [%c|%s] " ,ai->short_name ,ai->long_name)       ;
        }
        return 0;
    
}
static int print_action_help_header(program_options* options){
        
        
        print_program_title();
        print_program_name_action(options);
        fprintf(stderr,"- %s\n\n",options->action_info->description);
        print_action_help_usage(options);    
        
        return 0;
        
        
}

int print_info_action_help(program_options* options){
    
    D("options=%p\n",options);
   
        print_action_help_header(options);
    fprintf(stderr," <filename> [ <switches> ]\n\n");
    fprintf(stderr," filename: The file specified by <filename> must be one of the following types:\n");
    fprintf(stderr,"           Android Boot Image, Linux Kernel zImage, ASCII cpio archive,\n");
    fprintf(stderr,"           Compressed gzipped cpio archive. block device\n\n");
    fprintf(stderr," switches: \n\n");
    fprintf(stderr," -H, --headers      print the file header information\n");
    fprintf(stderr,"                    including device offset addresses, section sizes,\n");
    fprintf(stderr,"                    cmdline and board name\n");
    fprintf(stderr," -k, --kernel       prints information for the kernel contained within the boot image. \n");
    fprintf(stderr,"                    includes the compression type, kernel version\n");
    fprintf(stderr,"                    uncompressed size and config.gz size\n");
    fprintf(stderr," -r, --ramdisk,     prints information for the ramdisk contained within the boot image\n");
    fprintf(stderr,"                    includes compression type, uncompressed size, file count and ramdisk type\n");
    fprintf(stderr," -s, --second       prints information for the second section contained within the boot image\n");
    fprintf(stderr," -a, --additional   prints additional information for the boot image\n");
    fprintf(stderr,"                    includes section position and padding information\n\n");
    
    
    
    return 0; 
}
// print_install_action_help - prints the help for the install action
int print_install_action_help(program_options* options){
    
    // the help for install is a lot simpler than most because it does not
    // have a multicall version 
    print_program_title();
    fprintf(stderr," %s install - install multicall symlinks\n\n",options->program_name_action[0]);
    fprintf(stderr," Usage: %s install [ path ]\n\n",options->program_name_action[0]);
    fprintf(stderr," path: The location to create the symlinks\n\n");
    return 0;
}

int print_standard_help(program_options* options){
    
    print_program_title_and_description();
    
    fprintf(stderr," Usage: %s <action> [ ... ]\n\n",options->program_name_action[0]);
    fprintf(stderr," actions:\n\n");
    fprintf(stderr," install                        install actions as a standalone commands\n\n");
    
    fprintf(stderr," i, info                        prints detailed information for boot images and related components\n");
    fprintf(stderr," x, extract                     extract the components contained in boot images, ramdisks or kernels\n\n");
        
    fprintf(stderr," u, update                      update the components contained in boot images, ramdisks or kernels\n");
    fprintf(stderr," R, update-ramdisk              update the contents contained in boot images, ramdisks or kernels\n");
    fprintf(stderr," C, update-ramdisk-cpio         update the contents contained in boot images, ramdisks or kernels\n");
    fprintf(stderr," A, update-ramdisk-archive      update the contents contained in boot images, ramdisks or kernels\n");
    
    fprintf(stderr," s, scan                        extract the components contained in boot images, ramdisks or kernels\n");
    
    fprintf(stderr," c, create                      create an android boot image\n");
    fprintf(stderr," r, create-ramdisk              create a compressed rootfs ramdisk\n");
    fprintf(stderr,"\n");
    return 0;
    
}
int print_update_action_help(program_options* options){
        print_action_help_header(options);
        fprintf(stderr," <filename> [ <switches> ]\n\n");
        return 0;
}
int print_create_action_help(program_options* options){
        print_action_help_header(options);
        fprintf(stderr," <filename> [ <switches> ]\n\n");
        return 0;
}
int print_extract_action_help(program_options* options){
        print_action_help_header(options);
        fprintf(stderr," <filename> [ <switches> ]\n\n");
        return 0;
}
int print_help_message(program_options* options){
        
   D("options->program_name_action[0]=%s options->action_info=%p\n",options->program_name_action[0],options->action_info);
    
    if(options->action_info==NULL || options->action_info->help_processor==NULL ) {
        return print_standard_help(options); 
    }else{
        options->action_info->help_processor(options);
    }
        
    return  0 ; 
}
