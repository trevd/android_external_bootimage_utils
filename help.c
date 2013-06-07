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
#include <help.h>
#include <program.h>


int print_action_help_usage(global_action* gaction,char shortarg, char* longarg){
    
    fprintf(stderr," Usage: %s ",gaction->program_name);
    if(!gaction->multicall){
        fprintf(stderr," [%c|%s] ",shortarg,longarg);
    }
    return 0;
}
int print_info_action_help(global_action* gaction){
    
    
    print_program_title();
  
    fprintf(stderr," %s ",gaction->program_name);
    if(!gaction->multicall){
    fprintf(stderr,"info ");
        
    }
    fprintf(stderr,"- prints information for the specified boot image, kernel file or ramdisk\n\n");
    
    print_action_help_usage(gaction,'i',"info");
    
    fprintf(stderr," <filename> [ <switches> ]\n\n");
    fprintf(stderr," filename: The file specified by <filename> must be one of the following types:\n");
    fprintf(stderr,"           Android Boot Image, Linux Kernel zImage, ASCII cpio archive,\n");
    fprintf(stderr,"           Compressed gzipped cpio archive. block device\n\n");
    fprintf(stderr," switches: \n\n");
    fprintf(stderr," -h, --headers\n");
    fprintf(stderr," -k, --kernel\n");
    fprintf(stderr," -r, --ramdisk\n");
    fprintf(stderr," -s, --second\n");
    fprintf(stderr," -a, --additional\n\n");
    
    
    return 0; 
}
// print_install_action_help - prints the help for the install action
int print_install_action_help(global_action* gaction){
    
    // the help for install is a lot simpler than most because it does not
    // have a multicall version 
    print_program_title();
    fprintf(stderr," %s install - install multicall symlinks\n\n",gaction->program_name);
    fprintf(stderr," Usage: %s install [ path ]\n\n",gaction->program_name);
    fprintf(stderr," path: The location to create the symlinks\n\n");
    return 0;
}

int print_standard_help(global_action* gaction){
    
    print_program_title_and_description();
    
    fprintf(stderr," Usage: %s <action> [ ... ]\n\n",gaction->program_name);
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

int print_help_message(global_action* gaction){
        
    D("gaction->multicall=%d\n",gaction->multicall);
    D("gaction->process_action=%d\n",gaction->process_action);
    
    if(!gaction->multicall &&  gaction->process_action==ACTION_NONE ) {
        return print_standard_help(gaction);
         
    }
    
    switch(gaction->process_action){                
        case ACTION_INFO:               print_info_action_help(gaction); break;             
        case ACTION_UPDATE:             break;          
        case ACTION_UPDATE_KERNEL:      break;
        case ACTION_UPDATE_RAMDISK:     break;
        case ACTION_UPDATE_PROPERTIES:  break;
        case ACTION_UPDATE_FILES:       break;      
        case ACTION_EXTRACT:            break;          
        case ACTION_EXTRACT_KERNEL:     break;  
        case ACTION_EXTRACT_RAMDISK:    break;  
        case ACTION_EXTRACT_HEADER:     break;  
        case ACTION_SCAN:               break;              
        case ACTION_COPY_KERNEL:        break;      
        case ACTION_COPY_RAMDISK:       break;
        case ACTION_CREATE_BOOT_IMAGE:  break;
        case ACTION_CREATE_RAMDISK:     break;
        case ACTION_CREATE_KERNEL:      break;
        case ACTION_INSTALL:            print_install_action_help(gaction); break;
        default:                        break;
        }    
        
    return  0 ; 
}
