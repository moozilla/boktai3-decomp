#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Reviewed original extent: 492 bytes; remaining pointer reuse and loop allocation differ.
#include "modifier_list.h"
struct ModifierCapacityInfo {
    u8 id, kind, unknown2, capacity;
    u16 value;
};
s32 ModifierMenu_ClampSourceId(u32, s32);
s32 StateFlags_GetCategoryBit(s32, s32);
s32 GameFlag_FindFirstSetIndex(s32, s32);
s32 ScriptRecord_FindByteModifier(struct ModifierCapacityInfo *, s32);
extern const u8 gUnk_086058E8[];
void ModifierMenu_ValidateSavedSelection(struct ModifierMenu *menu)
{
    struct ModifierCapacityInfo info;
    u32 mask;
    u8 i;
    s32 count;
    s32 capacity;
    (*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[0] =
        ModifierMenu_ClampSourceId(0, (s16)(*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[0]);
    (*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[2] =
        ModifierMenu_ClampSourceId(1, (s16)(*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[2]);
    (*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[1] =
        ModifierMenu_ClampSourceId(2, (s16)(*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[1]);
    (*((typeof(gUnk_02000710) *)0x02000710))->unk81A =
        ModifierMenu_ClampSourceId(5, (*((typeof(gUnk_02000710) *)0x02000710))->unk81A);
    if (!StateFlags_GetCategoryBit(0, (s16)(*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[0]))
        (*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[0] = GameFlag_FindFirstSetIndex(0, 6);
    if (!StateFlags_GetCategoryBit(1, (s16)(*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[2]))
        (*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[2] = GameFlag_FindFirstSetIndex(1, 6);
    if (!StateFlags_GetCategoryBit(2, (s16)(*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[1]))
        (*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[1] = GameFlag_FindFirstSetIndex(2, 6);
    if (!StateFlags_GetCategoryBit(3, (s16)(*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[3]))
        (*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[3] = -1;
    ScriptRecord_FindByteModifier(&info, (s16)(*((typeof(gUnk_02000710) *)0x02000710))->unk7A0[2]);
    capacity = info.capacity;
    mask = 0;
    for (i = 0; i < 4; i++) {
        s16 id = ((s16 *)(*((typeof(gUnk_02000710) *)0x02000710))->modifierIdsBytes)[i];
        if ((id >= 0) && StateFlags_GetCategoryBit(4, id))
            mask |= 1 << ((s16 *)(*((typeof(gUnk_02000710) *)0x02000710))->modifierIdsBytes)[i];
    }

    for (i = 0; i < 4; i++)
        ((s16 *)(*((typeof(gUnk_02000710) *)0x02000710))->modifierIdsBytes)[i] = -1;

    count = 0;
    i = 0;
    if (mask & 1) {
        ((s16 *)(*((typeof(gUnk_02000710) *)0x02000710))->modifierIdsBytes)[0] = 0;
        count = 1;
        if (count >= capacity)
            goto secondary;
    }
    do {
        i++;
        if (i > 31)
            break;
        if (mask & (1 << i)) {
            ((s16 *)(*((typeof(gUnk_02000710) *)0x02000710))->modifierIdsBytes)[count] = i;
            count++;
            if (count >= capacity)
                break;
        }
    } while (1);
secondary:
    if (!StateFlags_GetCategoryBit(5, (*((typeof(gUnk_02000710) *)0x02000710))->unk81A))
        (*((typeof(gUnk_02000710) *)0x02000710))->unk81A =
            GameFlag_FindFirstSetIndex(5, (gUnk_086058E8[2] * 3) * 2);
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", ModifierMenu_ValidateSavedSelection);
#endif
