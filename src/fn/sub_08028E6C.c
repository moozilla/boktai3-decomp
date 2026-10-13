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
int sub_08028E6C(int value)
{
    struct Actor *p = sub_0802140C(0);
    if (p && p->handle && !p->command && !(gUnk_0300523C & 7)) {
        p->command = 1;
        p->argument = value;
        return 1;
    }
    return 0;
}
