#include "global.h"
s32 Mod(s32, s32);
void sub_0820B034(void *);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void DragonPuppy_UpdateAttackRecovery(void);
u32 DragonPuppy_UpdateAttackTiming(u8 *p)
{
    if (*(u32 *)(p + 0x164)) {
        s16 *index = (s16 *)(p + 0x186);
        s32 selected = *index * 2;
        s16 *table = (s16 *)(p + 0x188);
        if (*(s32 *)(p + 0x18) > *(s16 *)((u8 *)table + selected)) {
            sub_0820B034(p);
            {
                void *obj = p + 0x10c;
                void *res = *(void **)(p + 0x170);
                u32 value = p[0x132];
                Anim_SetSequence(obj, res, 4, value, 0);
            }
            *(u16 *)(p + 0x12c) = 30;
            *(void **)(p + 0x108) = DragonPuppy_UpdateAttackRecovery;
            *(u32 *)(p + 0x18) = 0;
            (*index)++;
            if (*index > *(s16 *)(p + 0x184) - 1)
                *index = 0;
        }
    } else {
        s32 value = Mod(*(s32 *)(p + 0x18), *(s16 *)(p + 0x134));
        if (!value) {
            sub_0820B034(p);
            Anim_SetSequence(p + 0x10c, *(void **)(p + 0x170), 4, p[0x132], value);
            *(u16 *)(p + 0x12c) = 30;
            *(u32 *)(p + 0x18) = value;
            *(void **)(p + 0x108) = DragonPuppy_UpdateAttackRecovery;
        }
    }
}
