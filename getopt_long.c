/* getopt_long and getopt_long_only entry points for GNU getopt.
   Copyright (C) 1987-1994, 1996-1998, 2004, 2006, 2009-2011 Free Software
   Foundation, Inc.
   This file is part of the GNU C Library.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
# include "getopt.h"
#include "getopt_int.h"

#include <stdio.h>

/* This needs to come after some library #include
   to get __GNU_LIBRARY__ defined.  */
#ifdef __GNU_LIBRARY__
#include <stdlib.h>
#endif

#ifndef NULL
#define NULL 0
#endif

int
getopt_long (int argc, char *__getopt_argv_const *argv, const char *options,
             const struct option *long_options, int *opt_index)
{
	
	int ret = _getopt_internal (argc, (char **) argv, options, long_options,
                           opt_index, 0, 0);
       return ret;
}

int
_getopt_long_r (int argc, char **argv, const char *options,
                const struct option *long_options, int *opt_index,
                struct _getopt_data *d)
{
  return _getopt_internal_r (argc, argv, options, long_options, opt_index,
                             0, d, 0);
}

/* Like getopt_long, but '-' as well as '--' can indicate a long option.
   If an option that starts with '-' (not '--') doesn't match a long option,
   but does match a short option, it is parsed as a short option
   instead.  */

int
getopt_long_only (int argc, char *__getopt_argv_const *argv,
                  const char *options,
                  const struct option *long_options, int *opt_index)
{
  return _getopt_internal (argc, (char **) argv, options, long_options,
                           opt_index, 1, 0);
}

int
_getopt_long_only_r (int argc, char **argv, const char *options,
                     const struct option *long_options, int *opt_index,
                     struct _getopt_data *d)
{
  return _getopt_internal_r (argc, argv, options, long_options, opt_index,
                             1, d, 0);
}
