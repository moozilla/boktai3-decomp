#include "global.h"

struct S { u8 f[0x1c]; void *p1c; void *p20; void *p24; void *p28; u8 g[0x1c]; u8 sub[1]; };
void sub_08006FD8(void *);
void sub_08013B74(void *);
void sub_08219D38(void *);
void sub_08217EAC(void *);
void sub_08214514(void *);

u32 sub_0800700C(struct S *s)
{
    sub_08006FD8(s);
    sub_08013B74(s->sub);
    if (s->p24 != 0)
        sub_08219D38(s->p24);
    if (s->p28 != 0)
        sub_08219D38(s->p28);
    if (s->p1c != 0) {
        sub_08217EAC(s->p1c);
        sub_08219D38(s->p1c);
    }
    if (s->p20 != 0) {
        sub_08214514(s->p20);
        sub_08219D38(s->p20);
    }
    sub_08219D38(s);
    return 0;
}
