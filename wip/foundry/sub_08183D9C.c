#define gActorControllerImmediateTransitionAnimations gUnk_0824EDE8
#define ActorController_FinishDirectionalTransition   sub_08183D9C
#define Armor_SetAnimationFromStart                   sub_0818253C /* Legacy name in the local ELF. */
#include "global.h"

struct ActorAnimationPart {
    u16 unknown0, unknown2, unknown4, flags;
};
struct ActorCollisionState {
    u32 unknown0, flags;
};

/* Tentative controller fields; offsets observed in this callback cohort. */
struct ActorController {
    u8 pad0[0xC4];
    u16 delay;
    u8 padC6[0xA5A];
    u16 saved, source;
    u8 padB24[0xE];
    u16 flags;
    u8 padB34[5];
    u8 facing;
    u8 padB3A[6];
    u8 state, result, padB42, complete;
};

extern u32 gActorControllerImmediateTransitionAnimations[];

void Armor_SetAnimationFromStart(u32 a, u32 b);

void sub_08183D9C(u8 *p)
{
    u8 *state = p + 0xB40;
    if (*state == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart(
            (u32)p,
            (u32)&gActorControllerImmediateTransitionAnimations[((p[0xB39] + 0x60) >> 6) & 3]);
        *state = 0xFF;
        p[0xB41] = 1;
        {
            u32 two = 2;
            *(u16 *)(p + 0xB32) = two | *(u16 *)(p + 0xB32);
        }
        ((struct ActorAnimationPart *)(p + 0x238))->flags &= ~four;
        ((struct ActorAnimationPart *)(p + 0x1E4))->flags &= ~four;
        ((struct ActorAnimationPart *)(p + 0x28C))->flags &= ~four;
        ((struct ActorAnimationPart *)(p + 0x190))->flags |= four;
        {
            u32 one = 1;
            ((struct ActorCollisionState *)(p + 0x180))->flags &= ~one;
        }
        *(u16 *)(0xB32 + (u32)p) &= ~0x20;
    }
}
