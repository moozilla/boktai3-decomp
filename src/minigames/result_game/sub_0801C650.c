#include "global.h"
int ResultMenu_IsPhaseStart(void *);
void ResultMenu_SendCommand(void *, int, int, void *);
void ResultMenu_SetPhase(void *, void *);
void ResultGame_Update(void *);
void Sound_PlaySong(int);
void ResultMenu_Countdown(u8 *p)
{
    if (ResultMenu_IsPhaseStart(p))
        ResultMenu_SendCommand(p, 0x31, 0, 0);
    if (*(u32 *)(p + 0x3c) > 239)
        ResultMenu_SetPhase(p, ResultGame_Update);
    else {
        if (Mod(*(u32 *)(p + 0x3c), 61) == 60)
            Sound_PlaySong(0x21c);
        ++*(u32 *)(p + 0x3c);
    }
}
