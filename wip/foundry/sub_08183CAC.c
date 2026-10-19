#define gActorControllerFlaggedTransitionAnimations      gUnk_0824EDD8
#define Armor_SetAnimationFromStart                      sub_0818253C /* Legacy name in the local ELF. */
#define ActorController_UpdateFlaggedAnimationTransition sub_08183CAC
#include "global.h"

struct ActorController {
    u8 pad[0xB20];
    u16 saved, source;
    u8 padB24[0xE];
    u16 flags;
    u8 padB34[5];
    u8 facing;
    u8 padB3A[6];
    u8 state, result, padB42, complete;
};
struct ActorAnimationPart {
    u16 unknown0, unknown2, unknown4, flags;
};
struct ActorCollisionState {
    u32 unknown0, flags;
};

extern u32 gActorControllerFlaggedTransitionAnimations[];

void Armor_SetAnimationFromStart(u32 a, u32 b);
void Sound_PlaySongChecked(u32 a);

void sub_08183CAC(struct ActorController *actor)
{
    u8 *p = (u8 *)actor;
    u8 *s = &actor->state;
    u8 st = *s;

    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                (u32)p,
                (u32)&gActorControllerFlaggedTransitionAnimations[((actor->facing + 0x60) >> 6) &
                                                                  3]);
            (*s)++;
            actor->result = st;
            actor->flags |= 2;
            actor->saved = actor->source;
            ((struct ActorAnimationPart *)(p + 0x238))->flags &= ~four;
            ((struct ActorAnimationPart *)(p + 0x1E4))->flags &= ~four;
            ((struct ActorAnimationPart *)(p + 0x28C))->flags &= ~four;
            ((struct ActorAnimationPart *)(p + 0x190))->flags |= four;
            {
                u32 one = 1;
                ((struct ActorCollisionState *)(p + 0x180))->flags &= ~one;
            }
            actor->flags &= ~0x20;
            Sound_PlaySongChecked(0xC2);
            break;
        }
        case 1:
            if (actor->complete != 0) {
                *s = 0xFF;
                actor->result = st;
            }
            break;
    }
}
