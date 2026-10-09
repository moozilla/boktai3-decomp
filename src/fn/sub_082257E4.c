#include "global.h"
struct CommandSet { void *next; u32 count; const void *entries; };
extern struct CommandSet gUnk_030025E8;
extern const u8 gUnk_08E8791C[];
void sub_082257E0(void);
void sub_0821ABDC(void);
u32 sub_0821ABE8(struct CommandSet *);
u32 sub_082257E4(void)
{
    sub_082257E0();
    sub_0821ABDC();
    gUnk_030025E8.next = 0;
    gUnk_030025E8.count = 8;
    gUnk_030025E8.entries = gUnk_08E8791C;
    return sub_0821ABE8(&gUnk_030025E8);
}
