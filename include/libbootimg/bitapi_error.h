/*
 * bilib.h - libbootimg public api 
 * 
 * Copyright 2013 Trevor Drake <trevd1234@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
// Internal UUID
#ifndef _95c07e32_d459_11e2_9e89_5404a601fa9d
#define _95c07e32_d459_11e2_9e89_5404a601fa9d

#include <errno.h>
// API Define
#ifndef _BITAPI_ERROR_H_
#define _BITAPI_ERROR_H_ _95c07e32_d459_11e2_9e89_5404a601fa9d
#endif

#define EOK 0

#define BITAPI_OK 0
#define BITAPI_ERROR_SUCCESS BITAPI_OK /* Operation not permitted */


// Wrapper System Error Message from /usr/include/asm-generic/errno-base.h 
#define BITAPI_ERROR_PERM   EPERM       /* Operation not permitted */
#define BITAPI_ERROR_NOENT  ENOENT      /* No such file or directory */
#define BITAPI_ERROR_SRCH   ESRCH       /* No such process */
#define BITAPI_ERROR_INTR   EINTR       /* Interrupted system call */
#define BITAPI_ERROR_IO     EIO         /* I/O error */
#define BITAPI_ERROR_NXIO   ENXIO       /* No such device or address */
#define BITAPI_ERROR_2BIG   E2BIG       /* Argument list too long */
#define BITAPI_ERROR_NOEXEC ENOEXEC     /* Exec format error */
#define BITAPI_ERROR_BADF   EBADF       /* Bad file number */
#define BITAPI_ERROR_CHILD  ECHILD      /* No child processes */
#define BITAPI_ERROR_AGAIN  EAGAIN      /* Try again */
#define BITAPI_ERROR_NOMEM  ENOMEM      /* Out of memory */
#define BITAPI_ERROR_ACCES  EACCES      /* Permission denied */
#define BITAPI_ERROR_FAULT  EFAULT      /* Bad address */
#define BITAPI_ERROR_NOTBLK ENOTBLK     /* Block device required */
#define BITAPI_ERROR_BUSY   EBUSY       /* Device or resource busy */
#define BITAPI_ERROR_EXIST  EEXIST      /* File exists */
#define BITAPI_ERROR_XDEV   EXDEV       /* Cross-device link */
#define BITAPI_ERROR_NODEV  ENODEV      /* No such device */
#define BITAPI_ERROR_NOTDIR ENOTDIR     /* Not a directory */
#define BITAPI_ERROR_ISDIR  EISDIR      /* Is a directory */
#define BITAPI_ERROR_INVAL  EINVAL      /* Invalid argument */
#define BITAPI_ERROR_NFILE  ENFILE      /* File table overflow */
#define BITAPI_ERROR_MFILE  EMFILE      /* Too many open files */
#define BITAPI_ERROR_NOTTY  ENOTTY      /* Not a typewriter */
#define BITAPI_ERROR_TXTBSY ETXTBSY     /* Text file busy */
#define BITAPI_ERROR_FBIG   EFBIG       /* File too large */
#define BITAPI_ERROR_NOSPC  ENOSPC      /* No space left on device */
#define BITAPI_ERROR_SPIPE  ESPIPE      /* Illegal seek */
#define BITAPI_ERROR_ROFS   EROFS       /* Read-only file system */
#define BITAPI_ERROR_MLINK  EMLINK      /* Too many links */
#define BITAPI_ERROR_PIPE   EPIPE       /* Broken pipe */
#define BITAPI_ERROR_DOM    EDOM        /* Math argument out of domain of func */
#define BITAPI_ERROR_RANGE  ERANGE      /* Math result not representable */



#define BITAPI_ERROR_DEADLK      EDEADLK        /* Resource deadlock would occur */
#define BITAPI_ERROR_NAMETOOLONG ENAMETOOLONG   /* File name too long */
#define BITAPI_ERROR_NOLCK       ENOLCK          /* No record locks available */
#define BITAPI_ERROR_NOSYS       ENOSYS          /* Function not implemented */
#define BITAPI_ERROR_NOTEMPTY    ENOTEMPTY       /* Directory not empty */
#define BITAPI_ERROR_LOOP        ELOOP           /* Too many symbolic links encountered */
#define BITAPI_ERROR_WOULDBLOCK  EWOULDBLOCK        /* Operation would block */
#define BITAPI_ERROR_NOMSG       ENOMSG          /* No message of desired type */
#define BITAPI_ERROR_IDRM        EIDRM           /* Identifier removed */
#define BITAPI_ERROR_CHRNG       ECHRNG          /* Channel number out of range */
#define BITAPI_ERROR_L2NSYNC     EL2NSYNC        /* Level 2 not synchronized */
#define BITAPI_ERROR_L3HLT       EL3HLT          /* Level 3 halted */
#define BITAPI_ERROR_L3RST       EL3RST          /* Level 3 reset */
#define BITAPI_ERROR_LNRNG       ELNRNG          /* Link number out of range */
#define BITAPI_ERROR_UNATCH      EUNATCH         /* Protocol driver not attached */
#define BITAPI_ERROR_NOCSI       ENOCSI          /* No CSI structure available */
#define BITAPI_ERROR_L2HLT       EL2HLT          /* Level 2 halted */
#define BITAPI_ERROR_BADE        EBADE           /* Invalid exchange */
#define BITAPI_ERROR_BADR        EBADR           /* Invalid request descriptor */
#define BITAPI_ERROR_XFULL       EXFULL          /* Exchange full */
#define BITAPI_ERROR_NOANO       ENOANO          /* No anode */
#define BITAPI_ERROR_BADRQC      EBADRQC         /* Invalid request code */
#define BITAPI_ERROR_BADSLT      EBADSLT         /* Invalid slot */

#define BITAPI_ERROR_EDEADLOCK   EDEADLK

#define BITAPI_ERROR_BFONT           EBFONT             /* Bad font file format */
#define BITAPI_ERROR_NOSTR           ENOSTR             /* Device not a stream */
#define BITAPI_ERROR_NODATA          ENODATA         /* No data available */
#define BITAPI_ERROR_TIME            ETIME           /* Timer expired */
#define BITAPI_ERROR_NOSR            ENOSR           /* Out of streams resources */
#define BITAPI_ERROR_NONET           ENONET          /* Machine is not on the network */
#define BITAPI_ERROR_NOPKG           ENOPKG          /* Package not installed */
#define BITAPI_ERROR_REMOTE          EREMOTE         /* Object is remote */
#define BITAPI_ERROR_NOLINK          ENOLINK         /* Link has been severed */
#define BITAPI_ERROR_ADV             EADV            /* Advertise error */
#define BITAPI_ERROR_SRMNT           ESRMNT          /* Srmount error */
#define BITAPI_ERROR_COMM            ECOMM           /* Communication error on send */
#define BITAPI_ERROR_PROTO           EPROTO          /* Protocol error */
#define BITAPI_ERROR_MULTIHOP        EMULTIHOP       /* Multihop attempted */
#define BITAPI_ERROR_DOTDOT          EDOTDOT         /* RFS specific error */
#define BITAPI_ERROR_BADMSG          EBADMSG         /* Not a data message */
#define BITAPI_ERROR_OVERFLOW        EOVERFLOW       /* Value too large for defined data type */
#define BITAPI_ERROR_NOTUNIQ         ENOTUNIQ        /* Name not unique on network */
#define BITAPI_ERROR_BADFD           EBADFD          /* File descriptor in bad state */
#define BITAPI_ERROR_REMCHG          EREMCHG         /* Remote address changed */
#define BITAPI_ERROR_LIBACC          ELIBACC         /* Can not access a needed shared library */
#define BITAPI_ERROR_LIBBAD          ELIBBAD         /* Accessing a corrupted shared library */
#define BITAPI_ERROR_LIBSCN          ELIBSCN         /* .lib section in a.out corrupted */
#define BITAPI_ERROR_LIBMAX          ELIBMAX         /* Attempting to link in too many shared libraries */
#define BITAPI_ERROR_LIBEXEC         ELIBEXEC        /* Cannot exec a shared library directly */
#define BITAPI_ERROR_ILSEQ           EILSEQ          /* Illegal byte sequence */
#define BITAPI_ERROR_RESTART         ERESTART        /* Interrupted system call should be restarted */
#define BITAPI_ERROR_STRPIPE         ESTRPIPE        /* Streams pipe error */
#define BITAPI_ERROR_USERS           EUSERS          /* Too many users */
#define BITAPI_ERROR_NOTSOCK         ENOTSOCK       /* Socket operation on non-socket */
#define BITAPI_ERROR_DESTADDRREQ     EDESTADDRREQ    /* Destination address required */
#define BITAPI_ERROR_MSGSIZE         EMSGSIZE       /* Message too long */
#define BITAPI_ERROR_PROTOTYPE       EPROTOTYPE     /* Protocol wrong type for socket */
#define BITAPI_ERROR_NOPROTOOPT      ENOPROTOOPT    /* Protocol not available */
#define BITAPI_ERROR_PROTONOSUPPORT  EPROTONOSUPPORT /* Protocol not supported */
#define BITAPI_ERROR_SOCKTNOSUPPORT  ESOCKTNOSUPPORT /* Socket type not supported */
#define BITAPI_ERROR_OPNOTSUPP       EOPNOTSUPP      /* Operation not supported on transport endpoint */
#define BITAPI_ERROR_PFNOSUPPORT     EPFNOSUPPORT        /* Protocol family not supported */
#define BITAPI_ERROR_AFNOSUPPORT     EAFNOSUPPORT    /* Address family not supported by protocol */
#define BITAPI_ERROR_ADDRINUSE       EADDRINUSE     /* Address already in use */
#define BITAPI_ERROR_ADDRNOTAVAIL    EADDRNOTAVAIL   /* Cannot assign requested address */
#define BITAPI_ERROR_NETDOWN         ENETDOWN        /* Network is down */
#define BITAPI_ERROR_NETUNREACH      ENETUNREACH     /* Network is unreachable */
#define BITAPI_ERROR_NETRESET        ENETRESET       /* Network dropped connection because of reset */
#define BITAPI_ERROR_CONNABORTED     ECONNABORTED    /* Software caused connection abort */
#define BITAPI_ERROR_CONNRESET       ECONNRESET     /* Connection reset by peer */
#define BITAPI_ERROR_NOBUFS          ENOBUFS        /* No buffer space available */
#define BITAPI_ERROR_ISCONN          EISCONN        /* Transport endpoint is already connected */
#define BITAPI_ERROR_NOTCONN         ENOTCONN       /* Transport endpoint is not connected */
#define BITAPI_ERROR_SHUTDOWN        ESHUTDOWN      /* Cannot send after transport endpoint shutdown */
#define BITAPI_ERROR_TOOMANYREFS     ETOOMANYREFS   /* Too many references: cannot splice */
#define BITAPI_ERROR_TIMEDOUT        ETIMEDOUT      /* Connection timed out */
#define BITAPI_ERROR_CONNREFUSED     ECONNREFUSED    /* Connection refused */
#define BITAPI_ERROR_HOSTDOWN        EHOSTDOWN      /* Host is down */
#define BITAPI_ERROR_HOSTUNREACH     EHOSTUNREACH    /* No route to host */
#define BITAPI_ERROR_ALREADY         EALREADY       /* Operation already in progress */
#define BITAPI_ERROR_INPROGRESS      EINPROGRESS    /* Operation now in progress */
#define BITAPI_ERROR_STALE           ESTALE         /* Stale NFS file handle */
#define BITAPI_ERROR_UCLEAN          EUCLEAN        /* Structure needs cleaning */
#define BITAPI_ERROR_NOTNAM          ENOTNAM        /* Not a XENIX named type file */
#define BITAPI_ERROR_NAVAIL          ENAVAIL        /* No XENIX semaphores available */
#define BITAPI_ERROR_ISNAM           EISNAM         /* Is a named type file */
#define BITAPI_ERROR_REMOTEIO        EREMOTEIO      /* Remote I/O error */
#define BITAPI_ERROR_DQUOT           EDQUOT         /* Quota exceeded */

#define BITAPI_ERROR_NOMEDIUM    ENOMEDIUM   /* No medium found */
#define BITAPI_ERROR_MEDIUMTYPE  EMEDIUMTYPE /* Wrong medium type */
#define BITAPI_ERROR_CANCELED    ECANCELED   /* Operation Canceled */
#define BITAPI_ERROR_NOKEY       ENOKEY      /* Required key not available */
#define BITAPI_ERROR_KEYEXPIRED  EKEYEXPIRED /* Key has expired */
#define BITAPI_ERROR_KEYREVOKED  EKEYREVOKED /* Key has been revoked */
#define BITAPI_ERROR_KEYREJECTED EKEYREJECTED   /* Key was rejected by service */

/* for robust mutexes */
#define BITAPI_ERROR_OWNERDEAD       EOWNERDEAD     /* Owner died */
#define BITAPI_ERROR_NOTRECOVERABLE  ENOTRECOVERABLE /* State not recoverable */
                                    
#define BITAPI_ERROR_RFKILL          ERFKILL        /* Operation not possible due to RF-kill */
                                    
#define BITAPI_ERROR_HWPOISON        EHWPOISON      /* Memory page has hardware error */

#define BITAPI_ERROR_NOKERNEL   501 EBADFD 

#endif
