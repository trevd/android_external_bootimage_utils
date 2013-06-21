/*
 * main.c
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
#include <program.h>
#include <help.h>



/*int get_action(unsigned argc,char ** argv,program_options* options){

    D("argv[0]=%s  options->process_action %d \n",argv[0],options->process_action);
    switch(options->process_action){                
        //case ACTION_INFO:               process_info_action( --argc, ++argv, options);              break;              
        case ACTION_UPDATE:             process_update_action( --argc, ++argv, options);            break;          
        case ACTION_UPDATE_KERNEL:      break;
        case ACTION_UPDATE_RAMDISK:     break;
        case ACTION_UPDATE_PROPERTIES:  break;
        case ACTION_UPDATE_FILES:       break;      
        case ACTION_EXTRACT:            process_extract_action(--argc,++argv,options);          break;          
        case ACTION_EXTRACT_KERNEL:     break;  
        case ACTION_EXTRACT_RAMDISK:    break;  
        case ACTION_EXTRACT_HEADER:     break;  
        case ACTION_SCAN:               process_scan_action(--argc,++argv,options);             break;              
        case ACTION_COPY_KERNEL:        process_copy_kernel_action(--argc,++argv,options);      break;      
        case ACTION_COPY_RAMDISK:       process_copy_ramdisk_action(--argc,++argv,options);     break;
        case ACTION_CREATE_BOOT_IMAGE:  process_create_action(--argc,++argv,options);           break;
        case ACTION_CREATE_RAMDISK:     process_create_ramdisk_action(--argc,++argv,options);   break;
        case ACTION_CREATE_KERNEL:      break;
        case ACTION_INSTALL:            process_install_action(--argc,++argv,options);          break;
        default:                        break;
        }    
        
    return  0 ;

}*/
int main(int argc,char ** argv){

    
    // first call - switch on debugging
    if(getenv("BITDEBUG")) utils_debug = 1;
    //DC(argc,argv);
    //D("called with argc");

    // this should never happen but as we are
    // narrowing the variable we may aswell check
    if(argc < 0) return -1;
    
    unsigned uargc = argc ;
    
    // initalize our global actions, these include
    // program name,  the action to process and whether 
    // the program was called using on of the many multicall 
    // binary shortcut commands
     
    program_options options ;
    init_program_options(uargc,argv,&options);

    // only one argument past and that can only be
    // the filename, print some nice help to guide
    // the users on their way ;
    D("options.program_name_action[1]:%s argc:%d options.action_info=%p\n",options.program_name_action[1],argc,options.action_info);
    if((argc==1) || ( (argc==2) && options.program_name_action[1] != NULL)){
        print_help_message( &options ) ;    
        return 0;
    }


    D("options.debug:%d argc:%d\n",options.debug,argc);
    if(options.debug){
        unsigned  i = 0 ;
        for(i = 0 ; i < uargc ; i ++ ){
            D("argv[%d]=%s\n",i,argv[i]);
        }
    }
    
    // was this a multicall, if not then move the arg pointer along
    //D("action.multicall=%u %s\n",options.program_name_action[1],argv[2]);
     
    

        // not a multicall
        if(!strlcmp(argv[1],"--help") || !strlcmp(argv[1],"-h")  || !strlcmp(argv[1],"help")  ){
             //standard help requested 
             D("Printing Standard Help\n");
             print_help_message(    &options ) ;    
             return 0;
         }
    if(!options.program_name_action[1]==NULL){
        --argc ; ++argv ;
    }
    D("options.action_info:%p argc:%d\n",options.action_info,argc);
    (*options.action_info->processor) (--argc , ++argv, &options );
    
    //get_action(
        
    return 0;

}
