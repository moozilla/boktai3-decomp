#include "global.h"
int ResultMenu_IsPhaseStart(void *);
void ResultMenu_SendCommand(void *, int, int, void *);
void Sound_PlayTrackedSong(int);
void ResultMenu_SetPhase(void *, void *);
void Transformation_Charge(void *);
void ResultGame_Finish(u8 *p)
{
    if (ResultMenu_IsPhaseStart(p)) {
        ResultMenu_SendCommand(p, 0x37, 0, 0);
        Sound_PlayTrackedSong(0xa9);
    }
    if (*(u32 *)(p + 0x3c) > 29)
        ResultMenu_SetPhase(p, Transformation_Charge);
    else
        ++*(u32 *)(p + 0x3c);
}
