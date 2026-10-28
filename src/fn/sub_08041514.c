#include "modifier_list.h"
#include "input.h"
extern const u8 gUnk_086058E8[];
void ModifierMenu_BeginClose(struct ModifierMenu *);
void Sound_PlaySong(u32);
s32 Input_UpdateKeyRepeat(u32, u8 *, u32, s32, s32);
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
void ModifierMenu_HandleOverviewInput(struct ModifierMenu *menu)
{
    s32 close, change;
    if (menu->inputDelay) {
        menu->inputDelay--;
        return;
    }
    close = 0;
    if (menu->specialHeader) {
        if (gPadState[1] & 8) {
            close = 1;
            Sound_PlaySong(0xdd);
        }
    } else {
        if (gPadState[1] & 2) {
            close = 1;
            Sound_PlaySong(0x119);
        }
    }
    if (close) {
        ModifierMenu_BeginClose(menu);
        return;
    }
    if (gPadState[1] & 1) {
        s32 mode = menu->mode;
        menu->choice.word = menu->savedChoices[mode];
        menu->state = 1;
        menu->stateFrame = close;
        if (menu->choice.cursor.category >= gUnk_086058E8[mode])
            menu->choice.cursor.category = close;
        menu->dirtyFlags = -1;
        Sound_PlaySong(0xdd);
        return;
    }
    change = ModifierMenu_ReadRepeatPair(&menu->modeRepeat, 0x20, 0x10);
    if (change != 0) {
        menu->mode += change;
        if (menu->mode > 2)
            menu->mode = 0;
        else if (menu->mode < 0)
            menu->mode = 2;
        ModifierMenu_MarkDirty(menu, ~0x20);
        Sound_PlaySong(0xdc);
    }
}
