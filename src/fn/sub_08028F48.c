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
void sub_08013B74(void *);
void sub_0821FE6C(void *);
void sub_082195E0(void *);
void sub_08021578(void *);
int sub_08028F48(u8 *p)
{
    sub_08013B74(p + 0x2a0);
    sub_0821FE6C(p + 0x1cc);
    sub_082195E0(p + 0x240);
    sub_08021578(p);
    return 0;
}
