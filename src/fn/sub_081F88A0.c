#include "global.h"
// Tentative family name: actor animation/state handlers; specific actor identity unknown.
#define AnimationActor_UpdateRepeatedAttack sub_081F88A0
struct AnimationPosition {
    u32 packedXY, packedZ;
};
struct AnimationActor {
    u8 pad0[0xaa];
    u8 animation;
    u8 padAB[4];
    u8 inputMode, active;
    u8 padB1[0x180 - 0xb1];
    s32 sequence;
    u32 animationFlag;
    u8 pad188[0xe5a - 0x188];
    u16 completionCount;
    struct AnimationPosition target;
    u8 padE64[0xede - 0xe64];
    s16 repeatCount;
};
void sub_081F3130(struct AnimationActor *, struct AnimationPosition *);
void Sound_PlaySong(u32);
static inline u8 ConsumeActivation(struct AnimationActor *p)
{
    if (p->active) {
        p->active = 0;
        p->inputMode = 0;
        return 1;
    }
    return 0;
}
void sub_081F88A0(struct AnimationActor *p)
{
    struct AnimationActor *q = p;
    if (ConsumeActivation(p)) {
        sub_081F3130(p, &p->target);
        {
            u32 animation = 4;
            q->animation = animation;
        }
        p->sequence = 0;
        p->completionCount = 0;
        p->repeatCount = 0;
    }
    switch (p->sequence) {
        case 0:
            if (q->inputMode) {
                p->sequence = 1;
                {
                    u32 animation = 10;
                    q->animation = animation;
                }
                Sound_PlaySong(0x55d);
            }
            break;
        case 1:
            if (q->inputMode) {
                if (++p->repeatCount > 2) {
                    p->sequence++;
                    q->animation = 1;
                } else {
                    u32 animation = 10;
                    q->animation = animation;
                }
            }
            break;
        case 2:
            p->animationFlag = 1;
            p->completionCount++;
            break;
    }
}
