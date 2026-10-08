#include "global.h"

void sub_080FC3F8(void *);
void sub_080FBEDC(void *);
void sub_080FB744(void *);
void sub_080FCB64(void *);
void sub_080FB050(void *);
void sub_080FCDEC(void *);

s32 sub_080FD128(void *s)
{
    sub_080FC3F8(s);
    sub_080FBEDC(s);
    sub_080FB744(s);
    sub_080FCB64(s);
    sub_080FB050(s);
    sub_080FCDEC(s);
    return 1;
}
