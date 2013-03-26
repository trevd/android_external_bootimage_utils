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
#define PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE { static int title_printed; if(!title_printed){fprintf(stderr,BOOT_IMAGE_UTILITIES_FULL_TITLE);title_printed=1; } }
#define PRINT_MAIN_USAGE fprintf(stderr,HELP_MAIN_USAGE);
#define PRINT_ERROR_PREFIX fprintf(stderr,"Error : "); 
#define HELP_MAIN_SUMMARY "bootimg-tools is an highly flexible utility for managing android boot images\n\n"


#define HELP_ERROR_NO_BOOT "boot image \"%s\" file not found"
#define HELP_ERROR_NO_KERNEL "kernel file \"%s\" file not found"
#define HELP_ERROR_FILE_SIZE "unexpected file size"
#define HELP_ERROR_BOOT_MAGIC "cannot find android boot magic in file \"%s\""
#define HELP_ERROR_FILE_SHORT_READ "cannot read full file contents"
#define HELP_ERROR_ARG_FILE_LIST_EMPTY "no files specified in filelist switch"
#define HELP_ERROR_ARG_FILE_LIST_FILE_NOT_FOUND_DISK "filelist entry %s not found"
#define HELP_ERROR_ARG_INVALID_LONG "invalid argument value \"%s\" for switch \"%s\""
#define HELP_ERROR_ARG_INVALID_SHORT "invalid argument value \"%s\" for switch \'%c\'"
#define HELP_ERROR_ARG_FILE_NOT_FOUND_LONG "file \"%s\" not found for switch \'%s\'"
#define HELP_ERROR_ARG_FILE_NOT_FOUND_SHORT "file \"%s\" not found for switch \'%c\'"
#define HELP_ERROR_ARG_FILE_NOT_FOUND_RAMDISK "file \"%s\" not found in ramdisk"
#define EXTRACT_TYPE_KERNEL "kernel"
#define EXTRACT_TYPE_HEADER "header"
#define EXTRACT_TYPE_CPIO "ramdisk cpio"
#define EXTRACT_TYPE_ARCHIVE "ramdisk archive"
#define EXTRACT_TYPE_DIRECTORY "ramdisk directory"
#define EXTRACT_MESSAGE_FILE_NAMES   "Extracting %s to \"%s\"\n"
#define EXTRACT_MESSAGE_FILE_NAMES_SIZES  "Extracting %s to \"%s\" size %u\n"
//#define PRINT_EXTRACT_MESSAGE(const char *format,...)  print_message(format, ...) ;


#define HELP_ERROR_IMAGE_RAMDISK_SIZE_ZERO "\
the boot image header reports the ramdisk size as zero\n\n"



#define HELP_MAIN_USAGE "\
Usage:  bootimg-tools [actions] <switches>\n\
Actions:\n\
x, extract  extract boot image parts\n\
p, pack		pack files into a boot image\n\
l, list		print boot image information\n\
a, add	 	add ramdisk files or boot image parts\n\
r, remove 	remove ramdisk files or boot image parts\n\
u, update  	update ramdisk files or boot image\n\
h, help		print the help information for the specificed action\n\n\
See bootimg-tools help <action> for detailed information\n"

#define HELP_EXTRACT_MAIN "\
extract: extract boot image parts\n\
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
    -f, --files <file1> <file2> ...   Extract files specified in the the file list from the ramdisk\n\
        --no-path                     if the file cannot be found the entry is ignored processed\n\
									  Add the --no-path switch to extract the file to the current directory\n\
									  regardless of the ramdisk path\n\n\
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
	fprintf(stderr,"Help Main\n");
	PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE
	PRINT_MAIN_USAGE
	exit(0);
}

static int help_identify(){exit(0);}
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

static void help_error_header()
{
	PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE
	PRINT_SINGLE_LINE
	PRINT_ERROR_PREFIX
	
}
static int help_error_file_size(){
	help_error_header();
	fprintf(stderr,HELP_ERROR_FILE_SIZE);
	PRINT_DOUBLE_LINE
	exit(0);
}
static int help_error_no_boot(char*filename){
	help_error_header();
	fprintf(stderr,HELP_ERROR_NO_BOOT,filename);
	PRINT_DOUBLE_LINE
	exit(0);
}
static int help_error_boot_magic(char*filename){
	help_error_header();
	fprintf(stderr,HELP_ERROR_BOOT_MAGIC,filename);
	PRINT_DOUBLE_LINE
	exit(0);
}
static int help_error_arg_file_list(char **filelist){
	free(filelist);
	help_error_header();
	fprintf(stderr,HELP_ERROR_ARG_FILE_LIST_EMPTY);
	PRINT_DOUBLE_LINE
	exit(0);
}
static int help_error_arg_invalid(char **switch_args,const char* long_name){
	help_error_header();
	if(switch_args[0][0]=='-'){
		if(switch_args[0][1]=='-')
			fprintf(stderr,HELP_ERROR_ARG_INVALID_LONG,switch_args[1],long_name);
		else 
			fprintf(stderr,HELP_ERROR_ARG_INVALID_SHORT,switch_args[1],switch_args[0][1]);
	}
	PRINT_DOUBLE_LINE
	exit(0);
}
static int help_error_arg_file(char **switch_args,const char* long_name){
	help_error_header();
	if(switch_args[0][0]=='-'){
		if(switch_args[0][1]=='-')
			fprintf(stderr,HELP_ERROR_ARG_FILE_NOT_FOUND_LONG,switch_args[1],long_name);
		else 
			fprintf(stderr,HELP_ERROR_ARG_FILE_NOT_FOUND_SHORT,switch_args[1],switch_args[0][1]);
	}
	PRINT_DOUBLE_LINE
	exit(0);
}
static int help_error_ramdisk_file(char** filelist){
	
	help_error_header();
	fprintf(stderr,HELP_ERROR_ARG_FILE_NOT_FOUND_RAMDISK,filelist[0]);
	PRINT_DOUBLE_LINE
	free(filelist);
	exit(0);

}
static int help_error_image_ramdisk_zero(){
	
	help_error_header();
	fprintf(stderr,HELP_ERROR_IMAGE_RAMDISK_SIZE_ZERO);
	PRINT_DOUBLE_LINE
	exit(0);
	
}
static int print_message(const char *format, ...)
{
	
	PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE
	int result;
	char *str = NULL;
	FILE* file;
	va_list args;
	va_start(args, format);

	result = vasprintf(&str, format, args);
	if(result == -1)
		return 0;
	va_end(args);
	fprintf(stderr,str,NULL);
	//free(str);

	return 1;
}
#endif 
