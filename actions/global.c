/*
 * global.c
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


// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <string.h> 
#include <ctype.h>
// internal program headers
#include <program.h>
#include <utils.h>

// get_program_action - internal function called from the init_program_options function. 
// Figures out the main action to be performed - this is either based on the 
// executing file name or on the first argument after the executing  file
static unsigned get_program_action(unsigned argc,char ** argv, program_options* options){

    
        D("called with argc=%d argv=%p action=%p\n",argc,argv,options);
    
        //
        if(argc < 1 )  DEBUG_AND_RETURN_INT(-1)  ;

        if(!argv || !argv[0]) DEBUG_AND_RETURN_INT(-2);

        // exe name - bootimage-utils but argc = 1 
        if(!argv[1] && (strstr(options->program_name_action[0],"-utils"))) DEBUG_AND_RETURN_INT(-6); 
    
        if(!options) DEBUG_AND_RETURN_INT(-3);
         
        if(!options->program_name_action[0] || !strlen(options->program_name_action[0]) ) DEBUG_AND_RETURN_INT(-4) ;
       
        
        
        if ((strstr(options->program_name_action[0],"-utils") && argv[1]!=NULL)){  
                argv++;
                options->program_name_action[1] = argv[0];
        }
        D("options->program_name_action[0]=%s\n",options->program_name_action[0]);
        D("options->program_name_action[1]=%s\n",options->program_name_action[1]);
        int i = 0 ; 
        for ( ; i <= PROGRAM_ACTION_MAX_INDEX ; i++ ){
            D("argv[0]=%s pa[%d]=%s\n",argv[0],i,pa[i].long_name);
            if(strstr(argv[0],pa[i].long_name)){
                 options->program_action = &pa[i];
                 break ;           
            }else if (argv[0][0]==pa[i].short_name && 
                                argv[0][1]=='\0'){
                options->program_action = &pa[i];
            }
        }
        D("options->program_action=%p\n",options->program_action); 
        return 0;
}

// init_program_options - initialize our global structure. 
// This is called from the main function
unsigned init_program_options(unsigned argc,char ** argv, program_options * options){
    
    errno = 0;
    if(!options){
        errno = EINVAL ;
        return errno;
    }
    
    if(getenv("BITDEBUG")){
        options->debug = 1 ;
        // initialize debug printing for libbootimage
        utils_debug = 1 ;
        D("debug output enabled options->debug=%d\n", options->debug) ;
    }
    
    options->debug = 0;
    options->log_level = 0;
    options->verbose = 0 ;
    options->program_action = NULL ;
    options->program_name_action[0] = argv[0] ;
    options->program_name_action[1] = NULL ; 
    get_program_action(argc,argv,options);
        
    while(argc > 0){
        
        if(!strlcmp(argv[0],PROGRAM_OPTION_LONG_PREFIX""PROGRAM_OPTION_LONG_VERBOSE)){
                options->verbose = 1 ;
        }
        if(!strlcmp(argv[0],PROGRAM_OPTION_LONG_PREFIX""PROGRAM_OPTION_LONG_LOG_LEVEL)){
                long log_level = strtol(argv[1],NULL,10);
               
                options->log_level = errno = ERANGE ? 0 : log_level ;
                argc--; argv++;
        }
        if(!strlcmp(argv[0],PROGRAM_OPTION_LONG_PREFIX""PROGRAM_OPTION_LONG_LOG_FILE)){
                options->log_filename = argv[1] ;
                argc--; argv++;
        }
        
        if(!strlcmp(argv[0],PROGRAM_OPTION_LONG_PREFIX""PROGRAM_OPTION_LONG_DEBUG)){
                options->debug = 1 ;
        }
        if(!strlcmp(argv[0],PROGRAM_OPTION_LONG_PREFIX""PROGRAM_OPTION_LONG_INTERACTIVE)){
                options->interactive = 1 ;
        }
        if(!strlcmp(argv[0],PROGRAM_OPTION_LONG_PREFIX""PROGRAM_OPTION_LONG_VERSION)){
        }
        
        if(!strlcmp(argv[0],PROGRAM_OPTION_LONG_PREFIX""PROGRAM_OPTION_LONG_HELP)){
        }
        
        argc--; argv++;
    }
    
    
    //D("options->process_action=%d\n",options->process_action);
    D("options->process_action=%p\n",options->program_action);
    
      
    return 0;
}

//  only_program_optionss will check the argument list for parameters other than global actions
//  returns 0 if the list contains only global actions
//  returns 1 if the list contains extra parameters
unsigned only_program_options(unsigned argc,char ** argv,program_options* options){
    
    errno = 0;
    D("only_program_optionss: debug=%d\n",options->debug);
    int return_value = 0;
    while(argc > 0){
        D("only_program_optionss argv[0]=%s\n",argv[0]);
        if(!(argv[0],"--verbose") || !strlcmp(argv[0],"--log")){
           return_value = 0 ;
        }else{
            return_value =1 ;
            break;
        }
        argc--; argv++;
    }
    D("only_program_optionss returning %d\n",return_value);
    return return_value;
}


