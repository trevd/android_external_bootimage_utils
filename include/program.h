/*
 * program.h
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


#ifndef _d40fb014_a3fc_11e2_ba4b_5404a601fa9d
#define _d40fb014_a3fc_11e2_ba4b_5404a601fa9d

unsigned print_program_title();
unsigned print_program_title_and_description();
unsigned print_program_error_processing(char* filename);
unsigned print_program_error_file_type_not_recognized(char * filename);
unsigned print_program_error_file_name_not_found(char * filename);
unsigned print_program_error_file_not_boot_image(char * filename);
unsigned print_program_error_file_write_boot_image(char * filename);
#endif
