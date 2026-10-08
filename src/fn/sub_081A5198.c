#include "global.h"

void sub_08214514(void *);
void sub_08013B74(void *);
void sub_0821FE6C(void *);
struct A { u8 f0[0x18]; u8 f18[0x48]; u8 f60[0x6c]; u8 fcc[1]; };

void sub_081A5198(struct A *p)
{
    sub_08214514(p->f18);
    sub_08013B74(p->fcc);
    sub_0821FE6C(p->f60);
}
