#ifndef _BOOT_IMAGE_UTILITIES_HELP_H_
#define _BOOT_IMAGE_UTILITIES_HELP_H_
#define BOOT_IMAGE_UTILITIES_TITLE "Android Boot Image Utilities"
#define BOOT_IMAGE_UTILITIES_VERSION "x.xx Alpha Release"
#define BOOT_IMAGE_UTILITIES_DESCRIPTION ""
#define BOOT_IMAGE_UTILITIES_PROGRAM_NAME "bootimage-utils"
#define BOOT_IMAGE_UTILITIES_FULL_TITLE "%s Version %s\n",BOOT_IMAGE_UTILITIES_TITLE,BOOT_IMAGE_UTILITIES_VERSION
#define PRINT_DOUBLE_LINE fprintf(stderr,"\n\n");
#define PRINT_SINGLE_LINE fprintf(stderr,"\n");
#define PRINT_DOUBLE_TAB fprintf(stderr,"\t");
#define PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE fprintf(stderr,BOOT_IMAGE_UTILITIES_FULL_TITLE);
#define PRINT_MAIN_USAGE fprintf(stderr,HELP_MAIN_USAGE);
#define HELP_MAIN_SUMMARY "bootimg-tools is an highly flexible utility for managing android boot images\n\n"

#define HELP_MAIN_USAGE "\
Usage:  bootimg-tools [actions] <switches>\n\
Actions:\n\
unpack		unpack a boot image into it's constituent parts\n\
pack		pack seperate files into a boot image\n\
list		print boot image header details or ramdisk fileinfo\n\
extract   	extract a single file from a boot image ramdisk\n\
update    	update the boot image ramdisk and header property\n\
help		print the help information for the specificed action\n\n\
See bootimg-tools help <action> for detailed information\n"

#define HELP_EXTRACT_MAIN "\
Unpack: unpacks a boot image into it's constituent parts\n\
Usage:  bootimg-tools extract <boot image file> <switches>\n\
Switches:\n\
    -h, --header [filename]           Extract the boot image header information to [filename] leave filename empty to use\n\
                                      default filename ( default=header)\n\
    -c, --cmdline [filename]          Extract the boot image kernel cmdline to [filename]\n\
                                      leave filename empty to use default filename ( default=cmdline )\n\
    -p, --pagesize [filename]         Extract the value of the boot image pagesize to [filename]\n\
                                      leave filename empty to use default filename ( default=pagesize )\n\
    -k, --kernel [filename]           Extract kernel image to [filename] leave filename\n\
                                      empty to use default filename ( defualt=kernel )\n\
    -d, --ramdisk-directory [dir]     Fully Extract the ramdisk to [dir] leave dir empty to use\n\
                                      default directory ( default=root )\n\
    -x, --ramdisk-archive [filename]  Extract the compressed ramdisk to [filename] leave filename\n\
                                      empty to use default ( default=initramfs.cpio.<type> ), when\n\
                                      using the defaults the <type> will be determined by the file magic\n\
                                      common type are lzop (.lzo) and gzip (.gz)\n\
    -f, --files < file1,file2,... >   Extract files specified in the comma seperated from the ramdisk\n\
                                      if the file cannot be found the entry is ignored processed\n\n\
Notes: <boot image file>  is required and must be a valid android boot image\n\
       --kernel, --header, --cmdline and --pagesize are optional --header includes the cmdline and pagesize info\n\
       --ramdisk-archive and --ramdisk-directory are optional\n\
  All optional commands can be used in any combination required.\n\n"
                                      
#define HELP_PACK_MAIN "\
Unpack: creates a boot image from constituent parts\n\
Usage:  	bootimg-tools pack <bootimage file> <switches>\n\
Switches:\n\
    -h, --header [filename]           Use cmdline and pagesize information contained in [filename]\n\
                                      leave filename empty to use default filename ( default=header)\n\
    -c, --cmdline <filename | text >  Use cmdline information contained in [filename] leave filename\n\
                                      empty to use default filename ( default=cmdline ) or specify a\n\
                                      cmdline text directly using text enclosed in single quotes ''\n\
    -p, --pagesize <filename | text > Use pagesize information contained in [filename] leave filename\n\
                                      empty to use default filename ( default=pagesize ) or specify a\n\
                                      pagesize value directly using text\n\
    -k, --kernel [filename]           Use kernel image in [filename] leave filename\n\
                                      empty to use default filename ( defualt=kernel )\n\
    -d, --ramdisk-directory [dir]     Use [dir] as the ramdisk root leave dir empty to use\n\
                                      default directory ( default=root ) \n\
    -x, --ramdisk-archive [filename]  Use [filename] as compressed ramdisk leave filename\n\
                                      empty to use default ( default=initramfs.cpio.gz )\n\n\
Notes: <boot image file>  is required and must be a valid android boot image\n\
       --kernel, --header, --cmdline and --pagesize are optional --header includes the cmdline and pagesize info\n\
       --ramdisk-archive and --ramdisk-directory are require an exclusive to one another\n\
  All optional commands can be used in any combination required.\n\n"

enum HELP_ME { HELP_NONE,HELP_MAIN, HELP_UNPACK , HELP_PACK, HELP_EXTRACT, HELP_UPDATE } ;	

static int print_usage(){
	
	PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE
	PRINT_MAIN_USAGE
	return 0;
}

static  void print_question(char *action){
	PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE	
	fprintf(stderr,"%s what?\n",action);
	PRINT_MAIN_USAGE
	exit(0);
}	

static void check_for_help_call(int argc,char ** argv){
	enum HELP_ME help_me = HELP_NONE;
	
	int compare_length = strlen(argv[1]) > 2 ? strlen(argv[1]) : 2;
	if(!strncmp(argv[1],"-h",compare_length)) help_me= HELP_MAIN;
		
	compare_length = strlen(argv[1]) > 6 ? strlen(argv[1]) : 6;
	if(!strncmp(argv[1],"--help",compare_length)) help_me= HELP_MAIN;
		
	compare_length = strlen(argv[1]) > 4 ? strlen(argv[1]) : 4;
	if(!strncmp(argv[1],"help",compare_length)) help_me= HELP_MAIN;
		//fprintf(stderr,"H:%d %d\n",HELP_UNPACK,argc);
		
		
	if((help_me==HELP_MAIN) && (argc>2)){
	//	fprintf(stderr,"HM:%d %s\n",help_me,argv[2]);
		compare_length = strlen(argv[2]) > 6 ? strlen(argv[2]) : 6;
		if(!strncmp(argv[2],"unpack",compare_length)){
			help_me=HELP_UNPACK;
		}
		compare_length = strlen(argv[2]) > 4 ? strlen(argv[2]) : 4;
		if(!strncmp(argv[2],"pack",compare_length)){
			help_me=HELP_PACK;
		}
	}
	return ;
	
}
static int help_main(){
	PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE
	PRINT_MAIN_USAGE
	exit(0);
}
static int help_create(){exit(0);}
static int help_extract(){ 
	PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE
	fprintf(stderr,HELP_EXTRACT_MAIN); 
	exit(0);
}
static int help_remove(){exit(0);}
static int help_add(){exit(0);}
static int help_list(){exit(0);}
static int help_update(){exit(0);}
#endif 
