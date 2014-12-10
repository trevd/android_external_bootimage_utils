
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <api/bootimage.h>

void list_archive(const char *name)
{

	struct bootimage* bi = NULL;
	printf("bi %p\n",bi);
	bi = bootimage_initialize();
	int ret = bootimage_file_read(bi,name);
	if ( ret == -1 ){
		printf("bootimage_file_read failed err=%d\n",errno);
	}else{
		ret = bootimage_extract_header_block(bi,NULL);
		if ( ret == -1 ){
			printf("bootimage_extract_header failed err=%d\n",errno);
		}
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

int main(int argc,char** argv)
{

list_archive(argv[1]);

 return 0 ;
}
