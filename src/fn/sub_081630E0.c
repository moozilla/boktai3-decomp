#include "global.h"

struct P { u8 f00[0x341]; u8 i; };
extern const u32 gUnk_08610E44[];

void sub_0824923C(struct P *, u32);
void sub_08163020(struct P *);

void sub_081630E0(struct P *p)
{
    sub_0824923C(p, gUnk_08610E44[p->i]);
    sub_08163020(p);
}
