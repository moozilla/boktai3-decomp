#include "global.h"
struct S {
    u8 f[0xB3D];
    u8 st;
    u8 g[0xB40 - 0xB3E];
};
s32 Armor_TryStartAction(void *, s32);
void ActorController_ForceAction(void *, s32);
void sub_08184FE8(void *, s32);
void Armor_UpdateAction48Completion(struct S *p)
{
    u8 *q = &p->st;
    switch (*q) {
        case 0:
            if (Armor_TryStartAction(p, 0x30) != 0)
                *q = 1;
            break;
        case 1:
            if (*(u8 *)((u8 *)p + 0xB40) == 0xff)
                *q = 0xff;
            break;
    }
}
