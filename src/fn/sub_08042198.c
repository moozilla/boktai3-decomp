#include "global.h"

void Script_SetPc(u32);
u8 *Script_GetPc(void);
u32 sub_0821AA88(u8 *);
u32 sub_0821AA00(u8 *);

void sub_08042198(u8 *p, s32 n)
{
    u32 a = 0;
    u32 b = 0;
    s32 i;
    *(s32 *)(p + 0x24) += n;
    if (*(s32 *)(p + 0x24) < 0)
        *(s32 *)(p + 0x24) = b;
    Script_SetPc(*(u32 *)(p + 0x18));
    for (i = 0; i <= *(s32 *)(p + 0x24); i++) {
        if (Script_GetPc() == 0) {
            *(s32 *)(p + 0x24) = i;
            break;
        }
        a = sub_0821AA88(Script_GetPc());
        b = sub_0821AA00(Script_GetPc());
    }
    *(u32 *)(p + 0x1c) = a;
    *(u32 *)(p + 0x20) = b;
}
