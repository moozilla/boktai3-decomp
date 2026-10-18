#include "global.h"
extern u32 gUnk_030053F4;
void Sound_StopSong(s32);
void SceneLifecycle_RequestRestart(s32);
static inline void andm(u32 *a, u32 m)
{
    *a &= m;
}
void GameOver_StopPresentation(void)
{
    andm(&gUnk_030053F4, ~0x400);
    Sound_StopSong(2);
    SceneLifecycle_RequestRestart(0);
}
