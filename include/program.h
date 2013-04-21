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
