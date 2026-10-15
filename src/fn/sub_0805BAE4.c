#include "global.h"
void Sound_PlaySong(u32);
void TetherActor_BeginDirectedState(u8 *);
void ScenePanel_SetEffectState(u8 *, u32);
void ScenePanel_CallPendingScript(u8 *, u32);
void sub_0805BAE4(u8 *p)
{
    if (*(u16 *)(p + 0x12e8) == 0) {
        Sound_PlaySong(0x1eb);
        TetherActor_BeginDirectedState(p + 0x510);
        ScenePanel_SetEffectState(p, 0xc);
        ScenePanel_CallPendingScript(p, 5);
    }
}
