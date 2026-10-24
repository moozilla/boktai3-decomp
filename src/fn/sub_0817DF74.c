// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct TimedEffectActor {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[2];
    u8 complete;
    u8 padA3;
    u32 timer;
    u8 padA8[7];
    u8 pending;
    u8 padB0;
    u8 restart;
    u8 padB2[0x52];
    void *callback;
    u8 pad108[0x1A4];
    u8 animationState[0x20];
    u8 animation;
};
void SceneActors_ConfigureEffect(void *, unsigned, int, int);
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
static inline void UpdateSequence(struct TimedEffectActor *p, u32 complete)
{
    if (complete == 0) {
        if (p->timer == 0) {
            u16 animation;
            u8 *a = &p->animation;
            u8 *st = p->animationState;
            animation = 0x148;
            asm("" : "+r"(animation));
            SceneActors_StartEightDirectionAnimation(p, a, st, animation, 3, 1, 4);
        }
        p->timer++;
        if (p->timer > 0xB4) {
            p->pending = 1;
            p->complete = 1;
            p->timer = complete;
        } else if (p->timer <= 7) {
            u32 flags = 4;
            p->flags |= flags;
        }
    }
}
void ActorEffect_UpdateTimedSequence(struct TimedEffectActor *p)
{
    if (p->restart) {
        p->restart = 0;
        p->complete = 0;
        p->timer = 0;
        Sound_PlaySong(0x57A);
        Sound_PlaySong(0x13C);
        SceneActors_ConfigureEffect(p, 5, 0xB4, 0xB4);
    }
    {
        u32 flags = 0x400040;
        p->flags |= flags;
    }
    if ((u8)Callback_InvokeActor(p, p->callback) == 0)
        UpdateSequence(p, p->complete);
}
