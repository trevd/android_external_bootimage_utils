#ifndef __LZOP_H
#define __LZOP_H

// FIX : include stdio to file unknown type name 'FILE'
#include <stdio.h>

#include <lzo/lzo1x.h>
#include "version.h"

#define LZOP_MAX_BLOCK 1024*1024*10
#define LZOP_HEADER_SIZE 2+2+2+1+1+4+4+4+4+1+4 // 29
#define LZOP_MAX_HEADER_SIZE 24+256+4 // standard fields, filename, checksum
#define LZOP_MAXLEN_ERR 1024
#define LZOP_FACTOR (1+(16/1024))

#define LZOP_BLOCK_SIZE  (256*10241)
#define LZOP_MAX_BLOCK_SIZE  (64*1024*1024)  // 64MB
/* LZO may expand uncompressible data by a small amount */
#define LZOP_BLOCK_MAX_COMPRESSED_SIZE(x)  ((x) + (x) / 16 + 64 + 3)
#define LZOP_MIN_BLOCK_HEADER_LEN  4  // when uncompressed size = 0
#define LZOP_MAX_BLOCK_HEADER_LEN  24 // see lzop_block

#define ADLER32_INIT_VALUE  1
#define CRC32_INIT_VALUE    0

/**********************************************************
 * enums
 **********************************************************/

enum {
    CMD_NONE,
    CMD_COMPRESS,
    CMD_DECOMPRESS, CMD_TEST, CMD_LIST, CMD_LS, CMD_INFO,
    CMD_SYSINFO, CMD_LICENSE, CMD_HELP, CMD_INTRO, CMD_VERSION
};


enum {
    M_LZO1X_1     =     1,
    M_LZO1X_1_15  =     2,
    M_LZO1X_999   =     3,
    M_NRV1A       =  0x1a,
    M_NRV1B       =  0x1b,
    M_NRV2A       =  0x2a,
    M_NRV2B       =  0x2b,
    M_NRV2D       =  0x2d,
    M_ZLIB        =   128,

    M_UNUSED
};


/**********************************************************
 * file header
 **********************************************************/

/*
 * The first nine bytes of lzop file always contain the following values:
 *
 *                             0   1   2   3   4   5   6   7   8
 *                           --- --- --- --- --- --- --- --- ---
 * (hex)                      89  4c  5a  4f  00  0d  0a  1a  0a
 * (decimal)                 137  76  90  79   0  13  10  26  10
 * (C notation - ASCII)     \211   L   Z   O  \0  \r  \n \032 \n
 */

static const unsigned char lzop_magic[9] =
                   { 0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a };

/* header flags */
#define F_ADLER32_D     0x00000001L
#define F_ADLER32_C     0x00000002L
#define F_STDIN         0x00000004L
#define F_STDOUT        0x00000008L
#define F_NAME_DEFAULT  0x00000010L
#define F_DOSISH        0x00000020L
#define F_H_EXTRA_FIELD 0x00000040L
#define F_H_GMTDIFF     0x00000080L
#define F_CRC32_D       0x00000100L
#define F_CRC32_C       0x00000200L
#define F_MULTIPART     0x00000400L
#define F_H_FILTER      0x00000800L
#define F_H_CRC32       0x00001000L
#define F_H_PATH        0x00002000L
#define F_MASK          0x00003FFFL

/* operating system & file system that created the file [mostly unused] */
#define F_OS_FAT        0x00000000L         /* DOS, OS2, Win95 */
#define F_OS_AMIGA      0x01000000L
#define F_OS_VMS        0x02000000L
#define F_OS_UNIX       0x03000000L
#define F_OS_VM_CMS     0x04000000L
#define F_OS_ATARI      0x05000000L
#define F_OS_OS2        0x06000000L         /* OS2 */
#define F_OS_MAC9       0x07000000L
#define F_OS_Z_SYSTEM   0x08000000L
#define F_OS_CPM        0x09000000L
#define F_OS_TOPS20     0x0a000000L
#define F_OS_NTFS       0x0b000000L         /* Win NT/2000/XP */
#define F_OS_QDOS       0x0c000000L
#define F_OS_ACORN      0x0d000000L
#define F_OS_VFAT       0x0e000000L         /* Win32 */
#define F_OS_MFS        0x0f000000L
#define F_OS_BEOS       0x10000000L
#define F_OS_TANDEM     0x11000000L
#define F_OS_SHIFT      24
#define F_OS_MASK       0xff000000L

/* character set for file name encoding [mostly unused] */
#define F_CS_NATIVE     0x00000000L
#define F_CS_LATIN1     0x00100000L
#define F_CS_DOS        0x00200000L
#define F_CS_WIN32      0x00300000L
#define F_CS_WIN16      0x00400000L
#define F_CS_UTF8       0x00500000L         /* filename is UTF-8 encoded */
#define F_CS_SHIFT      20
#define F_CS_MASK       0x00f00000L

/* these bits must be zero */
#define F_RESERVED      ((F_MASK | F_OS_MASK | F_CS_MASK) ^ 0xffffffffL)

typedef struct
{
    unsigned version;
    unsigned lib_version;
    unsigned version_needed_to_extract;
    unsigned char method;
    unsigned char level;
    lzo_uint32 flags;
    lzo_uint32 filter;
    lzo_uint32 mode;
    lzo_uint32 mtime_low;
    lzo_uint32 mtime_high;
    lzo_uint32 header_checksum;

    lzo_uint32 extra_field_len;
    lzo_uint32 extra_field_checksum;

/* info */
    const char *method_name;

    char name[255+1];
}
header_t;


/**********************************************************
 * Public Functions
 **********************************************************/

// TODO: this should be changed by a configure script, eventually
#define F_OS   F_OS_UNIX 
#define F_CS   F_CS_NATIVE

typedef struct {        // Default values:
  header_t h;
  int method;           // M_LZO1X_1
  int level;            // 1
  int is_stdin;         // 1
  int is_stdout;        // 1
  int is_multipart;     // 0
  int use_crc32;        // 0
  char *outbuf;         // NULL
  char *next_out;       // NULL
  char err_msg[LZOP_MAXLEN_ERR];
  lzo_bytep wrkmem;
} lzop_stream;


/* Block header and pointers to data
 * The following fields are written to the block header
 * in the order that they appear here.  The first field
 * always exists.
 */
typedef struct {            // Field is in the header?
  lzo_uint32 d_len;         // always
  lzo_uint32 c_len;         // only if d_len>0
  lzo_uint32 d_adler32;     // optional
  lzo_uint32 d_crc32;       // optional
  lzo_uint32 c_adler32;     // optional
  lzo_uint32 c_crc32;       // optional
  /* Below here, not part of the block header in the file */
  lzo_bytep c_buf;
  lzo_bytep d_buf;
} lzop_block;


/* Initialize the stream with default values
 *
 */
void init_lzop_stream(lzop_stream *s);

// like perror, but for lzop errors
void lzop_perror(lzop_stream *s, const char* msg);

/* Given output buffer and size of output buffer
 * Write the LZOP magic number to the buffer, which is
 * the start of any LZOP file.
 */
size_t bwrite_lzop_magic(unsigned char* outbuf, size_t outlen);

/* given stream, output buffer, and size of output buffer
 * write file header into buffer
 * and return number of bytes written or 0 on error.
 * Note that the file header does not include the magic.
 * Use lzop_perror to print an error msg to stderr
 */
size_t bwrite_lzop_header(lzop_stream *s, unsigned char* outbuf, size_t outlen);

/* given stream, input buffer, and size of input buffer
 * read file header from buffer, record values in stream,
 * and return number of bytes read or 0 on error.
 * Note that the file header does not include the magic.
 * Use lzop_perror to print an error msg to stderr
 */
size_t bread_lzop_header(lzop_stream *s, unsigned char* inbuf, size_t inlen);

/* given stream and input file descriptor
 * read file header, record values in stream,
 * and return number of bytes read or 0 on error.
 * Note that the file header does not include the magic.
 * Use lzop_perror to print an error msg to stderr
 */
size_t fread_lzop_header(lzop_stream *s, int fd);

/* given stream, input file descriptor, and a block
 *   where block.c_buf and block.c_len are set to output
 *   buffer and buffer size
 * read the next block from the file, set block header
 *   values, and write compressed data to block.c_buf
 * Return: number of bytes read or 0 on error
 */
size_t fread_lzop_block(lzop_stream *s, int fd, lzop_block *b);

/* 
 */
size_t bread_lzop_block(lzop_stream *s, unsigned char* inbuf, size_t inlen, lzop_block *b);

/* compress block
 * Given: stream, inbuf, inlen, outbuf, outlen
 * Where outlen >= LZOP_BLOCK_MAX_COMPRESSED_SIZE(inlen)
 * Compress inlen bytes of inbuf and store in outbuf
 * Return: number of bytes of compressed data stored in outbuf
 *   or 0 for error
 * Use lzop_perror to print an error msg to stderr
 */
size_t lzop_compress( lzop_stream *s, unsigned char* inbuf, size_t inlen, unsigned char* outbuf, size_t outlen);

/* Given stream, block, an output buffer, and size of output buffer
 * Write decompressed block into output buffer
 * Return: number of bytes written to output buffer, 0 on error
 * Use lzop_perror to print an error msg to stderr
 */
size_t lzop_decompress( lzop_stream *s, lzop_block *b, unsigned char *outbuf, size_t outlen);

/* given header and stream
 * where stream has the following members set:
 *   is_stdin
 *   is_stdout
 *   is_multipart
 *   use_crc32
 */
void init_lzop_header(header_t *h, lzop_stream *s);

/* Given buffer and length
 * Returns 0 if buffer matches lzop_magic
 */
int check_magic(const unsigned char *magic, const size_t len);

/* Given stream and file pointer
 * Print the stream header values to the file pointer
 * If f==NULL, then assume stdout
 */
void lzop_print_header( lzop_stream *s, FILE *f );

#endif
