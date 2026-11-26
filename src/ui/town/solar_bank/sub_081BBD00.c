#include "global.h"
#define SpriteMenu_SetState SolarBank_SetState
struct SpriteMenuState {
    u8 pad0[0x18];
    u8 data[0x40];
    u8 animation[0x6B4];
    u16 timer;
};
s32 Anim_Step(void *, void *);
void SolarBank_SetState(void *, void (*)(void));
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void SolarBank_UpdateExitDelay(void);
void SpriteMenu_UpdateTimedFade(struct SpriteMenuState *menu)
{
    Anim_Step(menu->animation, menu->data);
    if (++menu->timer > 149) {
        ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
        SolarBank_SetState(menu, SolarBank_UpdateExitDelay);
    }
}
