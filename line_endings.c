#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "file.h"
#include "line_endings.h"
#define BUFSIZE (1024*8)


typedef struct Node {
    struct Node *next;
    char buf[BUFSIZE*2+3];
} Node;


void dos_to_unix(char* buf)
{
    char* p = buf;
    char* q = buf;
    while (*p) {
        if (p[0] == '\r' && p[1] == '\n') {
            // dos
            *q = '\n';
            p += 2;
            q += 1;
        }
        else if (p[0] == '\r') {
            // old mac
            *q = '\n';
            p += 1;
            q += 1;
        }
        else {
            *q = *p;
            p += 1;
            q += 1;
        }
    }
    *q = '\0';
}

void unix_to_dos(char* buf2, const char* buf)
{
    const char* p = buf;
    char* q = buf2;
    while (*p) {
        if (*p == '\n') {
            q[0] = '\r';
            q[1] = '\n';
            q += 2;
            p += 1;
        } else {
            *q = *p;
            p += 1;
            q += 1;
        }
    }
    *q = '\0';
}

