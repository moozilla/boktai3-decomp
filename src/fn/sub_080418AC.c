#include "modifier_list.h"
#include "input.h"
extern const u8 gUnk_086058E8[];
void TextWindow_StopPlayback(s32);
void Sound_PlaySong(u32);
void ModifierMenu_HandleChangedInput(struct ModifierMenu *menu)
{
    if (menu->stateFrame > 39 && (gPadState[1] & 3)) {
        TextWindow_StopPlayback(menu->detailWindow);
        menu->state = 0;
        menu->stateFrame = 0;
        if (menu->choice.cursor.category >= gUnk_086058E8[menu->mode])
            menu->choice.cursor.category = 0;
        menu->dirtyFlags = -1;
        Sound_PlaySong(0xdd);
    }
}
