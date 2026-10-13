#include "global.h"
struct Actor {
    u8 pad0[0x3b9];
    u8 command;
    u8 pad3ba[2];
    int argument;
    u8 pad3c0[0x61c];
    void *handle;
};
struct Actor *sub_0802140C(int);
int sub_0821ABA8(int, int);
void sub_08022AF4(void *);
void sub_08047B68(void *);
void sub_08047B58(void *);
extern u32 gUnk_0300523C;
void sub_08028E18(void)
{
    struct Actor *p = sub_0802140C(0);
    if (p && p->handle) {
        int a = sub_0821ABA8(115, 0);
        int b = sub_0821ABA8(104, 0);
        if (a)
            sub_08022AF4(p);
        else if (b)
            sub_08047B68(p->handle);
        else
            sub_08047B58(p->handle);
    }
}
