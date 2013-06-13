/*
 * file.c
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
#include <stdio.h> 
#include <string.h> 
#include <kernel.h>
#include <ramdisk.h>
#include <bootimg.h>
#include <file.h>
#include <errno.h>
#include <utils.h>
#include <compression.h>


#define FILE_MAGIC_NULL NULL
#define FILE_MAGIC_NULL_SIZE 0
#define FILE_TYPE_MAX 255

typedef struct known_file_type known_file_type ;

struct known_file_type {
    
    unsigned id ;
    char * magic;
    unsigned magic_size;
    char* description ; 
    unsigned description_size ; 

} ;

//known_file_type known_file_types[] = { 
    
