#include "global.h"
struct A {
    u8 p[4];
    void *f4;
};
extern struct A *gUnk_030052F4;
void *ScriptProc_Start(u32, u16);
void sub_0821BB98(void);
void sub_0821BBBC(void);
void sub_0821BBF8(void);
u32 sub_0821BC1C(u16 v)
{
    gUnk_030052F4->f4 = ScriptProc_Start(0xAE1B, v);
    sub_0821BB98();
    sub_0821BBBC();
    sub_0821BBF8();
    return 0;
}
