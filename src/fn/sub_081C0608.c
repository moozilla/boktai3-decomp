#include "global.h"
struct S { u8 f[0x24]; u32 a24; u32 a28; u32 a2c; };
extern u16 gUnk_03004BD8;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_081C04B8(void *);
void sub_081C0554(void *);
u32 sub_081C0608(struct S *p)
{
    s32 v;
    { u16 *g = &gUnk_03004BD8; u32 m = 0xFFFFF0FF; *g = *g & m; }
    sub_081C04B8(p);
    if (Script_SeekToKeyword(0x66)) v = Script_GetValue();
    else v = 6;
    p->a28 = v;
    v = Script_SeekToKeyword(0x73);
    if (v) v = Script_GetValue();
    p->a2c = v;
    p->a24 = 0;
    sub_081C0554(p);
    return 0;
}
