#include "modifier_list.h"
#include "input.h"
extern const u8 gUnk_086058E8[];
void Sound_PlaySong(u32);
u32 StateFlags_GetCategoryBit(s32, s32);
s32 Input_UpdateKeyRepeat(u32, u8 *, u32, s32, s32);
void ModifierMenu_SelectPendingSource(struct ModifierMenu *);
s32 ModifierMenu_ApplyPendingSource(struct ModifierMenu *);
static inline s32 ModifierMenu_ReadRepeatPair(u8 *counterArg, u32 negative, u32 positive)
{
    u8 *counter = counterArg;
    u8 temp;
    s32 period = 15;
    s32 change;
    temp = *counter;
    change = Input_UpdateKeyRepeat(0, &temp, negative, -1, period);
    if (change == 0) {
        u32 previous = temp;
        temp = *counter;
        change = Input_UpdateKeyRepeat(0, &temp, positive, 1, period);
        if (change == 0 && previous != 0)
            temp = previous;
    }
    *counter = temp;
    return change;
}
static inline void ModifierMenu_MarkDirty(struct ModifierMenu *menu, u32 mask)
{
    menu->dirtyFlags |= mask;
}
void ModifierMenu_HandleSelectionInput(struct ModifierMenu *menuArg)
{
    struct ModifierMenu *menu = menuArg;
    s32 change = ModifierMenu_ReadRepeatPair(&menu->categoryRepeat, 0x200, 0x100);
    if (!change)
        change = ModifierMenu_ReadRepeatPair(&menu->modeRepeat, 0x20, 0x10);
    if (change) {
        s32 category = menu->choice.cursor.category + change;
        s32 count = gUnk_086058E8[menu->mode];
        if (category >= count)
            category = 0;
        else if (category < 0)
            category = count - 1;
        menu->choice.cursor.category = category;
        ModifierMenu_SelectPendingSource(menu);
        menu->dirtyFlags = -1;
        Sound_PlaySong(0x10a);
    }
    change = ModifierMenu_ReadRepeatPair(&menu->indexRepeat, 0x40, 0x80);
    if (change) {
        s32 index = menu->choice.cursor.index + change;
        if (index > 5)
            index = 0;
        else if (index < 0)
            index = 5;
        menu->choice.cursor.index = index;
        ModifierMenu_MarkDirty(menu, 0x18);
        Sound_PlaySong(0xdc);
        ModifierMenu_SelectPendingSource(menu);
    }
    if (gPadState[1] & 1) {
        if (ModifierMenu_ApplyPendingSource(menu) >= 0)
            Sound_PlaySong(0xdd);
        else {
            Sound_PlaySong(0x192);
            if (menu->state == 1 && menu->mode == 1) {
                s32 category = menu->choice.cursor.category;
                s32 index = menu->choice.cursor.index;
                if (StateFlags_GetCategoryBit(4, category * 3 * 2 + index))
                    menu->errorFlash = 0x40;
            }
        }
        ModifierMenu_MarkDirty(menu, 0x38);
    }
    if (gPadState[1] & 2) {
        menu->savedChoices[menu->mode] = menu->choice.word;
        if (menu->changed != 0) {
            menu->state = 2;
            menu->stateFrame = 0;
            if (menu->choice.cursor.category >= gUnk_086058E8[menu->mode])
                menu->choice.cursor.category = 0;
            menu->changed = 0;
        } else {
            menu->state = 0;
            menu->stateFrame = 0;
            if (menu->choice.cursor.category >= gUnk_086058E8[menu->mode])
                menu->choice.cursor.category = 0;
        }
        menu->dirtyFlags = -1;
        Sound_PlaySong(0xde);
    }
}
