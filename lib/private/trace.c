#define TRACE_TAG TRACE_PRIVATE_TRACE
#include <stdio.h>
#include <stdlib.h>
#include <private/api.h>

__LIBBOOTIMAGE_PRIVATE_API__ int   trace_mask;

/* read a comma/space/colum/semi-column separated list of tags
 * from the TRACE environment variable and build the trace
 * mask from it. note that '1' and 'all' are special cases to
 * enable all tracing
 */
__LIBBOOTIMAGE_PRIVATE_API__ void  trace_init(void)
{
    const char*  p = getenv("BITRACE");
    const char*  q;

    static const struct {
        const char*  tag;
        int           flag;
    } tags[] = {
		{ "all", TRACE_ALL },
		{ "1", TRACE_ALL },
		{ "", TRACE_API_BOOTIMAGE },
		{ "", TRACE_API_BOOTIMAGE_EXTRACT },
		{ "", TRACE_API_BOOTIMAGE_FILE },
		{ "", TRACE_API_BOOTIMAGE_FILE_EXTRACT },
		{ "", TRACE_API_BOOTIMAGE_FILE_PRINT },
		{ "", TRACE_API_BOOTIMAGE_PRINT },
		{ "", TRACE_PRIVATE_ARCHIVE },
		{ "", TRACE_PRIVATE_BOOTIMAGE },
		{ "", TRACE_PRIVATE_CHECKS },
		{ "", TRACE_PRIVATE_KERNEL },
		{ "", TRACE_PRIVATE_TRACE },
		{ "", TRACE_PRIVATE_UTILS },
        { NULL, 0 }
    };

    if (p == NULL)
            return;

    /* use a comma/column/semi-colum/space separated list */
    while (*p) {
        int  len, tagn;

        q = strpbrk(p, " ,:;");
        if (q == NULL) {
            q = p + strlen(p);
        }
        len = q - p;

        for (tagn = 0; tags[tagn].tag != NULL; tagn++)
        {
            int  taglen = strlen(tags[tagn].tag);

            if (len == taglen && !memcmp(tags[tagn].tag, p, len) )
            {
                int  flag = tags[tagn].flag;
                if (flag == 0) {

                    trace_mask = ~0;

                    return;
                }
                trace_mask |= (1 << flag);
                break;
            }
        }
        p = q;
        if (*p)
            p++;
    }

}
