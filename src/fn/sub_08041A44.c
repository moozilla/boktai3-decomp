#ifdef NONMATCHING
// fuzzy: 71.0%
// Reviewed original extent: 492 bytes; remaining pointer reuse and loop allocation differ.
#include "engine.h"
#define ModifierMenu_ValidateSavedSelection sub_08041A44
struct ModifierCapacityInfo {
    u8 id, kind, unknown2, capacity;
    u16 value;
};
s32 sub_08041A20(u32, s32);
s32 sub_0811DD94(s32, s32);
s32 sub_0811DE3C(s32, s32);
s32 sub_0811E5D0(struct ModifierCapacityInfo *, s32);
extern const u8 gUnk_086058E8[];
void ModifierMenu_ValidateSavedSelection(void)
{
    struct ModifierCapacityInfo info;
    u32 mask;
    s32 i;
    s32 count;
    s32 capacity;
    gUnk_02000710->unk7A0[0] = sub_08041A20(0, (s16)gUnk_02000710->unk7A0[0]);
    gUnk_02000710->unk7A0[2] = sub_08041A20(1, (s16)gUnk_02000710->unk7A0[2]);
    gUnk_02000710->unk7A0[1] = sub_08041A20(2, (s16)gUnk_02000710->unk7A0[1]);
    gUnk_02000710->unk81A = sub_08041A20(5, gUnk_02000710->unk81A);
    if (!sub_0811DD94(0, (s16)gUnk_02000710->unk7A0[0]))
        gUnk_02000710->unk7A0[0] = sub_0811DE3C(0, 6);
    if (!sub_0811DD94(1, (s16)gUnk_02000710->unk7A0[2]))
        gUnk_02000710->unk7A0[2] = sub_0811DE3C(1, 6);
    if (!sub_0811DD94(2, (s16)gUnk_02000710->unk7A0[1]))
        gUnk_02000710->unk7A0[1] = sub_0811DE3C(2, 6);
    if (!sub_0811DD94(3, (s16)gUnk_02000710->unk7A0[3]))
        gUnk_02000710->unk7A0[3] = -1;
    sub_0811E5D0(&info, (s16)gUnk_02000710->unk7A0[2]);
    capacity = info.capacity;
    mask = 0;
    for (i = 0; i < 4; i++) {
        s32 id = ((s16 *)gUnk_02000710->modifierIdsBytes)[i];
        if (id >= 0 && sub_0811DD94(4, id))
            mask |= 1 << ((s16 *)gUnk_02000710->modifierIdsBytes)[i];
    }
    for (i = 0; i < 4; i++)
        ((s16 *)gUnk_02000710->modifierIdsBytes)[i] = -1;
    count = 0;
    i = 0;
    if (mask & 1) {
        ((s16 *)gUnk_02000710->modifierIdsBytes)[0] = 0;
        count = 1;
        if (count >= capacity)
            goto secondary;
    }
    do {
        i++;
        if (i > 31)
            break;
        if (mask & (1 << i)) {
            ((s16 *)gUnk_02000710->modifierIdsBytes)[count] = i;
            count++;
            if (count >= capacity)
                break;
        }
    } while (1);
secondary:
    if (!sub_0811DD94(5, gUnk_02000710->unk81A))
        gUnk_02000710->unk81A = sub_0811DE3C(5, gUnk_086058E8[2] * 3 * 2);
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", sub_08041A44);
#endif
