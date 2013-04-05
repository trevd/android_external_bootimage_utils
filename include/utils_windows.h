#ifndef _a837e23a_9629_11e2_9d73_5404a601fa9d
#define _a837e23a_9629_11e2_9d73_5404a601fa9d

#include <windows.h>

#define lstat stat

#define S_IFLNK	 0120000
#define S_IFSOCK 0140000
#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_IWGRP 00020
#define CONVERT_LINE_ENDINGS 1==1
#define EOL WINDOWS_EOL

#endif
