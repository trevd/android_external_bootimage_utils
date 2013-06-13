/*
 * actions.h
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

#ifndef _ac16cc24_978f_11e2_8e27_5404a601fa9d
#define _ac16cc24_978f_11e2_8e27_5404a601fa9d

#define ACTION_UNKNOWN              -1
#define ACTION_NONE                 0
#define ACTION_INFO                 1
#define ACTION_UPDATE               2
#define ACTION_UPDATE_KERNEL        3
#define ACTION_UPDATE_RAMDISK       4
#define ACTION_UPDATE_PROPERTIES    5
#define ACTION_UPDATE_FILES         6
#define ACTION_EXTRACT              7
#define ACTION_EXTRACT_KERNEL       8
#define ACTION_EXTRACT_RAMDISK      9
#define ACTION_EXTRACT_HEADER       10
#define ACTION_SCAN                 11
#define ACTION_COPY_KERNEL          12
#define ACTION_COPY_RAMDISK         13
#define ACTION_CREATE_BOOT_IMAGE    14
#define ACTION_CREATE_RAMDISK       15
#define ACTION_CREATE_KERNEL        16
#define ACTION_INSTALL              17


typedef struct global_action global_action;

struct global_action{
    
    int     debug ;
    int     log ;
    char *  log_filename;
    char *  program_name;
    int     verbose ;
    int     multicall ;
    int     process_action;
    int     print_help; 
    int     current_working_directory;
};

// init_global_action - implemented in actions/global.c
int init_global_actions(unsigned argc,char ** argv,global_action* action);

// only_global_actions - implemented in actions/global.c
int only_global_actions(unsigned argc,char ** argv,global_action* action);


// process_extract_action - implemented in actions/extract.c
int process_extract_action(unsigned argc,char ** argv,global_action* gaction);

// process_update_action - implemented in actions/update.c
int process_update_action(unsigned argc,char ** argv,global_action* gaction);

// process_info_action - implemented in actions/info.c
int process_info_action(unsigned argc,char ** argv,global_action* gaction);

// process_scan_action - implemented in actions/scan.c
int process_scan_action(unsigned argc,char ** argv,global_action* gaction);

// process_copy_kernel_action - implemented in actions/copy_kernel.c
int process_copy_kernel_action(unsigned argc,char ** argv,global_action* gaction);

// process_copy_ramdisk_action - implemented in actions/copy_ramdisk.c
int process_copy_ramdisk_action(unsigned argc,char ** argv,global_action* gaction);

// process_create_action - implemented in actions/create.c
int process_create_action(unsigned argc,char ** argv,global_action* gaction);

// process_create_kernel_action - implemented in actions/extract.c
// int process_create_kernel_action(unsigned argc,char ** argv,global_action* gaction);

// process_create_ramdisk_action - implemented in actions/create_ramdisk.c
int process_create_ramdisk_action(unsigned argc,char ** argv,global_action* gaction);

// process_update_action - implemented in actions/install.c
int process_install_action(unsigned argc,char ** argv,global_action* gaction);



#endif
