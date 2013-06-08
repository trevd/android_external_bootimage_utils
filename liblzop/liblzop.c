#ifndef __LIBLZOP_C
#define __LIBLZOP_C

#include <stdlib.h>  // malloc
#include <stdio.h>   // fprintf
#include <string.h>  // memcpy, memset
#include <unistd.h>  // read
#include <assert.h>  // assert
#include "lzop.h"
#include "lzop_support.h"

/* 
 * Initialize the stream with default values
 */
void init_lzop_stream(lzop_stream *s) {
  s->method = M_LZO1X_1_15;
  s->level = 1;
  s->is_stdin = 1;
  s->is_stdout = 1;
  s->is_multipart = 0;
  s->use_crc32 = 0;
  s->outbuf = NULL;
  s->next_out = NULL;
  s->err_msg[0] = '\0';
  // initialize file header per above values
  init_lzop_header(&(s->h), s);
  s->wrkmem = malloc(lzo_malloc_value(s->method));
} // init_lzop_stream


// like perror, but for lzop errors
void lzop_perror(lzop_stream *s, const char* msg) {
  assert(NULL!=s);

  if( NULL == msg ) {
    fprintf(stderr, "%s\n", s->err_msg);
  } else {
    fprintf(stderr, "%s: %s\n", msg, s->err_msg);
  }
  return;
} // lzop_perror


/* given header and stream
 * where stream has the following members set:
 *   is_stdin
 *   is_stdout
 *   is_multipart
 *   use_crc32
 */  
void init_lzop_header(header_t *h, lzop_stream *s) {
  assert(s->method > 0);
  assert(s->level > 0);
  
  memset(h,0,sizeof(header_t));

  h->version = LZOP_VERSION & 0xffff;
  h->version_needed_to_extract = 0x0940;
  h->lib_version = lzo_version() & 0xffff;
  h->method = (unsigned char) s->method;
  h->level = (unsigned char) s->level;
  h->filter = 0;

  h->flags = 0;
  h->flags |= F_OS & F_OS_MASK;
  h->flags |= F_CS & F_CS_MASK;
  if(s->is_stdin)
    h->flags |= F_STDIN;
  if(s->is_stdout)
    h->flags |= F_STDOUT;
  if(s->is_multipart)
    h->flags |= F_MULTIPART;
#ifdef OPT_NAME_DEFAULT
  h->flags |= F_NAME_DEFAULT;
#endif
#ifdef DOSISH
  h->flags |= F_DOSISH;
#endif
  if(s->use_crc32) {
    h->flags |= F_H_CRC32;
    if (h->version_needed_to_extract < 0x1001)
      h->version_needed_to_extract = 0x1001;
  } else {
    h->flags |= F_ADLER32_D;
  }

  h->mode = 0;
  h->mtime_low = h->mtime_high = 0;

} // init_lzop_header


size_t bwrite_lzop_magic(unsigned char* outbuf, size_t outlen) {
  assert(outlen>=sizeof(lzop_magic));
  memcpy(outbuf, lzop_magic, sizeof(lzop_magic));
  return sizeof(lzop_magic);
} // bwrite_lzop_magic


int check_magic(const unsigned char *magic, const size_t len)
{
  if( len != sizeof(lzop_magic) &&
      memcmp(magic,lzop_magic,sizeof(lzop_magic)) == 0 ) {
    return 0;
  }

  return 1;
}

/* given stream, output buffer, and size of output buffer
 * write file header into buffer
 * and return number of bytes written or 0 on error.
 * Use lzop_perror to print an error msg to stderr
 */
size_t bwrite_lzop_header(lzop_stream *s, unsigned char* outbuf, size_t outlen) {
  unsigned char* outp = outbuf; // where to write next value

  // if output buffer too small, then error
  if( NULL == outbuf || outlen < LZOP_HEADER_SIZE ) {
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "get_lzop_header: outlen must be at least %d bytes", LZOP_HEADER_SIZE);
    return 0;
  }

  lzo_uint32 f_adler32 = ADLER32_INIT_VALUE;
  lzo_uint32 f_crc32 = CRC32_INIT_VALUE;

  set_be16(outp, s->h.version);
  outp += 2;

  set_be16(outp, s->h.lib_version);
  outp += 2;

  set_be16(outp,s->h.version_needed_to_extract);
  outp += 2;
  *outp = s->h.method;
  outp++;
  *outp = s->h.level;
  outp++;
  set_be32(outp,s->h.flags);
  outp += 4;
  set_be32(outp,s->h.mode);
  outp += 4;
  set_be32(outp,s->h.mtime_low);
  outp += 4;
  set_be32(outp,s->h.mtime_high);
  outp += 4;
  // assume no file name
  *outp = '\0';
  outp++;

  if (s->h.flags & F_H_CRC32) {
    // create checksum of header, minus magic number
    f_crc32 = lzo_crc32(f_crc32, outbuf, outp-outbuf);
    // write to buffer
    set_be32(outp,f_crc32);
  } else {
    // create checksum of header, minus magic number
    f_adler32 = lzo_adler32(f_adler32, outbuf, outp-outbuf);
    // write to buffer
    set_be32(outp,f_adler32);
  }
  outp += 4;

  return (outp - outbuf);  // this should always be LZOP_HEADER_SIZE
} // bwrite_lzop_header


size_t bwrite_lzop_block_header(lzop_stream *s, lzop_block *block, lzo_bytep outbuf, lzo_uint32 outlen) {
  lzo_bytep cursor = outbuf;

  assert(s!=NULL);
  assert(block!=NULL);
  assert(outlen>=24);

  // uncompressed block length
  set_be32(cursor, block->d_len);
  cursor += 4;
  if( 0 == block->d_len ) {
    return (cursor - outbuf);
  }
  // compressed block length
  set_be32(cursor, block->c_len);
  cursor += 4;
  // decompressed block adler32 checksum, if necessary
  if( s->h.flags & F_ADLER32_D ) {
    set_be32(cursor, block->d_adler32);
    cursor += 4;
  }
  // decompressed block crc32 checksum, if necessary
  if( s->h.flags & F_CRC32_D ) {
    set_be32(cursor, block->d_crc32);
    cursor += 4;
  }
  // compressed block adler32 checksum, if necessary
  if( s->h.flags & F_ADLER32_C ) {
    set_be32(cursor, block->c_adler32);
    cursor += 4;
  }
  // compressed block crc32 checksum, if necessary
  if( s->h.flags & F_CRC32_C ) {
    set_be32(cursor, block->c_crc32);
    cursor += 4;
  }

  return (cursor - outbuf);
}

/* compress block
 * Given: stream, inbuf, inlen, outbuf, outlen
 * Compress inlen bytes of inbuf and store in outbuf
 * Return: number of bytes of compressed data stored in outbuf
 *   or 0 for error
 * Use lzop_perror to print an error msg to stderr
 */
size_t lzop_compress( lzop_stream *s, unsigned char* inbuf, size_t inlen, unsigned char* outbuf, size_t outlen) {
  int ret = LZO_E_OK;
  lzop_block block;
  lzo_uint tmp;

  assert(s!=NULL);
  assert(inbuf!=NULL);
  assert(outbuf!=NULL);

  if( outlen < (LZOP_MAX_BLOCK_HEADER_LEN + LZOP_BLOCK_MAX_COMPRESSED_SIZE(inlen)) ) {
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_compress: outbuf not large enough: %d", (int)outlen);
    return 0;
  }

  // skip standard header fields (uncompressed len, compressed len)
  block.c_buf = outbuf + 8;

  // uncompressed block size
  block.d_len = inlen;

  // checksums of uncompressed block
  if( s->h.flags & F_ADLER32_D ) {
    block.d_adler32 = lzo_adler32(ADLER32_INIT_VALUE, inbuf, inlen);
    // make room
    block.c_buf += 4;
  }
  if( s->h.flags & F_CRC32_D ) {
    block.d_crc32 = lzo_crc32(CRC32_INIT_VALUE, inbuf, inlen);
    // make room
    block.c_buf += 4;
  }
  // make room for compressed values
  if( s->h.flags & F_ADLER32_C ) {
    block.c_buf += 4;
  }
  if( s->h.flags & F_CRC32_C ) {
    block.c_buf += 4;
  }

  // set max output length
  block.c_len = outlen - (block.c_buf - outbuf);

  tmp = block.c_len;
  // compress
  switch(s->h.method) {
    case M_LZO1X_1:
      ret = lzo1x_1_compress(inbuf, inlen, block.c_buf, &tmp, s->wrkmem);
      break;
    case M_LZO1X_1_15:
      ret = lzo1x_1_15_compress(inbuf, inlen, block.c_buf, &tmp, s->wrkmem);
      break;
    case M_LZO1X_999:
      ret = lzo1x_999_compress_level(inbuf, inlen, block.c_buf, &tmp, s->wrkmem, NULL, 0, 0, s->h.level);
      break;
    default:
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_compress: unknown compression method: %d", s->h.method);
      return 0;
  }
  block.c_len = tmp;

  if( ret != LZO_E_OK || block.c_len > LZOP_BLOCK_MAX_COMPRESSED_SIZE(block.d_len) ) {
    // according to docs, should never happen!
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_compress: compression failed: %d", ret);
    return 0;
  }

  // checksums of compressed block
  if( s->h.flags & F_ADLER32_C ) {
    block.c_adler32 = lzo_adler32(ADLER32_INIT_VALUE, block.c_buf, block.c_len);
  }
  if( s->h.flags & F_CRC32_C ) {
    block.c_crc32 = lzo_crc32(CRC32_INIT_VALUE, block.c_buf, block.c_len);
  }

  tmp = bwrite_lzop_block_header(s, &block, outbuf, outlen);
  if( tmp != block.c_buf - outbuf ) {
    // should never happen!
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_compress: error writing block header: %d != %d", (int)tmp, (int)(block.c_buf - outbuf));
    return 0;
  }

  /* return number of bytes written: end of the
   * compressed data (c_buf + c_len) minus the
   * beginning of the block (outbuf
   */
  return ( (block.c_buf + block.c_len) - outbuf );
} // lzop_compress

/* given stream and input file descriptor
 * read file header, record values in stream,
 * and return number of bytes read or 0 on error.
 * Note that the file header does not include the magic.
 * Use lzop_perror to print an error msg to stderr
 */
size_t fread_lzop_header(lzop_stream *s, int fd) {
  size_t num = 0, fn_len = 0;
  unsigned char buf[LZOP_MAX_HEADER_SIZE];
  lzo_bytep cursor = buf;


  assert(s!=NULL);

  // ver, lib_ver, ver_needed, method, lvl, flags, mode, mtime
  // 2+2+2+1+1+4+4+4+4
  if( 24 != (num = read(fd, cursor, 24)) ) {
    // error
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "fread_lzop_header: truncated file, expected 24 bytes");
    return 0;
  }
  cursor += 24;
  // original filename len
  num = read(fd, cursor, 1);
  fn_len = (size_t) *cursor;
  cursor++;
  if( 1 != num ) {
    // error
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "fread_lzop_header: truncated file, expected filename len");
    return 0;
  }
  if( fn_len > 0 ) {
    num = read(fd, cursor, fn_len);
    cursor += num;
    if( num != fn_len ) {
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "fread_lzop_header: truncated file, expected filename");
      return 0;
    }
  }
  // chksum
  num = read(fd, cursor, 4);
  if( 4 != num ) {
    // error
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "fread_lzop_header: truncated file, expected checksum");
    return 0;
  }
  cursor += 4;

  return bread_lzop_header(s, buf, cursor - buf);

} // fread_lzop_header

/* given stream, input buffer, and size of input buffer
 * read file header from buffer, record values in stream,
 * and return number of bytes read or 0 on error.
 * Note that the file header does not include the magic.
 * Use lzop_perror to print an error msg to stderr
 */
size_t bread_lzop_header(lzop_stream *s, unsigned char* inbuf, size_t inlen) {
  lzo_bytep cursor = inbuf;
  lzo_uint32 ck;
  size_t fn_len = 0;

  assert(s!=NULL);
  assert(inbuf!=NULL);
  assert(inlen>=LZOP_HEADER_SIZE);

  s->h.version = get_be16(cursor);
  cursor += 2;
  s->h.lib_version = get_be16(cursor);
  cursor += 2;
  s->h.version_needed_to_extract = get_be16(cursor);
  cursor += 2;
  s->h.method = *cursor;
  cursor++;
  s->h.level = *cursor;
  cursor++;
  s->h.flags = get_be32(cursor);
  cursor += 4;
  s->h.mode = get_be32(cursor);
  cursor += 4;
  s->h.mtime_low = get_be32(cursor);
  cursor += 4;
  s->h.mtime_high = get_be32(cursor);
  cursor += 4;
  fn_len = (size_t) *cursor;
  cursor++;
  if(0 == fn_len ) {
    s->h.name[0] = '\0';
  } else if(0 < fn_len) {
    memcpy(s->h.name, cursor, fn_len);
    s->h.name[fn_len] = '\0';
    cursor += fn_len;
  } else {
    // error
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "bread_lzop_header: impossible condition, filename len < 0");
    return 0;
  }

  // verify header checksum 
  s->h.header_checksum = get_be32(cursor);
  if (s->h.flags & F_H_CRC32) {
    // crc32
    if( (ck = lzo_crc32(CRC32_INIT_VALUE, inbuf, cursor-inbuf)) != s->h.header_checksum ) {
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "bread_lzop_header: checksum mismatch %08x != %08x", ck, s->h.header_checksum);
      return 0;
    }
  } else {
    // adler32
    if( (ck=lzo_adler32(ADLER32_INIT_VALUE, inbuf, cursor-inbuf)) != s->h.header_checksum ) {
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "bread_lzop_header: checksum mismatch %08x != %08x", ck, s->h.header_checksum);
      return 0;
    }
  }
  cursor += 4;
  //fprintf(stderr, "header checksum = %08x\n", s->h.header_checksum);

  return cursor - inbuf;
} // bread_lzop_header

// pre: c_buf and c_len are set to output buffer and buffer size
size_t fread_lzop_block(lzop_stream *s, int fd, lzop_block *block) {
  lzo_byte rbuf[LZOP_MAX_BLOCK_HEADER_LEN]; // read buffer
  size_t block_header_size = 0;
  lzo_bytep buf = block->c_buf;
  size_t buflen = block->c_len;

  assert(s!=NULL);
  assert(block!=NULL);
  assert(fd>=0);

  // uncompressed block length, compressed block length
  block_header_size = 4 + 4;

  if( s->h.flags & F_ADLER32_D ) {
    block_header_size += 4;
  }
  if( s->h.flags & F_CRC32_D ) {
    block_header_size += 4;
  }
  if( s->h.flags & F_ADLER32_C ) {
    block_header_size += 4;
  }
  if( s->h.flags & F_CRC32_C ) {
    block_header_size += 4;
  }

  // smallest block size is when uncompressed block len = 0
  if( read(fd, rbuf, block_header_size) < 4 ) {
    // error
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "fread_lzop_block: truncated block header");
    return 0;
  }

  if( 0 == bread_lzop_block(s, rbuf, sizeof(rbuf), block) ) {
    return 0;
  }
  // point back at real data block
  block->c_buf = buf;

  // read data into data block
  if( block->c_len > buflen ) {
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "fread_lzop_block: buffer too small, needed %d", block->c_len);
    return 0;
  }
  if( read(fd, block->c_buf, block->c_len) != block->c_len ) {
    // TODO: error
  }

  return block_header_size + block->c_len;
} // fread_lzop_block

// return true if var == val, else set s->err_msg to str
int check_val(lzop_stream *s, unsigned int var, unsigned int val, const char* str) {
  assert(s!=NULL);
  assert(str!=NULL);
  if( var != val ) {
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "%s", str);
    return 0;
  }
  return 1;
}

size_t bread_lzop_block(lzop_stream *s, unsigned char* inbuf, size_t inlen, lzop_block *block) {
  lzo_bytep cursor = inbuf;

  assert(s!=NULL);
  assert(block!=NULL);
  assert(inlen>=4);   //smallest block size

  // uncompressed block length
  block->d_len = get_be32(cursor);
  cursor += 4; inlen -= 4;
  if( 0 == block->d_len ) {
    return cursor - inbuf;
  }

  // compressed block length
  if( check_val(s, inlen, 4, "bread_lzop_block: truncated block header") == 1 ) { return 0; }
  block->c_len = get_be32(cursor);
  cursor += 4; inlen -= 4;
  // decompressed block adler32
  if( s->h.flags & F_ADLER32_D ) {
    if( check_val(s, inlen, 4, "bread_lzop_block: truncated block header") == 1 ) { return 0; }
    block->d_adler32 = get_be32(cursor);
    cursor += 4; inlen -= 4;
  }
  // decompressed block crc32 checksum, if necessary
  if( s->h.flags & F_CRC32_D ) {
    if( check_val(s, inlen, 4, "bread_lzop_block: truncated block header") == 1 ) { return 0; }
    block->d_crc32 = get_be32(cursor);
    cursor += 4;
  }
  // compressed block adler32 checksum, if necessary
  if( s->h.flags & F_ADLER32_C ) {
    if( check_val(s, inlen, 4, "bread_lzop_block: truncated block header") == 1 ) { return 0; }
    block->c_adler32 = get_be32(cursor);
    cursor += 4;
  }
  // compressed block crc32 checksum, if necessary
  if( s->h.flags & F_CRC32_C ) {
    if( check_val(s, inlen, 4, "bread_lzop_block: truncated block header") == 1 ) { return 0; }
    block->c_crc32 = get_be32(cursor);
    cursor += 4;
  }
  // compressed data
  block->c_buf = cursor;

  return cursor - inbuf;
} // bread_lzop_block

/* Given stream, lzop-style block, number of bytes in compressed block, an output buffer, and size of output buffer
 * Write decompressed block into output buffer
 * Return: number of bytes written to output buffer, 0 on error
 * Use lzop_perror to print an error msg to stderr
 */
size_t lzop_decompress( lzop_stream *s, lzop_block *b, unsigned char *outbuf, size_t outlen) {
  int ret = LZO_E_OK;

  assert(s!=NULL);
  assert(b!=NULL);
  assert(outbuf!=NULL);

  // TODO: make sure outbuf is big enough

  if( b->d_len > LZOP_MAX_BLOCK_SIZE ) {
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_decompress: corrupt archive, block size too large");
    return 0;
  } else if( b->c_len <= 0 ) {
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_decompress: corrupt archive, len(compressed data) <= zero");
    return 0;
  } else if( b->d_len > outlen ) {
    snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_decompress: output buffer too small, requires %d", b->d_len);
    return 0;
  }

  // verify compressed block checksum(s)
  if( s->h.flags & F_ADLER32_C ) {
//fprintf(stderr,"block: checking adler32\n");
    if( b->c_adler32 != lzo_adler32(ADLER32_INIT_VALUE, b->c_buf, b->c_len) ) {
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_decompress: adler32 checksum mismatch");
    }
    return 0;
  }
  if( s->h.flags & F_CRC32_C ) {
//fprintf(stderr,"block: checking crc32\n");
    if( b->c_crc32 != lzo_crc32(CRC32_INIT_VALUE, b->c_buf, b->c_len) ) {
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_decompress: crc32 checksum mismatch");
    }
    return 0;
  }

  // check for optimized blocksize
  b->d_buf = outbuf;
  if( b->c_len != b->d_len ) {
    // decompress
    ret = lzo1x_decompress_safe(b->c_buf, b->c_len, b->d_buf, &outlen, NULL);
    if( ret != LZO_E_OK || outlen != b->d_len ) {
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_decompress: data violation");
      return 0;
    }
  } else {
    // block did not compress well, so uncompressed block = compressed block
    memcpy(b->d_buf, b->c_buf, b->d_len);
  }

  // check decompress checksum
  if( s->h.flags & F_ADLER32_C ) {
    if( lzo_adler32(ADLER32_INIT_VALUE, b->d_buf, b->d_len) != b->d_adler32 ) {
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_decompress: bad decompressed block adler32 checksum");
      return 0;
    }
  }
  if( s->h.flags & F_CRC32_D ) {
    if( lzo_crc32(CRC32_INIT_VALUE, b->d_buf, b->d_len) != b->d_crc32 ) {
      snprintf(s->err_msg, LZOP_MAXLEN_ERR, "lzop_decompress: bad decompressed block crc32 checksum");
      return 0;
    }
  }

  return b->d_len;
} // lzop_decompress

/* Given stream and file pointer
 * Print the stream header values to the file pointer
 * If f==NULL, then assume stdout
 */
void lzop_print_header(lzop_stream *s, FILE *f) {
  fprintf(f, "version = %d\nlib_version = %d\nver extract = %d\nmethod = %d\nlevel = %d\n",
          s->h.version, s->h.lib_version, s->h.version_needed_to_extract, s->h.method, s->h.level);
  fprintf(f, "flags = %x\nmode = %d\nmtime_low = %x\nmtime_high = %x\nfname len = %zu\n",
          s->h.flags, s->h.mode, s->h.mtime_low, s->h.mtime_high, strlen(s->h.name) );
  fprintf(f, "checksum = %x\n", s->h.header_checksum);
}

#endif
