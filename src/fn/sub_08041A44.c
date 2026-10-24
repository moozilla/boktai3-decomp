#include "modifier_list.h"

#define SAVED_GAME_STATE (*(struct GameState **)(u32) & gUnk_02000710)

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
    s32 i;
    s32 count;
    s32 capacity;
    SAVED_GAME_STATE->unk7A0[0] = ModifierMenu_ClampSourceId(0, (s16)SAVED_GAME_STATE->unk7A0[0]);
    SAVED_GAME_STATE->unk7A0[2] = ModifierMenu_ClampSourceId(1, (s16)SAVED_GAME_STATE->unk7A0[2]);
    SAVED_GAME_STATE->unk7A0[1] = ModifierMenu_ClampSourceId(2, (s16)SAVED_GAME_STATE->unk7A0[1]);
    SAVED_GAME_STATE->unk81A = ModifierMenu_ClampSourceId(5, SAVED_GAME_STATE->unk81A);
    if (!StateFlags_GetCategoryBit(0, (s16)SAVED_GAME_STATE->unk7A0[0]))
        SAVED_GAME_STATE->unk7A0[0] = GameFlag_FindFirstSetIndex(0, 6);
    if (!StateFlags_GetCategoryBit(1, (s16)SAVED_GAME_STATE->unk7A0[2]))
        SAVED_GAME_STATE->unk7A0[2] = GameFlag_FindFirstSetIndex(1, 6);
    if (!StateFlags_GetCategoryBit(2, (s16)SAVED_GAME_STATE->unk7A0[1]))
        SAVED_GAME_STATE->unk7A0[1] = GameFlag_FindFirstSetIndex(2, 6);
    if (!StateFlags_GetCategoryBit(3, (s16)SAVED_GAME_STATE->unk7A0[3]))
        ((s16 *)SAVED_GAME_STATE->unk7A0)[3] = -1;
    ScriptRecord_FindByteModifier(&info, (s16)SAVED_GAME_STATE->unk7A0[2]);
    capacity = info.capacity;
    mask = 0;
    for (i = 0; i < 4; i++) {
        if ((((s16 *)SAVED_GAME_STATE->modifierIdsBytes)[i] >= 0) &&
            StateFlags_GetCategoryBit(4, ((s16 *)SAVED_GAME_STATE->modifierIdsBytes)[i]))
            mask |= 1 << ((s16 *)SAVED_GAME_STATE->modifierIdsBytes)[i];
    }

    for (i = 0; i < 4; i++)
        ((s16 *)gUnk_02000710->modifierIdsBytes)[i] = -1;

    count = 0;
    for (i = 0; i < 32; i++) {
        if (mask & (1 << i)) {
            ((s16 *)gUnk_02000710->modifierIdsBytes)[count] = i;
            count++;
            if (count >= capacity)
                break;
        }
    }
    if (!StateFlags_GetCategoryBit(5, gUnk_02000710->unk81A))
        gUnk_02000710->unk81A = GameFlag_FindFirstSetIndex(5, (gUnk_086058E8[2] * 3) * 2);
}
