#define ShadowActor_UpdateTransition sub_0819804C
#define ScaledShadow_Enable          ScaledShadow_Enable /* Existing name; stale baseline ELF uses address name. */
#include "global.h"

/* Tentative family: an animated actor with an embedded scaled shadow. */
struct ShadowActor {
    u8 unknown0[8];
    u8 animationData[0x20];
    u8 animation[0x60];
    u8 shadow[0xCC];
    u8 entered;
    u8 phase;
    u8 animationEnded;
    u8 state;
};
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void ScaledShadow_Enable(void *);

void sub_0819804C(struct ShadowActor *actor)
{
    if (!actor->entered) {
        Anim_SetKeyframes(actor->animation, actor->animationData, 5, 2, 0);
        actor->entered++;
    } else if (actor->animationEnded) {
        u32 zero;
        Anim_SetKeyframes(actor->animation, actor->animationData, 0, 2, zero = 0);
        ScaledShadow_Enable(actor->shadow);
        {
            s32 next = actor->state;
            switch (next) {
                case 3:
                    next = 4;
                    break;
                case 6:
                    next = 7;
                    break;
                case 0:
                default:
                    goto idle;
            }
            actor->state = next;
            actor->entered = zero;
            actor->phase = zero;
            return;
        idle: {
            u8 one = 1;
            actor->state = one;
            actor->entered = 0;
            actor->phase = 0;
        }
        }
    }
}
