#include "global.h"
struct A { u8 p[4]; void *f4; };
extern struct A *gUnk_030052F4;
void sub_0821BB98(void);
void sub_0821BBBC(void);
void sub_0821BBF8(void);
void sub_0821BC4C(void *v)
{
    gUnk_030052F4->f4 = v;
    sub_0821BB98();
    sub_0821BBBC();
    sub_0821BBF8();
}
