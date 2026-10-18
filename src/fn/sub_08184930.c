#define gActorControllerDelayedTransitionAnimations gUnk_0824EEC0
#define Armor_SetAnimationFromStart                 Armor_SetAnimationFromStart /* Legacy name in the local ELF. */
#include "global.h"

struct ActorAnimationPart {
    u16 unknown0, unknown2, unknown4, flags;
};
struct ActorCollisionState {
    u32 unknown0, flags;
};

/* Tentative layout shared by these animation-transition callbacks. */
struct ActorController {
    u8 pad0[0xC4];
    u16 delay;
    u8 padC6[0xA6C];
    u16 flags;
    u8 padB34[5];
    u8 facing;
    u8 padB3A[6];
    u8 state, result;
};

extern u32 gActorControllerDelayedTransitionAnimations[];

void Armor_SetAnimationFromStart(u32 a, u32 b);

void ActorController_FinishDelayedTransition(u8 *p)
{
    u8 *state = p + 0xB40;
    if (*state == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart(
            (u32)p,
            (u32)&gActorControllerDelayedTransitionAnimations[((p[0xB39] + 0x60) >> 6) & 3]);
        *state = 0xFF;
        p[0xB41] = 1;
        ((struct ActorController *)p)->delay = 0x5B;
        ((struct ActorAnimationPart *)(p + 0x238))->flags &= ~four;
        ((struct ActorAnimationPart *)(p + 0x1E4))->flags &= ~four;
        ((struct ActorAnimationPart *)(p + 0x28C))->flags &= ~four;
        ((struct ActorAnimationPart *)(p + 0x190))->flags |= four;
        {
            u32 one = 1;
            ((struct ActorCollisionState *)(p + 0x180))->flags &= ~one;
        }
        *(u16 *)(p + 0xB32) &= ~0x20;
    }
}
