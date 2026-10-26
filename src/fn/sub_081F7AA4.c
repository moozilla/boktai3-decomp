#include "global.h"
#include "engine.h"
// Tentative family name: actor animation/state handlers; specific actor identity unknown.
#define gPreviewKeys gUnk_03005260
#define gRandomTable gUnk_0203B400
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
    u8 pad180[0x49c - 0x180];
    s32 animationIndex;
    u8 pad4A0[0xccc - 0x4a0];
    struct AnimationPosition positions[9];
    u8 padD14[0xe5c - 0xd14];
    struct AnimationPosition position;
    u8 padE64[8];
    u16 duration;
};
extern u16 gUnk_03005260[];
extern const u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void AnimationActor_ProcessKeys(struct AnimationActor *p)
{
    struct AnimationActor *q = p;
    if (gUnk_03005260[1] & 0x100) {
        if (++p->animationIndex > 36)
            p->animationIndex = 0;
        p->animationTimer = 0;
        p->animation = p->animationIndex;
    }
    if (gUnk_03005260[1] & 0x200) {
        q->duration = 128;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        q->position =
            *(struct AnimationPosition
                  *)((Mod(*(const u16 *)(gRandomIndex * 2 + (u32)gUnk_0203B400), 9) * 8 + (u32)q) +
                     0xccc);
        {
            u32 state = 2;
            p->state = state;
        }
        p->timer = 0;
        p->active = 1;
    }
    if (p->inputMode == 1 && q->mode != 3)
        p->animation = 1;
}
