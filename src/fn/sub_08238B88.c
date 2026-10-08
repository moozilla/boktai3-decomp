#include "global.h"

struct S08238B88 {
    u8 f0[0xC6]; u16 fC6; u8 fC8[0xC]; u32 fD4;
    u8 fD8[0x7FC - 0xD8]; u32 f7FC;
    u8 f800[0x82E - 0x800]; u16 f82E; u8 f830[4]; u32 f834;
    u8 f838[0x860 - 0x838]; u8 f860;
};

void sub_08238B88(struct S08238B88 *p)
{
    p->f82E = p->fC6;
    p->f834 = p->fD4;
    p->f7FC &= ~1;
    p->f860 = 1;
}
