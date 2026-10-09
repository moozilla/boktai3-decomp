#include "global.h"
void sub_08072C40(void *);
void sub_082195E0(void *);
void sub_08214514(void *);
void sub_0821FE6C(void *);
void sub_08225938(void *);
void sub_0823A388(void *);
void sub_082384C8(void *);
extern u32 gUnk_02000580[];
extern u16 gUnk_02000544;
s32 sub_082439EC(u8 *p)
{
    sub_08072C40(p + 0x2E8);
    sub_082195E0(p + 0x8C);
    sub_08214514(p + 0x14C);
    sub_0821FE6C(p + 0x230);
    sub_08225938(p + 0x24);
    sub_0823A388(p);
    sub_082384C8(p);
    gUnk_02000580[*(u32 *)(p + 0x18)] = 0;
    gUnk_02000544--;
    return 0;
}
