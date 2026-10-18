#define ActorController_UpdateResetMotionTransition sub_08182F84
#define gActorControllerResetMotionAnimations       gUnk_0824EBE4

#include "global.h"

struct ActorControllerPart {
    u16 unk0, unk2, unk4, flags;
};

struct ActorControllerMotion {
    u32 unk0, flags;
};

struct ActorController {
    u8 unk0[0xC4];
    u16 speed;
    u8 unkC6[0xB32 - 0xC6];
    u16 flags;
    u8 unkB34[5];
    u8 direction;
    u8 unkB3A[6];
    u8 transitionState;
    u8 transitionComplete;
    u8 unkB42;
    u8 animationComplete;
};

extern const u32 gActorControllerResetMotionAnimations[];
void Armor_SetAnimationFromStart(u32 actor, u32 animation);
void sub_081822C0(struct ActorController *actor);

/* Tentative reset-motion transition; the optional argument sets speed to 120. */
void sub_08182F84(struct ActorController *p, s32 setSpeed)
{
    u8 *s = &p->transitionState;
    u8 st = *s;

    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                (u32)p,
                (u32)&gActorControllerResetMotionAnimations[((p->direction + 0x60) >> 6) & 3]);
            (*s)++;
            p->transitionComplete = st;
            sub_081822C0(p);
            if (setSpeed != 0)
                p->speed = 120;
            ((struct ActorControllerPart *)((u8 *)p + 0x238))->flags &= ~four;
            ((struct ActorControllerPart *)((u8 *)p + 0x1E4))->flags &= ~four;
            ((struct ActorControllerPart *)((u8 *)p + 0x28C))->flags &= ~four;
            ((struct ActorControllerPart *)((u8 *)p + 0x190))->flags |= four;
            {
                u32 one = 1;
                ((struct ActorControllerMotion *)((u8 *)p + 0x180))->flags &= ~one;
            }
            p->flags &= ~0x20;
            break;
        }
        case 1:
            if (p->animationComplete != 0) {
                *s = 0xFF;
                p->transitionComplete = st;
            }
            break;
    }
}
