
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <api/bootimage.h>
#include <api/bootimage_extract.h>
#include <api/bootimage_file_print.h>
#include <api/bootimage_print.h>

void extract_ramdisk(struct bootimage* bi)
{
	int ret = bootimage_extract_ramdisk_archive(bi,NULL);
	if ( ret == -1 ){
		printf("bootimage_extract_ramdisk failed err=%d\n",errno);
	}

}

void extract_ramdisk_directory(struct bootimage* bi)
{
	int ret = bootimage_extract_ramdisk(bi,NULL);
	if ( ret == -1 ){
		printf("bootimage_extract_ramdisk failed err=%d\n",errno);
	}

}

void extract_kernel(struct bootimage* bi)
{
	int ret = bootimage_extract_kernel(bi,NULL);
	if ( ret == -1 ){
		printf("bootimage_extract_kernel failed err=%d\n",errno);
	}

}
void extract_header_block(struct bootimage* bi)
{

	int ret = bootimage_extract_header_block(bi,NULL);
	if ( ret == -1 ){
		printf("bootimage_extract_header_block failed err=%d\n",errno);
	}
}

void list_archive(const char *name)
{

	struct bootimage* bi = NULL;
	printf("bi %p\n",bi);
	bi = bootimage_initialize();
	int ret = bootimage_file_read(bi,name);
	if ( ret == -1 ){
		printf("bootimage_file_read failed err=%d\n",errno);
	}else{

		extract_ramdisk(bi);
		extract_kernel(bi);
		extract_header_block(bi);
		extract_ramdisk_directory(bi);
	}
	bootimage_free(&bi);
	printf("bi %p\n",bi);
	//int ret = bootimage_read_filename_(bi,name);
	//int ret = bootimage_extract_ramdisk_entry_from_file(bi,name,"init.rc");
	//ret = bootimage_read_kernel(bi);
	/*printf("bi %p ret=%d errno=%d\n",bi,ret,errno);
	bootimage_read_free(&bi);

	printf("bi %p\n",bi);

	*/

	return ;

}
int print_tests(struct bootimage* bi )
{
	printf("Running bootimage_print_header\n");
	bootimage_print_header(bi);
	printf("Running bootimage_print_kernel\n");
	bootimage_print_kernel(bi);
	printf("Running bootimage_print_kernel_version\n");
	bootimage_print_kernel_version(bi);
	return 0 ;
}
int extract_tests(struct bootimage* bi )
{
	printf("Running bootimage_extract_uncompressed_kernel\n");
	bootimage_extract_uncompressed_kernel(bi,NULL);
	bootimage_extract_ramdisk(bi,NULL);
	return 0 ;
}


int main(int  argc  ,char** argv)
{
//int ret = bootimage_file_print_header(argv[1]) ;
//if ( ret == -1 ){
//		printf("bootimage_file_read failed err=%d\n",errno);
//}
 char cwd[1024] = "out/out.rc";
   //if (getcwd(cwd, sizeof(cwd)) != NULL)
   //    fprintf(stdout, "Current working dir: %s\n", cwd);
   //else
    //   perror("getcwd() error");

	char* dname = dirname(cwd);
	printf("dirname=%s\n", dname);


	struct bootimage* bi = NULL;
	//printf("bi %p\n",bi);
	bi = bootimage_initialize();
	int ret = bootimage_file_read(bi,argv[1]);
	//bootimage_extract_ramdisk_entry(bi,"init.rc","out.rc");
	//bootimage_extract_ramdisk_entry(bi,"init.rc","out/out.rc");
	printf("dirname=%s\n", dname);

	bootimage_extract_ramdisk(bi,NULL);
	//bootimage_extract_uncompressed_kernel(bi,NULL);
	//bootimage_extract_kernel(bi,NULL);
	//extract_tests(bi);
	//print_tests(bi) ;


	//printf("bi %p\n",bi);
	//bootimage_extract_ramdisk(bi,NULL);
	bootimage_free(&bi);
/*list_archive(argv[1]);*/

 return 0 ;
}
