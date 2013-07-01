/*
 * program.h - part of the android bootimage_utils project
 * 
 * Copyright 2013 android <android@thelab>
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

/* Notes on Exectuable Naming 
 * 
 * The Boot image utils binary is a multicall binary. This means it's behaviour
 * will change depending on the executable filename.
 * 
 * Some flexiability is allowed as there should be no reason the program will fail
 * if the filename is changed.
 * 
 *
 * 
 */



#ifndef _d40fb014_a3fc_11e2_ba4b_5404a601fa9d
#define _d40fb014_a3fc_11e2_ba4b_5404a601fa9d

#include <ramdisk.h>
#include <help.h>



#define DEFAULT_RAMDISK_COMPRESSION RAMDISK_COMPRESSION_GZIP

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__) 
    #define DEFAULT_HEADER_NAME "header.txt"
#else
    #define DEFAULT_HEADER_NAME "header"
#endif

#define DEFAULT_RAMDISK_DIRECTORY_NAME          "ramdisk"
#define DEFAULT_RAMDISK_CPIO_NAME               "ramdisk.cpio"
#define DEFAULT_RAMDISK_IMAGE_NAME              "ramdisk.img"
#define DEFAULT_KERNEL_NAME                     "kernel"    
#define DEFAULT_SECOND_NAME                     "second"


#define PROGRAM_OPTION_LONG_PREFIX              "--"
#define PROGRAM_OPTION_LONG_VERBOSE             "verbose"
#define PROGRAM_OPTION_LONG_LOG_LEVEL           "loglevel"
#define PROGRAM_OPTION_LONG_LOG_FILE            "logfile"
#define PROGRAM_OPTION_LONG_DEBUG               "debug"
#define PROGRAM_OPTION_LONG_INTERACTIVE         "interactive"
#define PROGRAM_OPTION_LONG_VERSION             "version"
#define PROGRAM_OPTION_LONG_HELP                "help"
#define PROGRAM_OPTION_SHORT_PREFIX             "-"


#define ACTION_UNKNOWN              -1
#define ACTION_NONE                 0

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
#define ACTION_LIST_RAMDISK         18
#define ACTION_REMOVE_RAMDISK_FILE  19
#define ACTION_MOVE_RAMDISK_FILE    20
#define ACTION_COPY_RAMDISK_FILE    21
#define ACTION_CHMOD_RAMDISK_FILE   22
#define ACTION_CHOWN_RAMDISK_FILE   23
#define ACTION_MAKE_RAMDISK_DIR     24
#define ACTION_CREATE_RAMDISK_FILE  25


#define PROGRAM_ACTION_INFO_INDEX           0
#define PROGRAM_ACTION_INFO_LONG_NAME       "info"
#define PROGRAM_ACTION_INFO_SHORT_NAME      'i'
#define PROGRAM_ACTION_INFO_USAGE           "<filename> [ <switches> ]"
#define PROGRAM_ACTION_INFO_DESCRIPTION     "prints information for the specified boot image, kernel file or ramdisk"

#define PROGRAM_ACTION_EXTRACT_INDEX        1
#define PROGRAM_ACTION_EXTRACT_LONG_NAME    "extract"
#define PROGRAM_ACTION_EXTRACT_SHORT_NAME   'x'
#define PROGRAM_ACTION_EXTRACT_USAGE        "<filename> [ <switches> ]"
#define PROGRAM_ACTION_EXTRACT_DESCRIPTION  "extract the components contained in boot images, ramdisks or kernels"

#define PROGRAM_ACTION_UPDATE_INDEX 2
#define PROGRAM_ACTION_UPDATE_LONG_NAME     "update"
#define PROGRAM_ACTION_UPDATE_SHORT_NAME    'u'
#define PROGRAM_ACTION_UPDATE_USAGE "<filename> [ <switches> ]"
#define PROGRAM_ACTION_UPDATE_DESCRIPTION "extract the components contained in boot images, ramdisks or kernels"

#define PROGRAM_ACTION_CREATE_INDEX 3
#define PROGRAM_ACTION_CREATE_LONG_NAME "create"
#define PROGRAM_ACTION_CREATE_SHORT_NAME 'c'
#define PROGRAM_ACTION_CREATE_USAGE "<filename> [ <switches> ]"
#define PROGRAM_ACTION_CREATE_DESCRIPTION "create a android boot image or ramdisk"

#define PROGRAM_ACTION_MAX_INDEX PROGRAM_ACTION_CREATE_INDEX

// typedef for program_options - 
// defined here so members of program_action can use type safe arguments
typedef struct program_options program_options;

unsigned print_help_message(program_options* options);
unsigned print_update_action_help(program_options* options);
unsigned print_create_action_help(program_options* options);
unsigned print_extract_action_help(program_options* options);
unsigned print_info_action_help(program_options* options);
unsigned print_install_action_help(program_options* options);

// process_extract_action - implemented in actions/extract.c
int process_extract_action(unsigned argc,char ** argv,program_options* options);

// process_update_action - implemented in actions/update.c
int process_update_action(unsigned argc,char ** argv,program_options* options);

// process_info_action - implemented in actions/info.c
int process_info_action(unsigned argc,char ** argv,program_options* options);

// process_create_action - implemented in actions/create.c
int process_create_action(unsigned argc,char ** argv,program_options* options);

// process_create_ramdisk_action - implemented in actions/create_ramdisk.c
int process_create_ramdisk_action(unsigned argc,char ** argv,program_options* options);


//  
typedef struct program_action program_action;

static struct program_action {
        int index;
        char* long_name;
        char short_name;
        char* usage;
        char* description;
        int (*processor) (unsigned,char**,program_options*);
        unsigned (*help_processor) (program_options*);
        
} pa[] = { 
        { PROGRAM_ACTION_INFO_INDEX,        /* actioninfo[0].index */
          PROGRAM_ACTION_INFO_LONG_NAME,    /* actioninfo[0].long_name */
          PROGRAM_ACTION_INFO_SHORT_NAME,  PROGRAM_ACTION_INFO_USAGE,          PROGRAM_ACTION_INFO_DESCRIPTION,    process_info_action, print_info_action_help},
        { PROGRAM_ACTION_EXTRACT_INDEX,     PROGRAM_ACTION_EXTRACT_LONG_NAME,   PROGRAM_ACTION_EXTRACT_SHORT_NAME,  PROGRAM_ACTION_EXTRACT_USAGE,       PROGRAM_ACTION_EXTRACT_DESCRIPTION, process_extract_action,  print_extract_action_help },
        { PROGRAM_ACTION_UPDATE_INDEX,      PROGRAM_ACTION_UPDATE_LONG_NAME,    PROGRAM_ACTION_UPDATE_SHORT_NAME,   PROGRAM_ACTION_UPDATE_USAGE,        PROGRAM_ACTION_UPDATE_DESCRIPTION,  process_update_action, print_update_action_help },
        { PROGRAM_ACTION_CREATE_INDEX,      PROGRAM_ACTION_CREATE_LONG_NAME,    PROGRAM_ACTION_CREATE_SHORT_NAME,   PROGRAM_ACTION_CREATE_USAGE,        PROGRAM_ACTION_CREATE_DESCRIPTION,  process_create_action, print_create_action_help},
        
};

struct program_options{
    
        int             interactive ;           /* interactive mode will confirm each action */
        int             debug ;                 /* specifies whether debugging */
        int             log_level ;             /* the level of logging */
        char *          log_filename;           /* the filename to log */
        char *          option_filename;        /* the filename to log */
        /* an array of pointers to the arguments that contains the name
           a pointer to the program name and addionally a pointer to the
           argument contain the program action if it's not a multicall 
           execution 
        */
        char *          program_name_action[2]; 
        program_action    *program_action;                                             
        int             verbose ;
        int             print_help; 
        char*           current_working_directory;
};

// init_program_options - implemented in actions/global.c
unsigned init_program_options(unsigned argc,char ** argv,program_options* options);

// only_program_options - implemented in actions/global.c
unsigned only_program_options(unsigned argc,char ** argv,program_options* options);


// process_scan_action - implemented in actions/scan.c
int process_scan_action(unsigned argc,char ** argv,program_options* options);

// process_copy_kernel_action - implemented in actions/copy_kernel.c
int process_copy_kernel_action(unsigned argc,char ** argv,program_options* options);

// process_copy_ramdisk_action - implemented in actions/copy_ramdisk.c
int process_copy_ramdisk_action(unsigned argc,char ** argv,program_options* options);


// process_create_kernel_action - implemented in actions/extract.c
// int process_create_kernel_action(unsigned argc,char ** argv,program_options* options);


// process_update_action - implemented in actions/install.c
int process_install_action(unsigned argc,char ** argv,program_options* options);


unsigned print_program_title();
unsigned print_program_title_and_description();
unsigned print_program_error_processing(char* filename);


unsigned print_program_error_file_type_not_recognized(char * filename);
unsigned print_program_error_file_name_not_found(char * filename);
unsigned print_program_error_file_not_boot_image(char * filename);
unsigned print_program_error_file_write_boot_image(char * filename);
unsigned print_program_error_invalid_option(char arg);

#define DEBUG_AND_RETURN_INT(value) { D("returning %d\n",value); return value ; }

/*typedef enum primary_type {
        NONE = 0,
        UNKNOWN = 1, 
        BOOT_IMAGE = 2,
        KERNEL = 3,
        RAMDISK = 4,    
};
*/


#endif
