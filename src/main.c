#include <stdlib.h>
#include <stdio.h>
#include <biutils.h>

int main(int argc , char** argv){

	/* Sanity Check argc first. Never trust user input
	   Not even in main */
	if ( argc <= 0 ){
		return 0 ;
	}
	if ( argc == 1 ){
		fprintf(stdout,"Help\n");
	}
	struct bootimage_utils* biu = bootimage_utils_initialize();
	bootimage_utils_file_read(biu,argv[1]);
	bootimage_utils_free(&biu);

	fprintf(stdout,"biu=%p\n",biu);

	return 0 ;

}
