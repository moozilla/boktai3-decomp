#include "global.h"

void sub_080FCB3C(void);
void sub_080FBF20(void *);
void sub_080FC844(void *);
void sub_0807E908(void *);

s32 sub_080FE668(void *p)
{
    sub_080FCB3C();
    sub_080FBF20(p);
    sub_080FC844(p);
    sub_0807E908(p);
    return 0;
}
