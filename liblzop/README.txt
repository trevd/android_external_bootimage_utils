= ACKNOWLEDGEMENT =

A good portion of this code is based on the LZOP program
source written by Markus F.X.J. Oberhumer and released
under the GNU General Public License v2


= PROGRAMMING WITH LIBLZOP =

Look at the Public Functions section in lzop.h

Should only need to include lzop.h in your C program

Then make in this directory and link your program with
the resulting .o files and -llzo2

WARNINGS:
* Currently hard-coded to work on Unix platforms
* Not thread-safe

Standard caveats apply: this is pre-pre-alpha code, which
may or may not work for you, which may or may not work for
me; give all props to LZOP author for what you like; blame
me for what you don't; IMHO YMMV YADA YADA YADA


= REFERENCES =

LZO             http://www.oberhumer.com/opensource/lzo/
LZOP            http://www.lzop.org/
