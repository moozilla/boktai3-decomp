#include "global.h"
struct P {
    u8 f0[0x340];
    u8 x;
};
extern struct P *gUnk_02000210;
void GameOver_ShowGraphics(struct P *);
void Hud_SetMode(s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void GameOver_StartFromScript(void)
{
    struct P *p = gUnk_02000210;
    if (p != 0) {
        GameOver_ShowGraphics(p);
        Hud_SetMode(7);
        p->x = 1;
        if (Script_SeekToKeyword(0x66) != 0) {
            if (Script_GetValue() != 0)
                ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1fff, 2);
        }
    }
}
