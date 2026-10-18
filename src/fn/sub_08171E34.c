#define MotionSequence_ResolveSelection sub_08171E34
#define MotionSequence_SetCallback Hud_SetInteractionCallback
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
struct MotionSequence { u8 unknown0[0x438A]; u8 selection; };
int sub_08165A30(void *);
void Sound_PlaySong(u32);
void PauseInventory_BeginConsumableDiscardSelection(void *, int);
void PauseMenu_AddSpriteFlags(void *, int, int);
s32 MessageWindow_Clear(void);
void sub_08171A5C(void *, int, u32, int);
void GameState_ClearConsumableSlot(u32);
void sub_08170D78(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void PauseInventory_UpdateConsumableDiscardAnimation(void);
void sub_08171E34(struct MotionSequence *p)
{
    int result = sub_08165A30(p);
    if (result == 0) {
        Sound_PlaySong(0xDE);
        PauseInventory_BeginConsumableDiscardSelection(p, 0);
    } else if (result == 1) {
        PauseMenu_AddSpriteFlags(p, 5, 1);
        MessageWindow_Clear();
        sub_08171A5C(p, 0, p->selection, 0x24);
        GameState_ClearConsumableSlot(p->selection);
        sub_08170D78(p);
        Hud_SetInteractionCallback(p, PauseInventory_UpdateConsumableDiscardAnimation, 1);
    }
}
