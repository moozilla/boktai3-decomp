#include "global.h"
struct Vanargand {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[7];
    u8 finished;
    u8 padB0;
    u8 pending;
    u8 ready;
    u8 padB3[0x18C - 0xB3];
    u8 animation[0x60];
    u8 drawable[0x5B0 - 0x1EC];
    u16 w5B0;
    u8 pad5B2[0x5B8 - 0x5B2];
    u16 w5B8;
    u16 w5BA;
};
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void Vanargand_UpdateAnimationGate(struct Vanargand *p, s32 frame)
{
    u8 *anim = p->animation;
    u8 phase;
    Vanargand_SetPendingCue(p, 3);
    if (p->pending) {
        p->pending = 0;
        p->w5B0 = 16;
        p->phase = 0;
        p->timer = 0;
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            if (frame == 16)
                SceneActors_StartUnmirroredAnimation(p, anim, p->drawable, 15, 2, 4);
            if (frame > 16 && p->ready) {
                p->phase = 1;
                p->timer = phase;
                {
                    u16 v = 4;
                    u32 off = 0x5B8;
                    *(u16 *)((u8 *)p + off) = v;
                    *(u16 *)((u8 *)p + off + 2) = v;
                }
                return;
            }
            break;
        case 1:
            p->finished = phase;
            return;
    }
    {
        u32 mask = 5;
        p->flags |= mask;
    }
}
