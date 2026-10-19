#define Hresvelgr_UpdateAlternatingMotion    sub_081A063C
#include "global.h"
struct HresvelgrActor {
    u8 unknown[0x32c];
};
s32 Callback_InvokeActor(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
void sub_0819CA38(struct HresvelgrActor *);
s32 sub_0819CB7C(struct HresvelgrActor *);
void sub_081A063C(struct HresvelgrActor *actor, s32 ticks)
{
    u8 *p = (u8 *)actor;
    u32 mask = 0x600048;
    u32 *flags = (u32 *)(p + 0x9c);
    *flags |= mask;
    if (p[0xb1]) {
        p[0xb1] = 0;
        SceneActors_StartUnmirroredAnimation(p, p + 0x194, p + 0x174, 1, 1, 4);
        p[0x32b] = (p[0x32b] + 1) & 1;
        sub_0819CA38(actor);
    }
    {
        u32 mask = 0x200000;
        *flags |= mask;
    }
    if (p[0xb2])
        Sound_PlaySong(0x3d9);
    if (!(u8)Callback_InvokeActor(p, *(void **)(p + 0x104))) {
        if (sub_0819CB7C(actor) || ticks > 0x80)
            p[0xaf] = 1;
    }
}
