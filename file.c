#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "file.h"



int is_path_directory(char *dname){
	struct stat sb;
	if (stat(dname, &sb) == -1) {
		return 0;
	} 
	return S_ISDIR(sb.st_mode);
}
char *remove_file_extension(char* filename) {
    char *retstr;
    char *lastdot;
    if (filename == NULL)
         return NULL;
    if ((retstr = malloc (strlen (filename) + 1)) == NULL)
        return NULL;
    strcpy (retstr, filename);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
}
int check_file_exists(char *fname, int exitonfailure){
	
	struct stat sb;
	if (stat(fname, &sb) == -1) {
	   perror("bootimg-tools");
	   if(exitonfailure)
			exit(EXIT_FAILURE);
		else
			return 0;
	}
	if(!S_ISREG(sb.st_mode)){
		if(exitonfailure){
			switch (sb.st_mode & S_IFMT) {
				case S_IFBLK:  printf("block device\n");            break;
				case S_IFCHR:  printf("character device\n");        break;
				case S_IFDIR:  printf("directory\n");               break;
				case S_IFIFO:  printf("FIFO/pipe\n");               break;
				case S_IFLNK:  printf("symlink\n");                 break;
				case S_IFSOCK: printf("socket\n");                  break;
				default:       printf("unknown?\n");                break;
			}        
			exit(EXIT_FAILURE);
		}
	}
	return S_ISREG(sb.st_mode);
}
int check_directory_exists(char *fname, int exitonfailure){
	
	struct stat sb;
	if (stat(fname, &sb) == -1) {
	   perror("bootimg-tools");
	   if(exitonfailure)
			exit(EXIT_FAILURE);
		else
			return 0;
	}
	if(!S_ISDIR(sb.st_mode)){
		if(exitonfailure){
			switch (sb.st_mode & S_IFMT) {
				case S_IFBLK:  printf("block device\n");            break;
				case S_IFCHR:  printf("character device\n");        break;
				case S_IFREG:  printf("regular file\n");               break;
				case S_IFIFO:  printf("FIFO/pipe\n");               break;
				case S_IFLNK:  printf("symlink\n");                 break;
				case S_IFSOCK: printf("socket\n");                  break;
				default:       printf("unknown?\n");                break;
			}        
			exit(EXIT_FAILURE);
		}
	}
	return S_ISDIR(sb.st_mode);
}

int is_cpio_file(const char *filepath)
{
	FILE* fp = fopen(filepath, "rb");
	char magic[MAGIC_CPIO_SIZE];
	fread(&magic,MAGIC_CPIO_SIZE,1,fp);
	magic[MAGIC_CPIO_SIZE-1] = '\0';
	int result = strncmp(MAGIC_CPIO_ASCII,magic,MAGIC_CPIO_SIZE);
	//fprintf(stderr,"magic:%s magic:%s %d\n",MAGIC_CPIO_ASCII,magic,result);
	fclose(fp);
	return !result;
		
}
int is_gzip_file(const char *filepath)
{
	FILE* fp = fopen(filepath, "rb");
	int magic=0;
	if (fp != NULL){
		fread(&magic,sizeof(MAGIC_GZIP),1,fp);
		//fprintf(stderr,"magic:%d magic:%d \n",MAGIC_GZIP,magic);
		fclose(fp);
	}
	if(magic==MAGIC_GZIP) return 1;
	if(magic==MAGIC_GZIP_NONAME) return 1;
	return 0;
}

int write_single_line_to_file(const char *filepath, const char *output_buffer,unsigned size)
{
    FILE *output_file_fp = fopen(filepath, "wb");
    if (output_file_fp != NULL)
    {
        fwrite(output_buffer,size,1,output_file_fp);
        fclose(output_file_fp);
    }
    return 0;
}
int read_file_to_size(const char *filepath, unsigned size , unsigned char *output_buffer)
{
	
	FILE* fp = fopen(filepath, "rb");
	if (fp != NULL)
    {
		int bread = fread(output_buffer,size,1,fp);
		fclose(fp);
		return bread;
	}
	return 0;
	
}
unsigned char *load_file_from_offset(const char *filepath,int offset,unsigned long *file_size)
{
	
    unsigned char *data;
    int sz; int fd;
	
    data = 0;
   // fprintf(stderr,"load siz1e:%s  offset:%d\n",filepath,offset);
    FILE *fp = fopen(filepath, "rb");
    fd = fileno(fp);
    if(fd < 0) return 0;
	//fprintf(stderr,"load siz2:%s  offset:%d\n",filepath,offset);
    sz = lseek(fd, 0, SEEK_END);
    //fprintf(stderr,"load siz4:%s size:%d offset:%d\n",filepath,sz,offset);
    if(sz < 0) goto oops;
	rewind(fp);
	//fprintf(stderr,"load size:%s size:%d offset:%d\n",filepath,sz,offset);	
    if(lseek(fd, offset, SEEK_SET) != offset) goto oops;
	
	sz -= offset;

    data = (unsigned char*) malloc(sz);
    if(data == 0) goto oops;

    if(read(fd, data, sz) != sz) goto oops; 
    close(fd);

    if(file_size) *file_size = sz;
    return data;

oops:
    fclose(fp);
    if(data != 0) free(data);
    return 0;
}
long read_file( const char *fn, unsigned char *output,unsigned long *output_size){

	long  file_size; int fd;
	output = 0 ;
	 FILE *fp = fopen(fn, "rb");
    fd = fileno(fp);
    if(fd < 0) return 0;
    file_size = lseek(fd, 0, SEEK_END);
    if(file_size < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;
    if(read(fd, output, file_size) != file_size) goto oops;
    close(fd);

    if(output_size) *output_size = file_size;   
	return file_size;
oops:
    fclose(fp);
    return 0;
}
int write_to_file_mode(unsigned char *data_in, unsigned output_size,char * output_filename, mode_t mode){
	if(write_to_file(data_in,output_size,output_filename))
		return -1;
	chmod(output_filename,mode);
	return 0;
	
}
int write_to_file(unsigned char *data_in, unsigned output_size,char * output_filename){
	// Validate input
	
	FILE *output_file_fp;
	//fprintf(stderr,"Fullpath:%s\n",output_filename);
	if((output_file_fp = fopen(output_filename,"wb"))== NULL){
		printf("Cannot open %s file!\n",output_filename);
		goto oops;
	}
	if(output_file_fp)
	{
		fwrite(data_in,output_size,1,output_file_fp);
		fclose(output_file_fp);

	}
oops:
	return 0;
}
int dump_file(FILE* boot_image_fp,char * output_filename,unsigned offset,unsigned size){
	
	// assign some memory for the read
	void *output_buffer = malloc(size);

	// goto the offset of the file we are looking
	fseek(boot_image_fp, offset, SEEK_SET);
	// smash it into the output buffer
	fread(output_buffer,size,1,boot_image_fp);
	
	FILE *output_file_fp;
	//fprintf(stderr,"Fullpath:%s\n",fullpath);
	if((output_file_fp = fopen(output_filename,"wb"))== NULL){
		printf("Cannot open %s file!\n",output_filename);
		goto oops;
	}
	if(output_file_fp)
	{
		fwrite(output_buffer,size,1,output_file_fp);
		fclose(output_file_fp);
	}
oops:
	if(!output_buffer)
		free(output_buffer);
	return 0;
}

unsigned char *load_file(const char *fn, unsigned long *file_size)
{
    unsigned char *data;
    int sz; int fd;

    data = 0;
    FILE *fp = fopen(fn, "rb");
    fd = fileno(fp);
    if(fd < 0) return 0;

    sz = lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;

    data = (unsigned char*) malloc(sz);
    if(data == 0) goto oops;

    if(read(fd, data, sz) != sz) goto oops; 
    close(fd);

    if(file_size) *file_size = sz;
    return data;

oops:
    fclose(fp);
    if(data != 0) free(data);
    return 0;
}


