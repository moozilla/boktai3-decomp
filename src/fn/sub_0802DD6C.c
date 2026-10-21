// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
/* Actor_PlayFootstepSound: plays a step sound when the animation hits one of two marked frames */

struct StepInfo {
    u8 pad0[3];
    u8 sound;
    s8 frames[4];
};

struct StepActor {
    u8 pad0[0xD4];
    struct DisplayAnimation anim;
    u8 padX[0x216 - 0xD4 - sizeof(struct DisplayAnimation)];
    u8 enabled;
    u8 pad217[0x2BC - 0x217];
    struct StepInfo *info;
};

struct StepSounds {
    u16 a, b;
};
extern const struct StepSounds gUnk_086055D4[];

void Actor_PlayFootstepSound(struct StepActor *actor, u32 mode)
{
    u32 frame;
    u32 idx;
    if (!actor->enabled)
        return;
    if (mode == 1)
        idx = 0;
    else if (mode == 3)
        idx = 2;
    else
        return;
    if (DisplayAnimation_GetFrameBoundary(&actor->anim, &frame)) {
        struct StepInfo *info = actor->info;
        s8 *fr = info->frames;
        s8 *f = fr + idx;
        s32 f0 = f[0];
        if (f0 == frame)
            Sound_PlaySong(gUnk_086055D4[info->sound].a);
        else if (f[1] == frame) {
            const u16 *t = (const u16 *)gUnk_086055D4;
            u32 s = info->sound << 2;
            asm volatile("" : "+r"(s));
            t += 1;
            Sound_PlaySong(*(u16 *)((u8 *)t + s));
        }
    }
}
