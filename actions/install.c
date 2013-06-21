/*
 * install.c
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
 
// standard program headers
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
 
// internal program headers
#include <actions.h>
#include <program.h> 
#include <utils.h> 
#include <help.h> 

int process_install_action(unsigned argc,char ** argv,program_options* options){
    
    
    char exe_path[PATH_MAX];
    char install_directory[PATH_MAX];
        
    if(argc == 0){
        // if no install directory is specified then we will use the current working directory.
        getcwd(install_directory,PATH_MAX);
        D("installing symlinks to current working directory %s\n",install_directory);
    }else{
        errno = 0;
        D("installing symlinks to %s\n",argv[0]);
        mkdir_and_parents(argv[0],0755);
        if(errno > 0){
            print_program_title();
            fprintf(stderr," install - error %d - %s\n",errno,strerror(errno));
            return 0;
            
        }
        strcpy(install_directory,argv[0]);
        D("mkdir_and_parents errno=%d\n",errno);
    }
    
    
    D("argc=%d\n",argc);
    
    get_exe_path(exe_path,PATH_MAX);
    chdir(install_directory);
    /*int install_directory_length = strlen(install_directory)-1;
    
    if(install_directory[install_directory_length]!='/'){
        install_directory_length += 1;
        install_directory[install_directory_length]='/';
        install_directory_length += 1;
        install_directory[install_directory_length]='\0';
    }
    D("install_directory=%s install_directory[%d]=%c\n",install_directory,install_directory_length-1,install_directory[install_directory_length-1]);*/
    chdir(install_directory);
    //strcat(install_directory,"bootimage-extract")
    print_program_title();
    fprintf(stderr,"Installing links to %s\n",install_directory);
    symlink(exe_path,"bootimage-extract");
    symlink(exe_path,"bootimage-info");
    symlink(exe_path,"bootimage-create");
    symlink(exe_path,"bootimage-update");
    symlink(exe_path,"bootimage-copy-ramdisk");
    symlink(exe_path,"bootimage-copy-kernel");
    D("exe_path=%s\n",exe_path);
    
    
    return 0;
    
}
