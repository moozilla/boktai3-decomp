#include "global.h"
s32 SceneHud_UpdateBinaryChoice(void *);
void Sound_PlaySong(s32);
void GameState_SortConsumableSlots(s32);
void PauseInventory_ReturnToConsumableActions(void *);
void PauseInventory_UpdateConsumableSortConfirmation(u8 *p)
{
    s32 r = SceneHud_UpdateBinaryChoice(p);
    if (r == 0) {
        Sound_PlaySong(0xde);
        PauseInventory_ReturnToConsumableActions(p);
    } else if (r == 1) {
        Sound_PlaySong(0xdd);
        GameState_SortConsumableSlots(0);
        p[0x4388] = 0;
        p[0x4389] = 0;
        p[0x438a] = 0;
        PauseInventory_ReturnToConsumableActions(p);
    }
}
