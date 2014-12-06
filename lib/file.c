#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

off_t fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    fprintf(stderr, "Cannot determine size of %s: %s\n",
            filename, strerror(errno));

	

    return -1;
}
