// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
#define SceneSteeringActor_DestroySlot sub_0806C708
s32 sub_08069D50(const struct ContactPosition *, u32, u32, s32, s32, s32, s32);
void sub_08013B74(void *);
static inline s32 Dimension(u16 *field)
{
    u32 value = *field;
    s32 result = 1;
    if (value)
        result = value;
    return result;
}
void SceneSteeringActor_DestroySlot(struct SceneSteeringActorPool *p,
                                    struct SceneSteeringActor *actor)
{
    struct SceneSteeringActor *early = actor;
    if (actor->enter) {
        register u16 *field asm("r1") = (void *)((u8 *)early + 0xe4);
        s32 width;
        register s32 height asm("r2");
        asm("" : "+r"(field));
        width = Dimension(field);
        field = (void *)((u8 *)actor + 0xe8);
        asm("" : "+r"(field));
        height = Dimension(field);
        asm("" : "+r"(height));
        sub_08069D50(&actor->position, 2, 0, width, height, 40, 16);
        {
            u16 *frame = &actor->stateFrame;
            register u32 zero asm("r8") = 0;
            asm("" : "+r"(frame), "+r"(zero)::"r0");
            *frame = 0;
            {
                register u32 offset asm("r1") = 0x115;
                register u8 *status asm("r5");
                asm("" : "+r"(offset));
                status = (void *)((u8 *)actor + offset);
                if (!*status) {
                    sub_08013B74(actor->shadow);
                    *status = 255;
                }
            }
            Sprite_Remove(actor);
            p->used &= ~(1U << actor->index);
            actor->index = -1;
            {
                register u32 offset asm("r1") = 0x122;
                u8 *state;
                register u32 lowZero asm("r1");
                asm("" : "+r"(offset));
                state = (void *)((u8 *)actor + offset);
                lowZero = zero;
                *state = lowZero;
                actor->enter = lowZero;
            }
        }
    }
}
