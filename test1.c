
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <api.h>

void list_archive(const char *name)
{
  
	struct bootimage* bi = NULL; 
	printf("bi %p\n",bi);
	bi = bootimage_read_initialize();
	printf("bi %p\n",bi);
	//int ret = bootimage_read_filename_(bi,name);
	int ret = bootimage_extract_ramdisk_entry_from_file(bi,name,"init.rc"); 
	//ret = bootimage_read_kernel(bi); 
	printf("bi %p ret=%d errno=%d\n",bi,ret,errno);
	bootimage_read_free(&bi);

	printf("bi %p\n",bi);
	bootimage_read_free(&bi);
	
	
	return ;

}

int main(int argc,char** argv)
{

list_archive(argv[1]);

 return 0 ; 
}
