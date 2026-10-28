// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
#include "engine_globals.h"

// Draft: effect timer tick (screen transition at 0, collider refresh, shake).
// Open: the original recomputes the u16 extension of the counter for the second
// compare, and loads the pause-flag word after building the mask.

void Sound_PlaySong(u32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
s32 Collider_Refresh(void *);
void Task_Kill(void *);
void Camera_RequestShake(s32);

static inline u32 Mask(u32 *a, u32 m)
{
    return *a & m;
}
void sub_082356E4(u8 *p)
{
    u16 *counter = (u16 *)(p + 0x34e);
    u16 *q;
    s32 n;
    if (*counter == 0) {
        gEnginePauseFlags = Mask(&gEnginePauseFlags, ~4);
        Sound_PlaySong(0x67);
        ScreenTransition_SendCommand(2, 4, 0x1f, 0x1f, 0x1f, 0x1fff, 1);
    }
    if (*counter <= 5) {
        Collider_Refresh(p + 0x94);
    } else if (*counter == 6) {
        gEngineFlags &= ~1;
    }
    q = (u16 *)(p + 0x34e);
    n = *q + 1;
    *q = n;
    if ((u16)n > 0x2f) {
        Task_Kill(p);
    } else if (({
                   asm("" : "+r"(n));
                   (u16) n;
               }) <= 0xf) {
        Camera_RequestShake(0x20);
    } else {
        Camera_RequestShake(0x30 - *q);
    }
}
