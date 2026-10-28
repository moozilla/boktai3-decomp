#include "global.h"
#include "engine.h"
#include "input.h"
#include "ram.h"
// Tentative family name: actor animation/state handlers; specific actor identity unknown.
struct AnimationPosition {
    u32 packedXY, packedZ;
};
struct AnimationActor {
    u8 pad0[0xaa];
    u8 animation, mode, padAC[2], state, inputMode, active;
    u8 padB1[0xc4 - 0xb1];
    u32 timer;
    u8 padC8[0x17c - 0xc8];
    u32 animationTimer;
    s32 sequence;
    u32 animationFlag;
    u8 pad188[0x49c - 0x188];
    s32 animationIndex;
    u8 pad4A0[0xccc - 0x4a0];
    struct AnimationPosition positions[9];
    u8 padD14[0xe5c - 0xd14];
    struct AnimationPosition position;
    u8 padE64[8];
    u16 duration;
    u8 padE6E[0xe96 - 0xe6e];
    s16 pendingCount;
};
s32 Mod(s32, s32);
u32 HomingProjectile_GetActiveCount(void);
static inline u8 ConsumeActivation(struct AnimationActor *p)
{
    if (p->active) {
        p->active = 0;
        p->inputMode = 0;
        return 1;
    }
    return 0;
}
void AnimationActor_UpdateProjectileSequence(struct AnimationActor *p)
{
    struct AnimationActor *q = p;
    if (ConsumeActivation(q)) {
        u32 animation = 6;
        q->animation = animation;
        p->sequence = 0;
    }
    {
        s32 *sequence = &p->sequence;
        switch (*sequence) {
            case 0:
                if (q->inputMode) {
                    if (p->pendingCount > 0) {
                        u32 animation = 10;
                        q->animation = animation;
                        *sequence = 1;
                    } else
                        goto next;
                }
                break;
            case 1:
                if (q->inputMode) {
                next:
                    *sequence = 2;
                    {
                        u32 animation = 8;
                        q->animation = animation;
                    }
                }
                break;
            case 2:
                if (q->inputMode) {
                    p->duration = 128;
                    gRandomIndex = (gRandomIndex + 1) & 1023;
                    p->position =
                        *(struct AnimationPosition
                              *)((Mod(*(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable), 9) * 8 +
                                  (u32)p) +
                                 0xccc);
                }
                if (HomingProjectile_GetActiveCount() == 0)
                    p->animationFlag = 1;
                break;
        }
    }
}
