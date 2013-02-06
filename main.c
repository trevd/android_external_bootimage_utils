

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "bootimg_utils.h"
#include "program.h"
#include "file.h"

#define BOOT_IMAGE_UTILITIES_TITLE "Android Boot Image Utilities "
#define BOOT_IMAGE_UTILITIES_VERSION "0.01b"
#define BOOT_IMAGE_UTILITIES_DESCRIPTION ""
#define BOOT_IMAGE_UTILITIES_PROGRAM_NAME "bootutils"
char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

int help(void){
	return 0;
}

int print_usage(void)
{
   fprintf(stderr,"You Sage\n");
   return 0;

}
enum command { UNPACK , PACK , LIST , ADD , REMOVE , UPDATE } ;

int main(int argc, char **argv)
{ 
	if(argc<3) return print_usage();
	
	int option_index =-1, option_return =-2, argument_count = argc, settings =0; 
	
	int compare_length = strlen(argv[1]) > 6 ? strlen(argv[1]) : 6;
	if(!strncmp(argv[1],"unpack",compare_length)){
		fprintf(stderr,"unpack option selected\n");	
		char stringopt[] = { "i:rl:xfpkcbsdho:a"};
		option_return = getopt_long (argc, argv,stringopt, unpack_long_options, &option_index);
		while (option_return != -1){
			switch(option_return){
				case 'a':{ params = (params | ALL ); break;	}
				case 'x':{ params = SET_RAMDISK_ARCHIVE ; break;	}
				case 'd':{ params = SET_RAMDISK_DIRECTORY ; break;}
				case 'p':{ params = SET_RAMDISK_CPIO	 ;  break; }
				case 'i':{
					 params = SET_IMAGE;
					 if(check_file_exists(optarg,CHECK_FAIL_EXIT))
						option_values.image = optarg;
					 break;
				}
				case 'r':{
					 params= SET_RAMDISK ;
					 option_values.ramdisk = malloc(PATH_MAX); 
					 if(!(argv[optind]) || argv[optind][0]=='-'){
						 strncpy(option_values.ramdisk,"default_ramdisk",strlen("default_ramdisk"));
						 break;
					 }				
					 if(argv[optind]){
						 memcpy(option_values.ramdisk,argv[optind],PATH_MAX);
					 }
					 break;
				}
				case 'k':{
					 params = SET_KERNEL ;
					 option_values.kernel= malloc(PATH_MAX); 
					 if(!(argv[optind]) || (argv[optind][0]=='-')){
						 memcpy(option_values.kernel,"default_kernel",strlen("default_kernel"));
						 break;
					 }				
					 if(argv[optind]){
						 memcpy(option_values.kernel,argv[optind],PATH_MAX);
					 }
					 break;
				}
				case 'c':{
					 params = SET_CMDLINE ;
					 option_values.cmdline = malloc(PATH_MAX); 
					 if(!(argv[optind]) || (argv[optind][0]=='-')){
						 strncpy(option_values.cmdline,"default_cmdline",strlen("default_cmdline")+1);
						 strncat(option_values.cmdline,DEFAULT_FILE_EXT,DEFAULT_FILE_EXT_LEN);
						 //fprintf(stderr,"%s\n",option_values.cmdline);
						 break;
					 }				
					 if(argv[optind]){
						 memcpy(option_values.cmdline,argv[optind],PATH_MAX);
					 }
					 //fprintf(stderr,"option_values.cmdline  %s %s\n",option_values.cmdline ,argv[optind]);
					 break;
				}
				case 'b':{
					 params = SET_BOARD ;
					 option_values.board = malloc(PATH_MAX); 
					 if(!(argv[optind]) || (argv[optind][0]=='-')){
						 memcpy(option_values.board,"default_board",strlen("default_board"));
						 break;
					 }				
					 if(argv[optind]){
						 memcpy(option_values.board,argv[optind],PATH_MAX);
					 }
					 break;
				}
				case 's':{
					 params = SET_SECOND ;
					 option_values.second= malloc(PATH_MAX); 
					 if(!(argv[optind]) || (argv[optind][0]=='-')){
						 memcpy(option_values.second,"default_second",strlen("default_second"));
						 break;
					 }				
					 if(argv[optind]){
						 memcpy(option_values.second,argv[optind],PATH_MAX);
					 }
					 break;
				}
				case 'h':{
					params = SET_HEADER ;
					option_values.header= malloc(PATH_MAX); 
					if(!(argv[optind]) || (argv[optind][0]=='-')){
						 memcpy(option_values.header,"default_header",strlen("default_header"));
						 break;
					}				
					if(argv[optind]){
					 memcpy(option_values.header,argv[optind],PATH_MAX);
					}
					break;
				}	
				case 'o':{
					 params = SET_OUTPUT ;
					 if(strlen(optarg)>0){
						option_values.output= malloc(PATH_MAX); 
						memcpy(option_values.output,optarg,PATH_MAX);
					 }
					 break;
				}
				default:	break;
			}
			option_return = getopt_long (argc, argv, stringopt, unpack_long_options, &option_index);
				
		} // end while	
		//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s\n",opterr,optind,option_index ,optopt,optarg);
		if(!optopt){
			if(!HAS_IMAGE){
				
				return print_usage();
			}
			if(!HAS_OUTPUT){
				option_values.output = malloc(PATH_MAX); 
				getcwd(option_values.output,PATH_MAX);
			}
			if(!HAS_RAMDISK){
				if(HAS_RAMDISK_ARCHIVE || HAS_RAMDISK_CPIO || HAS_RAMDISK_DIRECTORY)
				{
						option_values.ramdisk = malloc(PATH_MAX); 
						memcpy(option_values.ramdisk,"default_ramdisk",strlen("default_ramdisk"));
				}
			}
			//fprintf(stderr,"output : %s\n",option_values.output);	
			unpack_boot_image_file();
		}
		exit(0);
	}
	
	compare_length = strlen(argv[1]) > 4 ? strlen(argv[1]) : 4;
	if(!strncmp(argv[1],"pack",compare_length)){
		fprintf(stderr,"pack option selected\n");	 
		char stringopt[] = { "k:p:r:c:o:"};
		option_return = getopt_long (argc, argv,stringopt, pack_long_options, &option_index);
		while (option_return != -1){
			switch(option_return){
				case 'k':{
					params = SET_KERNEL;
					if(check_file_exists(optarg,CHECK_FAIL_EXIT))
						option_values.kernel = optarg;
						break;
					}
				case 'r':{
					params = SET_RAMDISK;
					if(check_directory_exists(optarg,CHECK_FAIL_OK)){
						params = SET_RAMDISK_DIRECTORY;
						option_values.ramdisk = optarg;
						break ;
					}
					if(check_file_exists(optarg,CHECK_FAIL_EXIT)){
						if(is_cpio_file(optarg)){
							params = SET_RAMDISK_CPIO;
							option_values.ramdisk = optarg;
							break;	 
						}
						if(is_gzip_file(optarg)){
							params = SET_RAMDISK_ARCHIVE;
							option_values.ramdisk = optarg;
							break;
						}
						perror("Not A Valid Ramdisk Type\n");
						exit(0);
					}
				}
				case 'p':{
					params = SET_PAGESIZE ;
					option_values.page_size = atoi(optarg);
					break;
				}
				case 'c':{
					params = SET_CMDLINE ;
					if(check_file_exists(optarg,CHECK_FAIL_OK))
						option_values.cmdline = optarg;
					break;
					}
				case 'o':{
					params = SET_OUTPUT ;
					 if(strlen(optarg)>0){ 
						option_values.output= optarg;
						//memcpy(option_values.output,optarg,PATH_MAX);
						fprintf(stderr,"output %s\n",option_values.output);
					 }
				 }
				default : break;
			}
			option_return = getopt_long (argc, argv,stringopt, pack_long_options, &option_index);
			//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s\n",opterr,optind,option_index ,optopt,optarg);		
		}
		if ( !HAS_OUTPUT ){
			fprintf(stderr,"output filename not set\n");
			exit(0);
		}
		if ( !HAS_RAMDISK){
			fprintf(stderr,"ramdisk not set\n");
			exit(0);
		}
		if ( !HAS_KERNEL){
			fprintf(stderr,"kernel not set\n");
			exit(0);
		}
		if ( !HAS_PAGESIZE ){
			option_values.page_size = DEFAULT_PAGE_SIZE ;// 2048 ; // DEFAULT_PAGE_SIZE; 
			//fprintf(stderr,"pagesize not set\n");
			
		}
		create_boot_image_file();
		exit(0);
	}
	
	compare_length = strlen(argv[1]) > 4 ? strlen(argv[1]) : 4;
	if(!strncmp(argv[1],"list",compare_length)){
		fprintf(stderr,"list option selected\n");	 
		char stringopt[] = { "i:rh"};
		option_return = getopt_long (argc, argv,stringopt, unpack_long_options, &option_index);
		while (option_return != -1){
			switch(option_return){
				case 'i':{
					 params = SET_IMAGE ;
					 if(check_file_exists(optarg,CHECK_FAIL_EXIT))
						option_values.image = optarg;
					 break;
				}
				default : break;
			}
			option_return = getopt_long (argc, argv,stringopt, unpack_long_options, &option_index);
			if(!HAS_IMAGE){
				return print_usage();
			}
				
			list_boot_image_info();
		}
				
	}
	compare_length = strlen(argv[1]) > 7 ? strlen(argv[1]) : 7;
	if(!strncmp(argv[1],"extract",compare_length)){
		
	fprintf(stderr,"extract option selected\n");	 
		char stringopt[] = { "i:f:o:"};
		option_return = getopt_long (argc, argv,stringopt, extract_long_options, &option_index);
		while (option_return != -1){
			switch(option_return){
				case 'i':{
					 params = SET_IMAGE ;
					 if(check_file_exists(optarg,CHECK_FAIL_EXIT))
						option_values.image = optarg;
					 break;
				}
				case 'f':{ 
						params = SET_FILENAME ;
						option_values.filename = optarg;
						option_values.filename_length = strlen(optarg);
						break;
					}
				case 'o':{ 
						params = SET_OUTPUT ;
						option_values.output = optarg;
						break;
					}
				default : break;
			}
			option_return = getopt_long (argc, argv,stringopt, extract_long_options, &option_index);
		}
		if(!HAS_FILENAME){
			return print_usage();
		}
		if(!HAS_IMAGE){
			return print_usage();
		}
		extract();
		
				
	}
	return 0;
}

